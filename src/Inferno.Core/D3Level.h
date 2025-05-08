#pragma once
//#include <System/System.h>
#include <optional>
#include "Object.h"
#include "Level.h"
//#include "Vector3.h"
#include "Types.h"
#include "OutrageTable.h"

//using namespace Inferno;

namespace Inferno::D3 {
    struct roomUVL
    {
        float u;
        float v;
        float u2;
        float v2;
        ubyte alpha;
    };
    class Face
    {
        public:
        List<int16> face_verts;
        List<roomUVL> face_uvls;
        int flags;
        int portal_num;
        int tmapIdx;
        ubyte light_multiple;
    };
    class Portal
    {
        public:
        uint flags;
        int portal_face;
        int croom;
        int cportal;
        int combine_master;
        int bnode_index;
        Vector3 path_pnt;
    };
    class Room
    {
        public:
        string name;
        Vector3 path_pnt;
        uint flags;
        int pulse_time;
        int pulse_offset;
        List<Vector3> verts;
        List<Face> faces;
        List<Portal> portals;
        short mirror_face;
        ubyte env_reverb;
        float damage;
        ubyte damage_type;
        int ambient_sound;
        float fog_depth;
        float fog_r;
        float fog_g;
        float fog_b;
        List<ubyte> volume_lights;
        int volume_width;
        int volume_height;
        int volume_depth;
    };
    struct Object {
        int Handle;
        ObjectType Type;
        int ID;
        string Name;
        uint32 Flags{};
        float Shields{};
        uint32 RoomNum{};
        Vector3 Position;
        Matrix3x3 Rotation;
        ContainsData Contains;
        float Lifespan = FLT_MAX;
        string CustomDefaultScriptName;
        string CustomDefaultModuleName;
    };
    static constexpr uint MkTag(char a, char b, char c, char d) {
        return a | (b << 8) | (c << 16) | (d << 24);
    }
    class D3Level
    {
        int LEVEL_FILE_OLDEST_COMPATIBLE_VERSION;
        int LEVEL_FILE_VERSION;
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
            int ROOM_NAME_LEN;
            int FF_LIGHTMAP;
            int FF_VERTEX_ALPHA;
            int FF_CORONA;
            int FF_TEXTURE_CHANGED;
            int FF_HAS_TRIGGER;
            int FF_SPEC_INVISIBLE;
            int FF_FLOATING_TRIG;
            int FF_DESTROYED;
            int FF_VOLUMETRIC;
            int FF_TRIANGULATED;
            int FF_VISIBLE;
            int FF_NOT_SHELL;
            int FF_TOUCHED;
            int FF_GOALFACE;
            int FF_NOT_FACING;
            int FF_SCORCHED;
            int OLD_FF_PORTAL_TRIG;
            uint OLD_PF_HAS_TRIGGER;
            int RF_FUELCEN;
            int RF_DOOR;
            int RF_EXTERNAL;
            int RF_GOAL1;
            int RF_GOAL2;
            int RF_TOUCHES_TERRAIN;
            int RF_SORTED_INC_Y;
            int RF_GOAL3;
            int RF_GOAL4;
            int RF_FOG;
            int RF_SPECIAL1;
            int RF_SPECIAL2;
            int RF_SPECIAL3;
            int RF_SPECIAL4;
            int RF_SPECIAL5;
            int RF_SPECIAL6;
            int RF_MIRROR_VISIBLE;
            int RF_TRIANGULATE;
            int RF_STROBE;
            int RF_FLICKER;
            int RFM_MINE;
            int RF_INFORM_RELINK_TO_LG;
            int RF_MANUAL_PATH_PNT;
            int RF_WAYPOINT;
            int RF_SECRET;
            int RF_NO_LIGHT;
            int DF_BLASTED;
            int DF_AUTO;
            int DF_LOCKED;
            int DF_KEY_ONLY_ONE;
            int DF_GB_IGNORE_LOCKED;
        static constexpr ubyte Float_to_ubyte(float f);
        static Face ReadFace(StreamReader& r, int version, const List<int>& textureTranslate);
        static Portal ReadPortal(StreamReader& r, int version);
        static List<ubyte> ReadCompressedBytes(StreamReader& r, int total);
        static Room ReadRoom(StreamReader& r, int version, const List<int>& textureTranslate);
        D3Level();
        public:
        string name;
        List<Room> rooms;
        List<D3::Object> objects;
        static std::optional<D3Level> Read(StreamReader& r, const Outrage::GameTable& table);
    };

    constexpr bool IsGeneric(ObjectType type) {
        return type == ObjectType::Robot || type == ObjectType::Powerup ||
            type == ObjectType::Clutter || type == ObjectType::Building;
    }
}
