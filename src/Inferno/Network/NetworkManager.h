#pragma once

#include "Messages.h"
#include <slikenet/peer.h>
#include <slikenet/MessageIdentifiers.h>
#include <slikenet/BitStream.h>
#include <string_view>
#include <vector>
#include <queue>
#include <mutex>

namespace Inferno::Network {
    constexpr uint16_t LISTEN_PORT = 12345; // Default port for multiplayer

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

        // Check if currently hosting
        bool isHost() const { return m_isHost; }

        // Check if connected (hosting or joined)
        bool isConnected() const { return m_peer != nullptr && m_peer->IsActive(); }

        // Get local player ID (0 for host, 1+ for clients)
        uint8_t getPlayerId() const { return m_playerId; }

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

        // Send a raw message to all connected peers
        void broadcastMessage(MessageID id, const std::vector<uint8_t>& data);

        // Send a raw message to a specific target
        void sendMessage(const SLNet::SystemAddress& target, MessageID id, const std::vector<uint8_t>& data);

        SLNet::RakPeerInterface* m_peer = nullptr;
        bool m_isHost = false;
        uint8_t m_playerId = 0;
        std::queue<SLNet::Packet*> m_packetQueue;
        std::mutex m_queueMutex;
        float m_updateTimer = 0.0f;
        const float m_updateRate = 1.0f / 30.0f; // 30 updates per second
    };
}