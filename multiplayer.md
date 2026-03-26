# Multiplayer Support Plan

## Overview
Implement peer-to-peer multiplayer using SLikeNet (networking) and alpaca (serialization).

## Dependencies

### SLikeNet
- Located in `deps/SLikeNet`
- Based on RakNet, provides UDP-based networking
- Supports peer-to-peer connections
- Must be built as a static library

### alpaca  
- Header-only serialization library
- Add to `deps/alpaca` as a header-only dependency
- No build required

## Network Architecture

### Connection Model
- **P2P Architecture**: One player hosts, others connect via IP address
- **Host** creates game, listens for connections on port `LISTEN_PORT`
- **Clients** connect to host IP on same port
- Uses UDP with SLikeNet's `RakPeerInterface`

### Message Types (MessageIDs)
```cpp
enum class MessageID : uint8_t {
    PLAYER_JOIN = 100,        // Player joining game
    PLAYER_LEAVE = 101,       // Player leaving game
    PLAYER_STATE = 102,       // Position/rotation update
    PLAYER_DEATH = 103,       // Player died
    GAME_START = 104,         // Host started the game
};
```

### Data Structures (for alpaca)
```cpp
struct PlayerStateMessage {
    uint8_t player_id;
    float position_x, position_y, position_z;
    float rotation_m11, rotation_m12, rotation_m13; // etc
    float velocity_x, velocity_y, velocity_z;
    uint8_t animation_state;
};

struct PlayerDeathMessage {
    uint8_t player_id;
    bool exploded; // Player exploded after death
};
```

## Implementation Plan

### Phase 1: Dependencies & CMake
1. Add SLikeNet to git submodules and `CMakeLists.txt`
2. Add alpaca as header-only include directory
3. Update `src/Inferno/CMakeLists.txt` to link SLikeNet

### Phase 2: Network Manager
Create `src/Inferno/Network/NetworkManager.h`:
- Singleton class managing all network operations
- `startHost()`
- `joinGame(std::string_view hostIp)`
- `disconnect()`
- Update loop per frame
- Queue for incoming packets

### Phase 3: Message Handling
Create message serialization with alpaca:
- `serializePlayerState()` - Pack player position/rotation
- `deserializePlayerState()` - Unpack player state
- Handle player death broadcast

### Phase 4: Game Integration
1. Add `NetworkManager` to `Game` module
2. Send player position multiple times per second (~30Hz)
3. Broadcast death events to all peers
4. Handle remote player position interpolation

### Phase 5: UI Integration (Menu)
Add multiplayer menu option:
- "Host Game" - Creates session, shows IP to other players
- "Join Game" - Input dialog for host IP address
- "Disconnect" - Leave current session

## Key Technical Details

### Player Identification
- `player_id` enumerates connected players (0 = host, 1, 2, 3... = clients)
- Host's player is always ID 0
- Assign IDs on connect

### Position Synchronization
- Send position directly without prediction (for simplicity)
- Remote players render at received positions (not interpolated to keep simple)
- Rate: 30 updates/second

### Death Handling
- When `Player::IsDead` becomes true → broadcast `PLAYER_DEATH`
- Other players see dead player as ghost/spectator
- Respawn synced across all clients

## Files to Create/Modify

### New Files
- `src/Inferno/Network/NetworkManager.h`
- `src/Inferno/Network/NetworkManager.cpp`
- `src/Inferno/Network/Messages.h`

### Modified Files
- `CMakeLists.txt` - Add dependencies
- `src/Inferno/CMakeLists.txt` - Link SLikeNet
- `src/Inferno/Game.h` - Add NetworkManager 
- `Game.cpp` or `Game.Player.cpp` - Network updates

## Build Considerations
- SLikeNet requires network libraries (ws2_32 on Windows, socket libs on Linux)
- Update root CMakeLists.txt to fetch SLikeNet
- Handle platform differences in linking

## Testing Checklist
- [ ] Local host connection works
- [ ] Across network connection works
- [ ] Player positions sync smoothly
- [ ] Death broadcast receives properly
- [ ] Disconnect handled gracefully