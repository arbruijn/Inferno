#include "pch.h"
#include "NetworkManager.h"
#include <algorithm>
#include <set>
#include <slikenet/types.h>
#include <slikenet/MessageIdentifiers.h>
#include <slikenet/BitStream.h>
#include <spdlog/spdlog.h>

namespace Inferno::Network {
    namespace {
        std::vector<uint8_t> ReadPayload(SLNet::Packet* packet) {
            SLNet::BitStream bs(packet->data + 1, packet->length - 1, false);
            std::vector<uint8_t> bytes(bs.GetNumberOfBytesUsed());
            if (!bytes.empty())
                bs.ReadBits(bytes.data(), bytes.size() * 8);
            return bytes;
        }
    }

    NetworkManager::NetworkManager() = default;

    NetworkManager::~NetworkManager() {
        disconnect();
    }

    NetworkManager& NetworkManager::Instance() {
        static NetworkManager instance;
        return instance;
    }

    void NetworkManager::setLocalPlayerName(std::string_view name) {
        m_localPlayerName = name.empty() ? "Player" : std::string(name);

        if (m_playerId != 0xFF) {
            setLobbyPlayer(m_playerId, m_localPlayerName);
            if (isConnected())
                sendLocalPlayerInfo();
        }
    }

    bool NetworkManager::startHost(uint16_t port) {
        if (m_peer) {
            SPDLOG_WARN("NetworkManager: Already initialized");
            return false;
        }

        m_peer = SLNet::RakPeerInterface::GetInstance();
        if (!m_peer) {
            SPDLOG_ERROR("NetworkManager: Failed to create RakPeerInterface");
            return false;
        }

        SLNet::SocketDescriptor sd(port, nullptr);
        SLNet::StartupResult result = m_peer->Startup(8, &sd, 1);
        if (result != SLNet::RAKNET_STARTED) {
            SPDLOG_ERROR("NetworkManager: Failed to start host on port {}", port);
            SLNet::RakPeerInterface::DestroyInstance(m_peer);
            m_peer = nullptr;
            return false;
        }

        m_peer->SetMaximumIncomingConnections(8);
        m_isHost = true;
        m_port = port;
        m_playerId = 0; // Host is always player 0
        setLobbyPlayer(m_playerId, m_localPlayerName);
        SPDLOG_INFO("NetworkManager: Hosting on port {}", port);
        return true;
    }

    bool NetworkManager::joinGame(std::string_view hostIp, uint16_t port) {
        if (m_peer) {
            SPDLOG_WARN("NetworkManager: Already initialized");
            return false;
        }

        m_peer = SLNet::RakPeerInterface::GetInstance();
        if (!m_peer) {
            SPDLOG_ERROR("NetworkManager: Failed to create RakPeerInterface");
            return false;
        }

        SLNet::SocketDescriptor sd;
        SLNet::StartupResult result = m_peer->Startup(1, &sd, 1);
        if (result != SLNet::RAKNET_STARTED) {
            SPDLOG_ERROR("NetworkManager: Failed to initialize client");
            SLNet::RakPeerInterface::DestroyInstance(m_peer);
            m_peer = nullptr;
            return false;
        }

        SLNet::ConnectionAttemptResult connectResult = m_peer->Connect(hostIp.data(), port, nullptr, 0);
        if (connectResult != SLNet::CONNECTION_ATTEMPT_STARTED) {
            SPDLOG_ERROR("NetworkManager: Failed to start connection to {}:{}", hostIp, port);
            SLNet::RakPeerInterface::DestroyInstance(m_peer);
            m_peer = nullptr;
            return false;
        }

        m_isHost = false;
        m_port = port;
        // Player ID will be assigned by host upon connection
        m_playerId = 0xFF; // Invalid until assigned
        SPDLOG_INFO("NetworkManager: Attempting to connect to {}:{}", hostIp, port);
        return true;
    }

    void NetworkManager::disconnect() {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        while (!m_packetQueue.empty()) {
            if (m_peer)
                m_peer->DeallocatePacket(m_packetQueue.front());
            m_packetQueue.pop();
        }

        if (m_peer) {
            m_peer->Shutdown(300);
            SLNet::RakPeerInterface::DestroyInstance(m_peer);
            m_peer = nullptr;
        }

        m_isHost = false;
        m_playerId = 0;
        m_nextPlayerId = 1;
        m_updateTimer = 0.0f;
        m_shouldSendPlayerState = false;
        m_port = LISTEN_PORT;
        m_playerAddresses.clear();
        m_remotePlayers.clear();
        m_lobbyPlayers.clear();
    }

    void NetworkManager::update(float dt) {
        if (!m_peer) return;

        // Process incoming packets
        SLNet::Packet* packet = nullptr;
        for (packet = m_peer->Receive(); packet; packet = m_peer->Receive()) {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_packetQueue.push(packet);
        }

        // Process queued packets
        processPackets();

        // Periodic state updates (send player state at fixed rate)
        m_updateTimer += dt;
        if (m_updateTimer >= m_updateRate) {
            m_updateTimer -= m_updateRate;
            m_shouldSendPlayerState = true;
        }

        updateRemotePlayers(dt);
    }

    void NetworkManager::processPackets() {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        while (!m_packetQueue.empty()) {
            SLNet::Packet* packet = m_packetQueue.front();
            m_packetQueue.pop();

            switch (packet->data[0]) {
                case ID_NEW_INCOMING_CONNECTION:
                    SPDLOG_INFO("NetworkManager: Player connected from {}", packet->systemAddress.ToString(true));
                    // If host, assign player ID and send join confirmation
                    if (m_isHost) {
                        const auto playerId = assignPlayerId(packet->systemAddress);
                        sendMessage(packet->systemAddress, MessageID::PlayerJoin, { playerId });
                        sendExistingLobbyPlayers(packet->systemAddress);
                    }
                    break;
                case ID_CONNECTION_REQUEST_ACCEPTED:
                    SPDLOG_INFO("NetworkManager: Connected to host");
                    break;
                case ID_CONNECTION_LOST:
                case ID_DISCONNECTION_NOTIFICATION:
                    SPDLOG_INFO("NetworkManager: Player disconnected from {}", packet->systemAddress.ToString(true));
                    if (m_isHost) {
                        if (auto playerId = findPlayerId(packet->systemAddress)) {
                            broadcastMessage(MessageID::PlayerLeave, { *playerId });
                            removeRemotePlayer(*playerId);
                        }
                    }
                    break;
                case ID_NO_FREE_INCOMING_CONNECTIONS:
                    SPDLOG_WARN("NetworkManager: Host is full");
                    break;
                default:
                    if (packet->data[0] >= static_cast<uint8_t>(MessageID::PlayerJoin)) {
                        // Our custom messages
                        switch (static_cast<MessageID>(packet->data[0])) {
                            case MessageID::PlayerJoin:
                                handlePlayerJoin(packet);
                                break;
                            case MessageID::PlayerLeave:
                                handlePlayerLeave(packet);
                                break;
                            case MessageID::PlayerState:
                                handlePlayerState(packet);
                                break;
                            case MessageID::PlayerDeath:
                                handlePlayerDeath(packet);
                                break;
                            case MessageID::GameStart:
                                handleGameStart(packet);
                                break;
                            case MessageID::PlayerInfo:
                                handlePlayerInfo(packet);
                                break;
                            default:
                                SPDLOG_WARN("NetworkManager: Unknown custom message ID {}", packet->data[0]);
                                break;
                        }
                    }
                    break;
            }

            m_peer->DeallocatePacket(packet);
        }
    }

    void NetworkManager::handlePlayerJoin(SLNet::Packet* packet) {
        auto bytes = ReadPayload(packet);
        if (bytes.empty()) {
            SPDLOG_WARN("NetworkManager: PlayerJoin missing player ID payload");
            return;
        }

        if (!m_isHost) {
            m_playerId = bytes[0];
            setLobbyPlayer(m_playerId, m_localPlayerName);
            sendLocalPlayerInfo();
            SPDLOG_INFO("NetworkManager: Assigned player ID {}", m_playerId);
        }
    }

    void NetworkManager::handlePlayerLeave(SLNet::Packet* packet) {
        auto bytes = ReadPayload(packet);
        if (bytes.empty()) {
            SPDLOG_WARN("NetworkManager: PlayerLeave missing player ID payload");
            return;
        }

        removeRemotePlayer(bytes[0]);
        SPDLOG_INFO("NetworkManager: Player {} left", bytes[0]);
    }

    void NetworkManager::handlePlayerState(SLNet::Packet* packet) {
        auto bytes = ReadPayload(packet);
        PlayerStateMessage state;
        if (Deserialize(bytes, state)) {
            if (state.player_id == m_playerId)
                return;

            auto& remote = m_remotePlayers[state.player_id];
            remote.playerId = state.player_id;
            remote.previousSnapshot = remote.hasSnapshot ? remote.latestSnapshot : state;
            remote.latestSnapshot = state;
            remote.hasPreviousSnapshot = remote.hasSnapshot;
            remote.hasSnapshot = true;
            remote.interpolationAlpha = remote.hasPreviousSnapshot ? 0.0f : 1.0f;
            remote.isDead = false;
            remote.exploded = false;

            if (m_isHost) {
                broadcastMessage(MessageID::PlayerState, bytes);
            }

            SPDLOG_DEBUG("NetworkManager: Received state for player {}", state.player_id);
        } else {
            SPDLOG_WARN("NetworkManager: Failed to deserialize PlayerState");
        }
    }

    void NetworkManager::handlePlayerDeath(SLNet::Packet* packet) {
        auto bytes = ReadPayload(packet);
        PlayerDeathMessage death;
        if (Deserialize(bytes, death)) {
            if (death.player_id == m_playerId)
                return;

            auto& remote = m_remotePlayers[death.player_id];
            remote.playerId = death.player_id;
            remote.isDead = true;
            remote.exploded = death.exploded;

            if (m_isHost) {
                broadcastMessage(MessageID::PlayerDeath, bytes);
            }

            SPDLOG_INFO("NetworkManager: Player {} died (exploded={})", death.player_id, death.exploded);
        } else {
            SPDLOG_WARN("NetworkManager: Failed to deserialize PlayerDeath");
        }
    }

    void NetworkManager::handleGameStart(SLNet::Packet* packet) {
        SPDLOG_INFO("NetworkManager: GameStart received");
        // TODO: start game
    }

    void NetworkManager::handlePlayerInfo(SLNet::Packet* packet) {
        auto bytes = ReadPayload(packet);
        PlayerInfoMessage info;
        if (!Deserialize(bytes, info)) {
            SPDLOG_WARN("NetworkManager: Failed to deserialize PlayerInfo");
            return;
        }

        if (info.name.empty())
            info.name = "Player";

        setLobbyPlayer(info.player_id, info.name);

        if (m_isHost)
            broadcastMessage(MessageID::PlayerInfo, bytes);
    }

    void NetworkManager::broadcastMessage(MessageID id, const std::vector<uint8_t>& data) {
        SLNet::BitStream bs;
        bs.Write(static_cast<uint8_t>(id));
        bs.WriteBits(data.data(), data.size() * 8);

        if (!m_peer) return;

        const auto reliability = id == MessageID::PlayerState ? UNRELIABLE_SEQUENCED : RELIABLE_ORDERED;

        if (m_isHost) {
            m_peer->Send(&bs, HIGH_PRIORITY, reliability, 0, SLNet::UNASSIGNED_SYSTEM_ADDRESS, true);
            return;
        }

        if (m_peer->NumberOfConnections() == 0)
            return;

        m_peer->Send(&bs, HIGH_PRIORITY, reliability, 0, m_peer->GetSystemAddressFromIndex(0), false);
    }

    void NetworkManager::sendMessage(const SLNet::SystemAddress& target, MessageID id, const std::vector<uint8_t>& data) {
        if (!m_peer) return;

        SLNet::BitStream bs;
        bs.Write(static_cast<uint8_t>(id));
        bs.WriteBits(data.data(), data.size() * 8);

        const auto reliability = id == MessageID::PlayerState ? UNRELIABLE_SEQUENCED : RELIABLE_ORDERED;
        m_peer->Send(&bs, HIGH_PRIORITY, reliability, 0, target, false);
    }

    void NetworkManager::broadcastPlayerState(const PlayerStateMessage& state) {
        auto bytes = Serialize(state);
        broadcastMessage(MessageID::PlayerState, bytes);
    }

    void NetworkManager::broadcastPlayerDeath(const PlayerDeathMessage& death) {
        auto bytes = Serialize(death);
        broadcastMessage(MessageID::PlayerDeath, bytes);
    }

    bool NetworkManager::consumePlayerStateSendTick() {
        const bool shouldSend = m_shouldSendPlayerState;
        m_shouldSendPlayerState = false;
        return shouldSend;
    }

    std::vector<std::string> NetworkManager::getLocalAddresses() const {
        if (!m_peer)
            return {};

        std::set<std::string> uniqueAddresses;
        const auto count = m_peer->GetNumberOfAddresses();

        for (unsigned i = 0; i < count; ++i) {
            if (const char* ip = m_peer->GetLocalIP(i)) {
                std::string address = ip;
                if (!address.empty() && address != "127.0.0.1" && address != "::1" && address != "0.0.0.0")
                    uniqueAddresses.insert(std::move(address));
            }
        }

        return { uniqueAddresses.begin(), uniqueAddresses.end() };
    }

    uint8_t NetworkManager::assignPlayerId(const SLNet::SystemAddress& address) {
        for (const auto& [playerId, systemAddress] : m_playerAddresses) {
            if (systemAddress == address)
                return playerId;
        }

        const auto playerId = m_nextPlayerId++;
        m_playerAddresses[playerId] = address;
        return playerId;
    }

    std::optional<uint8_t> NetworkManager::findPlayerId(const SLNet::SystemAddress& address) const {
        for (const auto& [playerId, systemAddress] : m_playerAddresses) {
            if (systemAddress == address)
                return playerId;
        }

        return std::nullopt;
    }

    void NetworkManager::removeRemotePlayer(uint8_t playerId) {
        m_playerAddresses.erase(playerId);
        m_remotePlayers.erase(playerId);
        m_lobbyPlayers.erase(playerId);
    }

    void NetworkManager::setLobbyPlayer(uint8_t playerId, std::string name) {
        auto& player = m_lobbyPlayers[playerId];
        player.playerId = playerId;
        player.name = name.empty() ? "Player" : std::move(name);
    }

    void NetworkManager::sendExistingLobbyPlayers(const SLNet::SystemAddress& target) {
        for (const auto& [playerId, player] : m_lobbyPlayers) {
            sendMessage(target, MessageID::PlayerInfo, Serialize(PlayerInfoMessage{
                .player_id = playerId,
                .name = player.name,
            }));
        }
    }

    void NetworkManager::sendLocalPlayerInfo() {
        if (m_playerId == 0xFF)
            return;

        broadcastMessage(MessageID::PlayerInfo, Serialize(PlayerInfoMessage{
            .player_id = m_playerId,
            .name = m_localPlayerName,
        }));
    }

    void NetworkManager::updateRemotePlayers(float dt) {
        for (auto& [_, player] : m_remotePlayers) {
            if (!player.hasSnapshot)
                continue;

            if (!player.hasPreviousSnapshot) {
                player.interpolationAlpha = 1.0f;
                continue;
            }

            player.interpolationAlpha = std::min(player.interpolationAlpha + dt / m_updateRate, 1.0f);
        }
    }
}
