#pragma once

#include "Messages.h"
#include "Difficulty.h"
#include "Mission.h"
#include "Types.h"
#include <optional>
#include <unordered_map>
#include <slikenet/peer.h>
#include <slikenet/MessageIdentifiers.h>
#include <slikenet/BitStream.h>
#include <string_view>
#include <vector>
#include <queue>
#include <mutex>

namespace Inferno::Network {
    constexpr uint16_t LISTEN_PORT = 12345; // Default port for multiplayer

    struct RemotePlayerState {
        uint8_t playerId = 0xFF;
        PlayerStateMessage previousSnapshot{};
        PlayerStateMessage latestSnapshot{};
        float interpolationAlpha = 1.0f;
        bool hasSnapshot = false;
        bool hasPreviousSnapshot = false;
        bool isDead = false;
        bool exploded = false;
    };

    struct LobbyPlayer {
        uint8_t playerId = 0xFF;
        std::string name;
    };

    struct LobbyGameSelection {
        std::string missionPath;
        std::string missionName;
        int level = 1;
        DifficultyLevel difficulty = DifficultyLevel::Hotshot;
    };

    class NetworkManager {
    public:
        // Get singleton instance
        static NetworkManager& Instance();

        // Delete copy/move constructors
        NetworkManager(const NetworkManager&) = delete;
        NetworkManager& operator=(const NetworkManager&) = delete;

        // Start hosting a game (listen for connections)
        bool startHost(uint16_t port = LISTEN_PORT);

        // Join an existing game
        bool joinGame(std::string_view hostIp, uint16_t port = LISTEN_PORT);

        // Disconnect from network session
        void disconnect();

        // Update network state (call once per frame)
        void update(float dt);

        // Send player state to all peers
        void broadcastPlayerState(const PlayerStateMessage& state);

        // Send player death to all peers
        void broadcastPlayerDeath(const PlayerDeathMessage& death);

        // Returns true when the game should send another local player snapshot.
        bool consumePlayerStateSendTick();

        // Check if currently hosting
        bool isHost() const { return m_isHost; }

        // Check if connected (hosting or joined)
        bool isConnected() const { return m_peer != nullptr && m_peer->IsActive(); }

        // Get local player ID (0 for host, 1+ for clients)
        uint8_t getPlayerId() const { return m_playerId; }

        void setLocalPlayerName(std::string_view name);
        const std::string& getLocalPlayerName() const { return m_localPlayerName; }

        // Returns the local IPv4/IPv6 addresses reported by the active peer.
        std::vector<std::string> getLocalAddresses() const;

        uint16_t getPort() const { return m_port; }

        const std::unordered_map<uint8_t, RemotePlayerState>& getRemotePlayers() const { return m_remotePlayers; }
        const std::unordered_map<uint8_t, LobbyPlayer>& getLobbyPlayers() const { return m_lobbyPlayers; }
        const std::optional<LobbyGameSelection>& getLobbyGameSelection() const { return m_lobbyGameSelection; }
        const std::unordered_map<uint8_t, ObjID>& getPlayerObjectIds() const { return m_playerObjectIds; }

        bool hasPlayer(uint8_t playerId) const;
        void clearPlayerObjectIds();
        void setPlayerObjectId(uint8_t playerId, ObjID objectId);
        std::optional<ObjID> getPlayerObjectId(uint8_t playerId) const;

        void setHostedGameSelection(const MissionInfo& mission, int level, DifficultyLevel difficulty);
        void startHostedGame();

    private:
        NetworkManager();
        ~NetworkManager();

        // Process incoming packets
        void processPackets();

        // Handle specific message types
        void handlePlayerJoin(SLNet::Packet* packet);
        void handlePlayerLeave(SLNet::Packet* packet);
        void handlePlayerState(SLNet::Packet* packet);
        void handlePlayerDeath(SLNet::Packet* packet);
        void handleGameStart(SLNet::Packet* packet);
        void handlePlayerInfo(SLNet::Packet* packet);
        void handleGameConfig(SLNet::Packet* packet);
        uint8_t assignPlayerId(const SLNet::SystemAddress& address);
        std::optional<uint8_t> findPlayerId(const SLNet::SystemAddress& address) const;
        void removeRemotePlayer(uint8_t playerId);
        void setLobbyPlayer(uint8_t playerId, std::string name);
        void sendExistingLobbyPlayers(const SLNet::SystemAddress& target);
        void sendLocalPlayerInfo();
        void sendGameSelection(const SLNet::SystemAddress& target);
        void updateRemotePlayers(float dt);

        // Send a raw message to all connected peers
        void broadcastMessage(MessageID id, const std::vector<uint8_t>& data);

        // Send a raw message to a specific target
        void sendMessage(const SLNet::SystemAddress& target, MessageID id, const std::vector<uint8_t>& data);

        SLNet::RakPeerInterface* m_peer = nullptr;
        bool m_isHost = false;
        uint8_t m_playerId = 0;
        std::string m_localPlayerName = "Player";
        std::queue<SLNet::Packet*> m_packetQueue;
        std::mutex m_queueMutex;
        float m_updateTimer = 0.0f;
        bool m_shouldSendPlayerState = false;
        const float m_updateRate = 1.0f / 2.0f; // 30 updates per second
        uint16_t m_port = LISTEN_PORT;
        uint8_t m_nextPlayerId = 1;
        std::unordered_map<uint8_t, SLNet::SystemAddress> m_playerAddresses;
        std::unordered_map<uint8_t, RemotePlayerState> m_remotePlayers;
        std::unordered_map<uint8_t, LobbyPlayer> m_lobbyPlayers;
        std::unordered_map<uint8_t, ObjID> m_playerObjectIds;
        std::optional<LobbyGameSelection> m_lobbyGameSelection;
    };
}
