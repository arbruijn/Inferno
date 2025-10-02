#pragma once
#include "pch.h"

#include <optional>
#include "Object.h"
#include "Level.h"
//#include "Vector3.h"
#include "Types.h"
#include "OutrageTable.h"
#include "Streams.h"


//using namespace Inferno;

namespace Inferno::Outrage {
    static constexpr uint TERRAIN_WIDTH = 256;
    static constexpr uint TERRAIN_DEPTH = 256;
    static constexpr uint TERRAIN_TEX_WIDTH = 32;
    static constexpr uint TERRAIN_TEX_DEPTH = 32;
    static constexpr uint OCCLUSION_SIZE = 16;

    struct VertexData
    {
        float U;
        float V;
        float U2;
        float V2;
        ubyte Alpha = 255;
    };

    struct TerrainSky
    {
        float FogScalar;
        float DamagePerSecond;

        ubyte Textured;

        int16 DomeTexture;

        uint32 SkyColor;
        uint32 HorizonColor;
        uint32 FogColor;

        uint32 Flags;
        float Radius;
        float RotateRate;
    };

    struct Satellite
    {
        int16 Texture;
        Vector3 Position;
        ubyte Flags;
        float Size;
        Vector3 Color; 
    };

    enum class FaceFlags : uint16 {
        Lightmap = 0x0001,
        VertexAlpha = 0x0002,
        Corona = 0x0004,
        TextureChanged = 0x0008,
        HasTrigger = 0x0010,
        SpecInvisible = 0x0020,
        FloatingTrigger = 0x0040,
        Destroyed = 0x0080,
        Volumetric = 0x0100,
        Triangulated = 0x0200,
        Visible = 0x0400,
        Not_shell = 0x0800,
        Touched = 0x1000,
        Goalface = 0x2000,
        Not_facing = 0x4000,
        Scorched = 0x8000,
        OldPortalTrigger = 0x0020
    };

    struct Face
    {
        List<int16> VerticesIndex;
        List<VertexData> VerticesData;
        FaceFlags Flags;
        int Portal;
        int Texture;
        ubyte LightMultiple;
    };

    struct Portal
    {
        uint Flags;
        int Face;
        int OppositeRoom;
        int OppositePortal;
        int combine_master;
        int bnode_index;
        Vector3 PathPoint;
    };

    enum class RoomFlags : uint {
        Fuelcen = 1,
        Door = (1 << 1),
        External = (1 << 2),
        Goal1 = (1 << 3),
        Goal2 = (1 << 4),
        TouchesTerrain = (1 << 5),
        SortedIncY = (1 << 6),
        Goal3 = (1 << 7),
        Goal4 = (1 << 8),
        Fog = (1 << 9),
        Special1 = (1 << 10),
        Special2 = (1 << 11),
        Special3 = (1 << 12),
        Special4 = (1 << 13),
        Special5 = (1 << 14),
        Special6 = (1 << 15),
        MirrorVisible = (1 << 16),
        Triangulate = (1 << 17),
        Strobe = (1 << 18),
        Flicker = (1 << 19),
        MineMask = (0x1f << 20),
        InformRelinkToLG = (1 << 25),
        ManualPathPnt = (1 << 26),
        Waypoint = (1 << 27),
        Secret = (1 << 28),
        NoLight = (1 << 29)
    };
    
    enum class ObjectFlags : uint {
        ForceCeilingCheck = 0x00000001,
        Dead = 0x00000002,
        Destroyed = 0x00000004,
        StoppedThisFrame = 0x00000008,
        Attached = 0x00000010,
        MovedThisFrame = 0x00000020,
        AiDoDeath = 0x00000040,
        UsesLifeleft = 0x00000080,
        SafeToRender = 0x00000100,
        OutsideMine = 0x00000200,
        Destroyable = 0x00000400,
        BigObject = 0x00000800,
        PolygonObject = 0x00001000,
        Dying = 0x00002000,
        UseDestroyedPolyModel = 0x00004000,
        Rendered = 0x00008000,
        NoObjectCollisions = 0x00010000,
        StuckOnPortal = 0x00020000,
        TempGravity = 0x00040000,
        ClientKnows = 0x00080000,
        ServerSaysDelete = 0x00100000,
        ServerObject = 0x00200000,
        PingAccelerate = 0x00400000,
        AiDeath = 0x00800000,
        SendMultiRemoveOnDeath = 0x01000000,
        SendMultiRemoveOnDeathWs = 0x02000000,
        Predicted = 0x04000000,
        InPlayerInventory = 0x08000000,
        InformPlayerCollideToLg = 0x10000000,
        InformPlayerWeaponCollideToLg = 0x20000000,
        InformDestroyToLg = 0x40000000,
        ClientDemoObject = 0x80000000
    };

    struct Room
    {
        string Name;
        Vector3 PathPoint;
        RoomFlags Flags;
        int PulseTime;
        int PulseOffset;
        List<Vector3> Vertices;
        List<Face> Faces;
        List<Portal> Portals;
        short MirrorFace;
        ubyte EnvironmentReverb;
        float Damage;
        ubyte DamageType;
        int AmbientSound;
        float FogDepth;
        Vector3 FogColor;
        List<ubyte> VolumeLights;
        int VolumeWidth;
        int VolumeHeight;
        int VolumeDepth;
    };
    struct Object {
        int Handle;
        ObjectType Type;
        int ID;
        string Name;
        Outrage::ObjectFlags Flags{};
        float Shields{};
        uint32 RoomNum{};
        Vector3 Position;
        Matrix3x3 Rotation;
        ContainsData Contains;
        float Lifespan;
        string CustomDefaultScriptName;
        string CustomDefaultModuleName;
    };

    struct ESegSide {
        int16 ChildSegment;
        int16 TMap;
        Array<Vector2, 4> UVs;
    };

    struct ESegSegment {
        Array<int16, 8> Indices;
        Array<ESegSide, 6> Sides;
    };

    struct ESegInfo {
        List<Vector3> Vertices;
        List<ESegSegment> Segments;
    };

    static constexpr uint MkTag(char a, char b, char c, char d) {
        return a | (b << 8) | (c << 16) | (d << 24);
    }
    static constexpr uint LEVEL_FILE_OLDEST_COMPATIBLE_VERSION = 13;
    static constexpr uint LEVEL_FILE_VERSION = 128;
    static constexpr uint LEVEL_FILE_TAG = MkTag('D', '3', 'L', 'V');
    static constexpr uint CHUNK_TEXTURE_NAMES = MkTag('T', 'X', 'N', 'M');
    static constexpr uint CHUNK_GENERIC_NAMES = MkTag('G', 'N', 'N', 'M');
    static constexpr uint CHUNK_ROBOT_NAMES = MkTag('R', 'B', 'N', 'M');
    static constexpr uint CHUNK_POWERUP_NAMES = MkTag('P', 'W', 'N', 'M');
    static constexpr uint CHUNK_DOOR_NAMES = MkTag('D', 'R', 'N', 'M');
    static constexpr uint CHUNK_ROOMS = MkTag('R', 'O', 'O', 'M');
    static constexpr uint CHUNK_ROOM_WIND = MkTag('R', 'W', 'N', 'D');
    static constexpr uint CHUNK_OBJECTS = MkTag('O', 'B', 'J', 'S');
    static constexpr uint CHUNK_TERRAIN = MkTag('T', 'E', 'R', 'R');
    static constexpr uint CHUNK_EDITOR_INFO = MkTag('E', 'D', 'I', 'T');
    static constexpr uint CHUNK_SCRIPT = MkTag('S', 'C', 'P', 'T');
    static constexpr uint CHUNK_TERRAIN_HEIGHT = MkTag('T', 'E', 'R', 'H');
    static constexpr uint CHUNK_TERRAIN_TMAPS_FLAGS = MkTag('T', 'E', 'T', 'M');
    static constexpr uint CHUNK_TERRAIN_LINKS = MkTag('T', 'L', 'N', 'K');
    static constexpr uint CHUNK_TERRAIN_SKY = MkTag('T', 'S', 'K', 'Y');
    static constexpr uint CHUNK_TERRAIN_END = MkTag('T', 'E', 'N', 'D');
    static constexpr uint CHUNK_SCRIPT_CODE = MkTag('C', 'O', 'D', 'E');
    static constexpr uint CHUNK_TRIGGERS = MkTag('T', 'R', 'I', 'G');
    static constexpr uint CHUNK_LIGHTMAPS = MkTag('L', 'M', 'A', 'P');
    static constexpr uint CHUNK_BSP = MkTag('C', 'B', 'S', 'P');
    static constexpr uint CHUNK_OBJECT_HANDLES = MkTag('O', 'H', 'N', 'D');
    static constexpr uint CHUNK_GAME_PATHS = MkTag('P', 'A', 'T', 'H');
    static constexpr uint CHUNK_BOA = MkTag('C', 'B', 'O', 'A');
    static constexpr uint CHUNK_NEW_BSP = MkTag('C', 'N', 'B', 'S');
    static constexpr uint CHUNK_LEVEL_INFO = MkTag('I', 'N', 'F', 'O');
    static constexpr uint CHUNK_PLAYER_STARTS = MkTag('P', 'S', 'T', 'R');
    static constexpr uint CHUNK_MATCEN_DATA = MkTag('M', 'T', 'C', 'N');
    static constexpr uint CHUNK_LEVEL_GOALS = MkTag('L', 'V', 'L', 'G');
    static constexpr uint CHUNK_ROOM_AABB = MkTag('A', 'A', 'B', 'B');
    static constexpr uint CHUNK_NEW_LIGHTMAPS = MkTag('N', 'L', 'M', 'P');
    static constexpr uint CHUNK_ALIFE_DATA = MkTag('L', 'I', 'F', 'E');
    static constexpr uint CHUNK_TERRAIN_SOUND = MkTag('T', 'S', 'N', 'D');
    static constexpr uint CHUNK_BNODES = MkTag('N', 'O', 'D', 'E');
    static constexpr uint CHUNK_OVERRIDE_SOUNDS = MkTag('O', 'S', 'N', 'D');
    static constexpr uint CHUNK_FFT_MOD = MkTag('F', 'F', 'T', 'M');
    static constexpr uint CHUNK_ESEG = MkTag('E', 'S', 'E', 'G');

    class OutrageLevel
    {
            int ROOM_NAME_LEN;

        //uint OLD_PF_HAS_TRIGGER;


/*            int DF_BLASTED;
            int DF_AUTO;
            int DF_LOCKED;
            int DF_KEY_ONLY_ONE;
            int DF_GB_IGNORE_LOCKED;*/
        static constexpr ubyte Float_to_ubyte(float f);
        static Face ReadFace(StreamReader& r, int version, const List<int>& textureTranslate);
        static Portal ReadPortal(StreamReader& r, int version);
        static List<ubyte> ReadCompressedBytes(StreamReader& r, int total);
        static Room ReadRoom(StreamReader& r, int version, const List<int>& textureTranslate);
        void WriteRooms(StreamWriter& f);
        void WriteTerrainHeight(StreamWriter& f);
        void WriteTerrainSky(StreamWriter& f, const GameTable& table);
        void WriteTerrainTMapsFlags(StreamWriter& f);
        void WriteTerrain(StreamWriter& f, const GameTable& table);
        void WriteObject(StreamWriter& f, const Outrage::Object& obj);
        void WriteObjects(StreamWriter& f);
        void WriteEditorSegments(StreamWriter& f);
    public:
        OutrageLevel();
        string Name;
        List<Room> Rooms;
        List<Outrage::Object> Objects;
        ESegInfo EditorSegments;
        static std::optional<OutrageLevel> Read(StreamReader& r, const Outrage::GameTable& table);
        void Write(StreamWriter& f, const Outrage::GameTable& table);
    };

    constexpr bool IsGeneric(ObjectType type) {
        return type == ObjectType::Robot || type == ObjectType::Powerup ||
            type == ObjectType::Clutter || type == ObjectType::Building;
    }
}
