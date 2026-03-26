#include "pch.h"
#include "NetworkManager.h"
#include <slikenet/types.h>
#include <slikenet/MessageIdentifiers.h>
#include <slikenet/BitStream.h>
#include <spdlog/spdlog.h>

namespace Inferno::Network {
    NetworkManager::NetworkManager() = default;

    NetworkManager::~NetworkManager() {
        disconnect();
    }

    NetworkManager& NetworkManager::Instance() {
        static NetworkManager instance;
        return instance;
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
        m_playerId = 0; // Host is always player 0
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
        // Player ID will be assigned by host upon connection
        m_playerId = 0xFF; // Invalid until assigned
        SPDLOG_INFO("NetworkManager: Attempting to connect to {}:{}", hostIp, port);
        return true;
    }

    void NetworkManager::disconnect() {
        if (m_peer) {
            m_peer->Shutdown(300);
            SLNet::RakPeerInterface::DestroyInstance(m_peer);
            m_peer = nullptr;
        }
        m_isHost = false;
        m_playerId = 0;
        // Clear packet queue
        std::lock_guard<std::mutex> lock(m_queueMutex);
        while (!m_packetQueue.empty()) {
            m_peer->DeallocatePacket(m_packetQueue.front());
            m_packetQueue.pop();
        }
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
            m_updateTimer = 0.0f;
            // TODO: Trigger sending of player state from game logic
        }
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
                        // TODO: assign player ID and send PLAYER_JOIN
                    }
                    break;
                case ID_CONNECTION_REQUEST_ACCEPTED:
                    SPDLOG_INFO("NetworkManager: Connected to host");
                    // TODO: request player ID assignment
                    break;
                case ID_CONNECTION_LOST:
                case ID_DISCONNECTION_NOTIFICATION:
                    SPDLOG_INFO("NetworkManager: Player disconnected from {}", packet->systemAddress.ToString(true));
                    // TODO: handle player leave
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
        // TODO: deserialize PlayerStateMessage? Actually join message may just contain player ID.
        // For now, just log.
        SPDLOG_INFO("NetworkManager: PlayerJoin from {}", packet->systemAddress.ToString(true));
    }

    void NetworkManager::handlePlayerLeave(SLNet::Packet* packet) {
        SPDLOG_INFO("NetworkManager: PlayerLeave from {}", packet->systemAddress.ToString(true));
    }

    void NetworkManager::handlePlayerState(SLNet::Packet* packet) {
        SLNet::BitStream bs(packet->data + 1, packet->length - 1, false);
        std::vector<uint8_t> bytes(bs.GetNumberOfBytesUsed());
        bs.ReadBits(bytes.data(), bytes.size() * 8);

        PlayerStateMessage state;
        if (Deserialize(bytes, state)) {
            // TODO: apply state to remote player
            SPDLOG_DEBUG("NetworkManager: Received state for player {}", state.player_id);
        } else {
            SPDLOG_WARN("NetworkManager: Failed to deserialize PlayerState");
        }
    }

    void NetworkManager::handlePlayerDeath(SLNet::Packet* packet) {
        SLNet::BitStream bs(packet->data + 1, packet->length - 1, false);
        std::vector<uint8_t> bytes(bs.GetNumberOfBytesUsed());
        bs.ReadBits(bytes.data(), bytes.size() * 8);

        PlayerDeathMessage death;
        if (Deserialize(bytes, death)) {
            // TODO: handle death
            SPDLOG_INFO("NetworkManager: Player {} died (exploded={})", death.player_id, death.exploded);
        } else {
            SPDLOG_WARN("NetworkManager: Failed to deserialize PlayerDeath");
        }
    }

    void NetworkManager::handleGameStart(SLNet::Packet* packet) {
        SPDLOG_INFO("NetworkManager: GameStart received");
        // TODO: start game
    }

    void NetworkManager::broadcastMessage(MessageID id, const std::vector<uint8_t>& data) {
        if (!m_peer || !m_isHost) return;

        SLNet::BitStream bs;
        bs.Write(static_cast<uint8_t>(id));
        bs.WriteBits(data.data(), data.size() * 8);

        m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, SLNet::UNASSIGNED_SYSTEM_ADDRESS, true);
    }

    void NetworkManager::sendMessage(const SLNet::SystemAddress& target, MessageID id, const std::vector<uint8_t>& data) {
        if (!m_peer) return;

        SLNet::BitStream bs;
        bs.Write(static_cast<uint8_t>(id));
        bs.WriteBits(data.data(), data.size() * 8);

        m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, target, false);
    }

    void NetworkManager::broadcastPlayerState(const PlayerStateMessage& state) {
        auto bytes = Serialize(state);
        broadcastMessage(MessageID::PlayerState, bytes);
    }

    void NetworkManager::broadcastPlayerDeath(const PlayerDeathMessage& death) {
        auto bytes = Serialize(death);
        broadcastMessage(MessageID::PlayerDeath, bytes);
    }
}