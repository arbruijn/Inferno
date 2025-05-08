#include <functional>
#include "Streams.h"
#include "D3Level.h"
#include "OutrageTable.h"

#include <string>
using std::optional, std::nullopt;
constexpr auto PAGENAME_LEN = 35;

using namespace Inferno::Outrage;

namespace Inferno::D3 {
    constexpr ubyte D3Level::Float_to_ubyte(float f)
    {
        return static_cast<ubyte>(f * 255.0f);
    }

    // Reads a null terminated string
    static string ReadString(StreamReader& r) {
        List<char> b{};
        char c;
        while ((c = (char)r.ReadByte()) != 0)
            b.push_back(c);
        return string(b.begin(), b.end());
    }

    Face D3Level::ReadFace(StreamReader& r, int version, const List<int>& textureTranslate)
    {
        Face face{};
        int vertexCount = r.ReadByte();
        face.face_verts.resize(vertexCount);
        for (int i = 0; i < vertexCount; i++)
            face.face_verts[i] = r.ReadInt16();
        bool alphaed = false;
        face.face_uvls.resize(vertexCount);
        for (int j = 0; j < vertexCount; j++) {
            face.face_uvls[j].u = r.ReadFloat();
            face.face_uvls[j].v = r.ReadFloat();
            if (version < 56) {
                r.ReadFloat();
                r.ReadFloat();
                r.ReadFloat();
                r.ReadFloat();
            }
            if (version >= 21) {
                if (version < 61) {
                    face.face_uvls[j].alpha = D3Level::Float_to_ubyte(r.ReadFloat());
                }
                else {
                    face.face_uvls[j].alpha = r.ReadByte();
                }
            }
            else {
                face.face_uvls[j].alpha = 255;
            }
            if (face.face_uvls[j].alpha != 255) {
                alphaed = true;
            }
        }
        if (version < 27)
            face.flags = r.ReadByte();
        else
            face.flags = r.ReadInt16();
        if (version < 103)
            face.flags = face.flags & -33;
        if (alphaed)
            face.flags = face.flags | 2;
        else
            face.flags = face.flags & -3;
        if (version >= 23) 
            face.portal_num = r.ReadByte();
        else
            face.portal_num = r.ReadInt16();
        face.tmapIdx = textureTranslate[r.ReadInt16()];
        if (((face.flags & 1) != 0) && (version >= 19)) {
            if (version <= 29) {
                int w = r.ReadByte();
                int h = r.ReadByte();
                for (int k = 0; k < (w * h); k++) {
                    r.ReadInt16();
                }
                face.flags = face.flags & -2;
            }
            else {
                /*int num4 = (int)*/r.ReadUInt16();
                if (version <= 88) {
                    r.ReadByte();
                    r.ReadByte();
                    r.ReadByte();
                    r.ReadByte();
                }
            }
            for (int l = 0; l < vertexCount; l++) {
                face.face_uvls[l].u2 = r.ReadFloat();
                face.face_uvls[l].v2 = r.ReadFloat();
                if (face.face_uvls[l].u2 < 0)
                    face.face_uvls[l].u2 = 0;
                if (face.face_uvls[l].u2 > 1)
                    face.face_uvls[l].u2 = 1;
                if (face.face_uvls[l].v2 < 0)
                    face.face_uvls[l].v2 = 0;
                if (face.face_uvls[l].v2 > 1)
                    face.face_uvls[l].v2 = 1;
            }
        }
        if ((version >= 22) && (version <= 29)) {
            r.ReadVector3();
        }
        if ((version >= 40) && (version <= 60)) {
            r.ReadInt16();
            r.ReadInt16();
        }
        if (version >= 50) {
            face.light_multiple = r.ReadByte();
            if (face.light_multiple == 186) {
                face.light_multiple = 4;
            }
            if (version <= 52) {
                face.light_multiple *= 4;
            }
        }
        else {
            face.light_multiple = 4;
        }
        if (version >= 71) {
            ubyte special = r.ReadByte();
            if (special != 0) {
                if (version < 77) {
                    r.ReadByte();
                    r.ReadVector3();
                    r.ReadInt16();
                }
                else {
                    ubyte smoothVertCount = 0;
                    /*ubyte type =*/ r.ReadByte();
                    ubyte instCount = r.ReadByte();
                    ubyte smooth;
                    if (version >= 117) {
                        smooth = r.ReadByte();
                        if (smooth != 0) {
                            smoothVertCount = r.ReadByte();
                        }
                        else {
                            smooth = 0;
                        }
                    }
                    else {
                        smooth = 0;
                    }
                    for (int m = 0; m < instCount; m++) {
                        /*Vector3 vector =*/ r.ReadVector3();
                        /*ushort num5 =*/ r.ReadUInt16();
                    }
                    if (smooth != 0) {
                        for (int n = 0; n < smoothVertCount; n++) {
                            /*Vector3 vector2 =*/ r.ReadVector3();
                        }
                    }
                }
            }
        }
        return face;
    }
    Portal D3Level::ReadPortal(StreamReader& r, int version)
    {
        Portal portal{};
        portal.flags = r.ReadUInt32();
        if (version < 103) {
            portal.flags = portal.flags & 4294967291u;
        }
        if (version < 80) {
            int num = r.ReadInt16();
            for (int i = 0; i < num; i++)
                r.ReadInt16();
            /*int num2 = */r.ReadInt16();
        }
        portal.portal_face = (int)(r.ReadInt16());
        portal.croom = r.ReadInt32();
        portal.cportal = r.ReadInt32();
        if (version >= 123) {
            portal.bnode_index = (int)(r.ReadInt16());
        }
        else {
            portal.bnode_index = -1;
        }
        if (version >= 63) {
            portal.path_pnt = r.ReadVector3();
        }
        if (version >= 100) {
            portal.combine_master = r.ReadInt32();
        }
        return portal;
    }
    List<ubyte> D3Level::ReadCompressedBytes(StreamReader& r, int total)
    {
        List<ubyte> bytes{};
        bytes.resize(total);
        if (r.ReadByte() == 0) {
            r.ReadBytes(bytes);
            return bytes;
        }
        int i = 0;
        while (i < total) {
            int count = r.ReadByte();
            ubyte value = r.ReadByte();
            if (count == 0) {
                bytes[i++] = value;
            } else {
                if (count < 2 || count >= 250)
                    throw new Exception("Invalid compressed data");
                for (int j = 0; j < count; j++)
                    bytes[i++] = value;
            }
        }
        return bytes;
    }
    Room D3Level::ReadRoom(StreamReader& r, int version, const List<int>& textureTranslate)
    {
        Room room{};
        int vertexCount = r.ReadInt32();
        int faceCount = r.ReadInt32();
        int portalCount = r.ReadInt32();
        if (version >= 96)
            room.name = ReadString(r);
        if (version >= 63)
            room.path_pnt = r.ReadVector3();
        room.verts.resize(vertexCount);
        for (int i = 0; i < vertexCount; i++) {
            room.verts[i] = r.ReadVector3();
            if ((version >= 52) && (version <= 67)) {
                r.ReadInt16();
            } else {
                if ((version >= 68) && (version < 71)) {
                    r.ReadVector3();
                    r.ReadInt16();
                }
            }
        }
        room.faces.resize(faceCount);
        for (int i = 0; i < faceCount; i++)
            room.faces[i] = ReadFace(r, version, textureTranslate);
        room.portals.resize(portalCount);
        for (int i = 0; i < portalCount; i++)
            room.portals[i] = ReadPortal(r, version);
        room.flags = r.ReadUInt32();
        if (version < 29)
            r.ReadFloat();
        if (version >= 68) {
            room.pulse_time = r.ReadByte();
            room.pulse_offset = r.ReadByte();
        }
        if (version >= 79)
            room.mirror_face = r.ReadInt16();
        else
            room.mirror_face = -1;
        if ((room.flags & 2u) != 0u) {
            if ((version >= 28) && (version <= 32)) {
                /*int num4 = */r.ReadInt32();
            }
            else {
                if (version >= 33) {
                    if (version < 106) {
                        r.ReadInt32();
                    }
                    /*int num5 = */r.ReadByte();
                    if (version < 106) {
                        //num5 = num5 | 2;
                    }
                    if (version >= 36) {
                        /*int num6 = */r.ReadByte();
                    }
                    /*int num4 = */r.ReadInt32();
                    if (version >= 106) {
                        /*float num7 = */r.ReadFloat();
                    }
                }
            }
            if ((version >= 28) && (version < 106)) {
                r.ReadFloat();
                r.ReadFloat();
                r.ReadFloat();
            }
        }
        if ((version >= 67) && (r.ReadByte() == 1)) {
            int w = r.ReadInt32();
            int h = r.ReadInt32();
            int d = r.ReadInt32();
            int size = w * h * d;
            if (size)
                room.volume_lights = D3Level::ReadCompressedBytes(r, size);
            else
                room.volume_lights.clear();
            room.volume_width = w;
            room.volume_height = h;
            room.volume_depth = d;
        }
        if (version >= 73) {
            room.fog_depth = r.ReadFloat();
            room.fog_r = r.ReadFloat();
            room.fog_g = r.ReadFloat();
            room.fog_b = r.ReadFloat();
        }
        if (version >= 78) {
            string text = ReadString(r);
            room.ambient_sound = -1;
        }
        else {
            room.ambient_sound = -1;
        }
        room.env_reverb = version < 98 ? 0 : r.ReadByte();
        if (version >= 108) {
            room.damage = r.ReadFloat();
            room.damage_type = r.ReadByte();
        }
        return room;
    }

    Matrix3x3 ReadRotationFloat(StreamReader &r) {
        auto rvec = r.ReadVector3();
        auto uvec = r.ReadVector3();
        auto fvec = r.ReadVector3();
        return Matrix3x3(rvec, uvec, -fvec); // flip Z due to LH data
    }
    
    D3::Object ReadObject(StreamReader &r, int version, const List<int>& genericTranslate) {
        constexpr int OBJ_NAME_LEN = 19;
        constexpr int MAX_D3XID_NAME = 32;
        D3::Object obj;
        auto type = (ObjectType)r.ReadByte();
        int id = version >= 34 ? r.ReadInt16() : r.ReadByte();
        if (type == ObjectType::Robot || type == ObjectType::Powerup ||
            type == ObjectType::Clutter || type == ObjectType::Building)
            id = genericTranslate[id];
        if (id == -1)
            id = 0;
        obj.Type = type;
        obj.ID = id;
        obj.Name = r.ReadCString(OBJ_NAME_LEN);
        obj.Flags = version >= 101 ? r.ReadUInt32() : r.ReadUInt16();
        if (type == ObjectType::Door && version >= 109)
            obj.Shields = r.ReadInt16();
        obj.RoomNum = r.ReadUInt32();
        constexpr uint OLD_OBJECT_OVER_TERRAIN_FLAG = 256;
        constexpr uint ROOMNUM_CELLNUM_FLAG = 0x80000000;
        if (version < 49 && (obj.Flags & OLD_OBJECT_OVER_TERRAIN_FLAG)) {
            obj.Flags &= ~OLD_OBJECT_OVER_TERRAIN_FLAG;
            obj.RoomNum |= ROOMNUM_CELLNUM_FLAG;
        }
        obj.Position = r.ReadVector3();
        obj.Rotation = ReadRotationFloat(r);
        obj.Contains.Type = ObjectType(r.ReadByte());
        obj.Contains.ID = r.ReadByte();
        obj.Contains.Count = r.ReadByte();
        obj.Lifespan = r.ReadFloat();
        if (version >= 65 && version < 111)
            r.ReadInt32(); // was ParentHandle
        
        if (type == ObjectType(24)) { // soundsource
            if (version < 119)
                r.ReadInt32();
            else
                r.ReadCString(PAGENAME_LEN);
            r.ReadFloat();
        }

        if (version >= 105) {
            int n;
            if ((n = r.ReadByte()) != 0)
                obj.CustomDefaultScriptName = r.ReadString(n);
            if ((n = r.ReadByte()) != 0)
                obj.CustomDefaultModuleName = r.ReadString(n);
        }
        
        if (version < 97) { // osiris 1
            if (version < 31)
                r.ReadInt32();
            else
                r.ReadCString(MAX_D3XID_NAME);
            if (version >= 46) { // params
                int n = r.ReadInt16();
                for (int i = 0; i < n; i++) {
                    int ptype = r.ReadByte();
                    if (ptype == 0 || ptype == 2)
                        r.ReadFloat();
                    else if (ptype == 1)
                        r.ReadVector3();
                }
            }
            if (version >= 70)
                r.ReadByte(); // scriptcheck
        }
        
        if (version >= 35 && r.ReadByte()) { // has lightmap
            int modelCount = r.ReadByte();
            for (int i = 0; i < modelCount; i++) {
                int faceCount = r.ReadInt16();
                for (int j = 0; j < faceCount; j++) {
                    /*int lmIndex = */r.ReadInt16();
                    if (version <= 88)
                        r.ReadInt32();
                    if (version >= 58) {
                        if (version <= 59)
                            r.ReadVector3(); // tvec
                        r.ReadVector3(); // rvec
                        r.ReadVector3(); // uvec
                    }
                    int vertexCount = r.ReadByte();
                    for (int k = 0; k < vertexCount; k++) {
                        r.ReadFloat(); // u
                        r.ReadFloat(); // v
                    }
                }
            }
        }
        return obj;
    }

    constexpr char OrdLower(char c) {
        return c >= 'A' && c <= 'Z' ? c + 'a' - 'A' : c;
    }

    struct OrdLowerHash {
        size_t operator()(const string& key) const {
            size_t h = 0;
            for (char c : key)
                h = h * 101 + OrdLower(c);
            return h;
        }
    };

    struct OrdLowerEqual {
        bool operator()(const string& a, const string& b) const {
            return a.size() == b.size() 
                && std::equal(a.begin(), a.end(), a.begin(),
                    [](char a, char b) { return OrdLower(a) == OrdLower(b); });
        }
    };
    
    template <class T, typename F> //, typename F>
    List<int> ReadTranslate(StreamReader& r, const List<T>& items, F getName) {
        int n = r.ReadInt32();
        std::unordered_map<string, int, OrdLowerHash, OrdLowerEqual> itemIndex;
        for (int i = 0; i < (int)items.size(); i++)
            itemIndex[getName(items[i])] = i;

        List<int> translate(n);
        for (int i = 0; i < n; i++) {
            auto it = itemIndex.find(r.ReadCString(PAGENAME_LEN));
            translate[i] = it != itemIndex.end() ? it->second : -1;
        }
        return translate;
    }

    optional<D3Level> D3Level::Read(StreamReader& r, const GameTable& table)
    {
        if (r.ReadUInt32() != D3Level::LEVEL_FILE_TAG) {
            return nullopt;
        }
        int version = r.ReadInt32();
        if (version > 132) {
            return nullopt;
        }
        if (version < 13) {
            return nullopt;
        }
        List<Room> rooms;
        List<D3::Object> objects;
        List<int> textureTranslate;
        List<int> genericTranslate;
        string name;
        while (!r.EndOfStream()) {
            uint chunkType = r.ReadUInt32();
            size_t startPos = r.Position();
            int chunkSize = r.ReadInt32();
            if (chunkType == D3Level::CHUNK_TEXTURE_NAMES) {
                textureTranslate = ReadTranslate(r, table.Textures, [](const TextureInfo& tex) { return tex.Name; });
            } else if (chunkType == D3Level::CHUNK_GENERIC_NAMES) {
                genericTranslate = ReadTranslate(r, table.Generics, [](const GenericInfo& gen) { return gen.Name; });
            } else if (chunkType == D3Level::CHUNK_ROOMS) {
                int roomCount = r.ReadInt32();
                if (version >= 85) {
                    /*int num6 = */r.ReadInt32();
                    /*int num7 = */r.ReadInt32();
                    /*int num8 = */r.ReadInt32();
                    /*int num9 = */r.ReadInt32();
                }
                rooms.resize(roomCount);
                for (int i = 0; i < roomCount; i++) {
                    int roomNum = version < 96 ? i : r.ReadInt16();
                    if (roomNum >= rooms.size())
                        rooms.resize(roomNum + 1);
                    rooms[roomNum] = D3Level::ReadRoom(r, version, textureTranslate);
                }
            } else if (chunkType == D3Level::CHUNK_OBJECTS) {
                int objectCount = r.ReadInt32();
                objects.resize(objectCount);
                for (int i = 0;i < objectCount; i++) {
                    int handle, idx;
                    if (version >= 45) {
                        handle = r.ReadInt32();
                        if (version < 94)
                            handle = ((handle & ~1023) << 1) | (handle & 1023);
                        idx = handle & 2047;
                        if (idx >= objects.size())
                            objects.resize(idx + 1);
                    } else {
                        handle = i + 2048;
                        idx = i;
                    }
                    objects[idx] = ReadObject(r, version, genericTranslate);
                    objects[idx].Handle = handle;
                }
            } else if (chunkType == D3Level::CHUNK_LEVEL_INFO) {
                name = ReadString(r);
            }
            r.Seek(startPos + chunkSize);
        }
        D3Level level{};
        level.name = name;
        level.rooms = rooms;
        level.objects = objects;
        //level.texture_xlate = textures;
        return {level};
    }
    D3Level::D3Level()
    {
        LEVEL_FILE_OLDEST_COMPATIBLE_VERSION = 13;
        LEVEL_FILE_VERSION = 132;
        ROOM_NAME_LEN = 19;
        FF_LIGHTMAP = 1;
        FF_VERTEX_ALPHA = 2;
        FF_CORONA = 4;
        FF_TEXTURE_CHANGED = 8;
        FF_HAS_TRIGGER = 16;
        FF_SPEC_INVISIBLE = 32;
        FF_FLOATING_TRIG = 64;
        FF_DESTROYED = 128;
        FF_VOLUMETRIC = 256;
        FF_TRIANGULATED = 512;
        FF_VISIBLE = 1024;
        FF_NOT_SHELL = 2048;
        FF_TOUCHED = 4096;
        FF_GOALFACE = 8192;
        FF_NOT_FACING = 16384;
        FF_SCORCHED = 32768;
        OLD_FF_PORTAL_TRIG = 32;
        OLD_PF_HAS_TRIGGER = 4u;
        RF_FUELCEN = 1;
        RF_DOOR = 2;
        RF_EXTERNAL = 4;
        RF_GOAL1 = 8;
        RF_GOAL2 = 16;
        RF_TOUCHES_TERRAIN = 32;
        RF_SORTED_INC_Y = 64;
        RF_GOAL3 = 128;
        RF_GOAL4 = 256;
        RF_FOG = 512;
        RF_SPECIAL1 = 1024;
        RF_SPECIAL2 = 2048;
        RF_SPECIAL3 = 4096;
        RF_SPECIAL4 = 8192;
        RF_SPECIAL5 = 16384;
        RF_SPECIAL6 = 32768;
        RF_MIRROR_VISIBLE = 65536;
        RF_TRIANGULATE = 131072;
        RF_STROBE = 262144;
        RF_FLICKER = 524288;
        RFM_MINE = 32505856;
        RF_INFORM_RELINK_TO_LG = 33554432;
        RF_MANUAL_PATH_PNT = 67108864;
        RF_WAYPOINT = 134217728;
        RF_SECRET = 268435456;
        RF_NO_LIGHT = 536870912;
        DF_BLASTED = 1;
        DF_AUTO = 2;
        DF_LOCKED = 4;
        DF_KEY_ONLY_ONE = 8;
        DF_GB_IGNORE_LOCKED = 16;
    }
}

#if 0
#include <iostream>
using namespace d3read;
using std::cout, std::endl;

int main() {
    StreamReader r("../../tmp/level1.d3l");
    D3Level *lvl = D3Level::Read(r);
    cout << lvl->rooms.size() << endl;
    delete lvl;
}
#endif