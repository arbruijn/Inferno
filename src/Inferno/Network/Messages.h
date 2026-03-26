#pragma once

#include <cstdint>
#include <alpaca/alpaca.h>
#include <slikenet/types.h>

namespace Inferno::Network {
    // Message IDs for network communication
    enum class MessageID : uint8_t {
        PlayerJoin = 100,
        PlayerLeave = 101,
        PlayerState = 102,
        PlayerDeath = 103,
        GameStart = 104,
        PlayerInfo = 105,
        GameConfig = 106,
    };

    // Player state update message
    struct PlayerStateMessage {
        uint8_t player_id;
        float position_x, position_y, position_z;
        float rotation_m11, rotation_m12, rotation_m13;
        float rotation_m21, rotation_m22, rotation_m23;
        float rotation_m31, rotation_m32, rotation_m33;
        float velocity_x, velocity_y, velocity_z;
        uint8_t animation_state;
    };

    // Player death message
    struct PlayerDeathMessage {
        uint8_t player_id;
        bool exploded; // Player exploded after death
    };

    struct PlayerInfoMessage {
        uint8_t player_id;
        std::string name;
    };

    struct GameConfigMessage {
        std::string mission_path;
        std::string mission_name;
        int level_number = 1;
        uint8_t difficulty = 0;
    };

    // Serialization functions using alpaca
    template<typename T>
    std::vector<uint8_t> Serialize(const T& data) {
        std::vector<uint8_t> bytes;
        alpaca::serialize(data, bytes);
        return bytes;
    }

    template<typename T>
    bool Deserialize(const std::vector<uint8_t>& bytes, T& data) {
        std::error_code ec;
        data = alpaca::deserialize<T>(bytes, ec);
        return !ec;
    }
}
