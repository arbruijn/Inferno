#include "pch.h"

#include <functional>
#include "Streams.h"
#include "OutrageLevel.h"
#include "OutrageTable.h"

#include <string>
using std::optional, std::nullopt;
constexpr auto PAGENAME_LEN = 35;

using namespace Inferno::Outrage;

namespace Inferno::Outrage {
    constexpr ubyte OutrageLevel::Float_to_ubyte(float f)
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

    Face OutrageLevel::ReadFace(StreamReader& r, int version, const List<int>& textureTranslate)
    {
        Face face{};
        int vertexCount = r.ReadByte();
        face.VerticesIndex.resize(vertexCount);
        for (int i = 0; i < vertexCount; i++)
            face.VerticesIndex[i] = r.ReadInt16();
        bool alphaed = false;
        face.VerticesData.resize(vertexCount);
        for (int j = 0; j < vertexCount; j++) {
            face.VerticesData[j].U = r.ReadFloat();
            face.VerticesData[j].V = r.ReadFloat();
            if (version < 56) {
                r.ReadFloat();
                r.ReadFloat();
                r.ReadFloat();
                r.ReadFloat();
            }
            if (version >= 21) {
                if (version < 61) {
                    face.VerticesData[j].Alpha = OutrageLevel::Float_to_ubyte(r.ReadFloat());
                }
                else {
                    face.VerticesData[j].Alpha = r.ReadByte();
                }
            }
            else {
                face.VerticesData[j].Alpha = 255;
            }
            if (face.VerticesData[j].Alpha != 255) {
                alphaed = true;
            }
        }
        if (version < 27)
            face.Flags = (FaceFlags)r.ReadByte();
        else
            face.Flags = (FaceFlags)r.ReadInt16();
        if (version < 103)
            face.Flags &= ~FaceFlags::SpecInvisible;
        if (alphaed)
            face.Flags |= FaceFlags::VertexAlpha;
        else
            face.Flags &= ~FaceFlags::VertexAlpha;
        if (version >= 23)
            face.Portal = r.ReadByte();
        else
            face.Portal = r.ReadInt16();
        face.Texture = textureTranslate[r.ReadInt16()];
        if (HasFlag(face.Flags, FaceFlags::Lightmap) && (version >= 19)) {
            if (version <= 29) {
                int w = r.ReadByte();
                int h = r.ReadByte();
                for (int k = 0; k < (w * h); k++) {
                    r.ReadInt16();
                }
                face.Flags &= ~FaceFlags::Lightmap;
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
                face.VerticesData[l].U2 = r.ReadFloat();
                face.VerticesData[l].U2 = r.ReadFloat();
                if (face.VerticesData[l].U2 < 0)
                    face.VerticesData[l].U2 = 0;
                if (face.VerticesData[l].U2 > 1)
                    face.VerticesData[l].U2 = 1;
                if (face.VerticesData[l].V2 < 0)
                    face.VerticesData[l].V2 = 0;
                if (face.VerticesData[l].V2 > 1)
                    face.VerticesData[l].V2 = 1;
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
            face.LightMultiple = r.ReadByte();
            if (face.LightMultiple == 186) {
                face.LightMultiple = 4;
            }
            if (version <= 52) {
                face.LightMultiple *= 4;
            }
        }
        else {
            face.LightMultiple = 4;
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
    Portal OutrageLevel::ReadPortal(StreamReader& r, int version)
    {
        Portal portal{};
        portal.Flags = r.ReadUInt32();
        if (version < 103) {
            portal.Flags = portal.Flags & ~4;
        }
        if (version < 80) {
            int num = r.ReadInt16();
            for (int i = 0; i < num; i++)
                r.ReadInt16();
            /*int num2 = */r.ReadInt16();
        }
        portal.Face = (int)(r.ReadInt16());
        portal.OppositeRoom = r.ReadInt32();
        portal.OppositePortal = r.ReadInt32();
        if (version >= 123) {
            portal.bnode_index = (int)(r.ReadInt16());
        }
        else {
            portal.bnode_index = -1;
        }
        if (version >= 63) {
            portal.PathPoint = r.ReadVector3();
        }
        if (version >= 100) {
            portal.combine_master = r.ReadInt32();
        }
        return portal;
    }

    List<ubyte> OutrageLevel::ReadCompressedBytes(StreamReader& r, int total)
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

    Room OutrageLevel::ReadRoom(StreamReader& r, int version, const List<int>& textureTranslate)
    {
        Room room{};
        int vertexCount = r.ReadInt32();
        int faceCount = r.ReadInt32();
        int portalCount = r.ReadInt32();
        if (version >= 96)
            room.Name = ReadString(r);
        if (version >= 63)
            room.PathPoint = r.ReadVector3();
        room.Vertices.resize(vertexCount);
        for (int i = 0; i < vertexCount; i++) {
            room.Vertices[i] = r.ReadVector3();
            if ((version >= 52) && (version <= 67)) {
                r.ReadInt16();
            } else {
                if ((version >= 68) && (version < 71)) {
                    r.ReadVector3();
                    r.ReadInt16();
                }
            }
        }
        room.Faces.resize(faceCount);
        for (int i = 0; i < faceCount; i++)
            room.Faces[i] = ReadFace(r, version, textureTranslate);
        room.Portals.resize(portalCount);
        for (int i = 0; i < portalCount; i++)
            room.Portals[i] = ReadPortal(r, version);
        room.Flags = (RoomFlags)r.ReadUInt32();
        if (version < 29)
            r.ReadFloat();
        if (version >= 68) {
            room.PulseTime = r.ReadByte();
            room.PulseOffset = r.ReadByte();
        }
        if (version >= 79)
            room.MirrorFace = r.ReadInt16();
        else
            room.MirrorFace = -1;
        if (HasFlag(room.Flags, RoomFlags::Door)) {
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
                room.VolumeLights = OutrageLevel::ReadCompressedBytes(r, size);
            else
                room.VolumeLights.clear();
            room.VolumeWidth = w;
            room.VolumeHeight = h;
            room.VolumeDepth = d;
        }
        if (version >= 73) {
            room.FogDepth = r.ReadFloat();
            room.FogColor = r.ReadVector3();
        }
        if (version >= 78) {
            string text = ReadString(r);
            room.AmbientSound = -1;
        }
        else {
            room.AmbientSound = -1;
        }
        room.EnvironmentReverb = version < 98 ? 0 : r.ReadByte();
        if (version >= 108) {
            room.Damage = r.ReadFloat();
            room.DamageType = r.ReadByte();
        }
        return room;
    }

    Matrix3x3 ReadRotationFloat(StreamReader &r) {
        auto rvec = r.ReadVector3();
        auto uvec = r.ReadVector3();
        auto fvec = r.ReadVector3();
        return Matrix3x3(rvec, uvec, -fvec); // flip Z due to LH data
    }
    
    Outrage::Object ReadObject(StreamReader &r, int version, const List<int>& genericTranslate) {
        constexpr int OBJ_NAME_LEN = 19;
        constexpr int MAX_D3XID_NAME = 32;
        Outrage::Object obj;
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
        obj.Flags = version >= 101 ? (ObjectFlags)r.ReadUInt32() : (ObjectFlags)r.ReadUInt16();
        if (type == ObjectType::Door && version >= 109)
            obj.Shields = r.ReadInt16();
        obj.RoomNum = r.ReadUInt32();
        constexpr uint OLD_OBJECT_OVER_TERRAIN_FLAG = 256;
        constexpr uint ROOMNUM_CELLNUM_FLAG = 0x80000000;
        if (version < 49 && HasFlag(obj.Flags, (ObjectFlags)OLD_OBJECT_OVER_TERRAIN_FLAG)) {
            obj.Flags &= ~(ObjectFlags)OLD_OBJECT_OVER_TERRAIN_FLAG;
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

    ESegInfo ReadEditorSegments(StreamReader& r)
    {
        ESegInfo editorSegments{};
        int version = r.ReadInt16();
        if (version != 0)
            return editorSegments;
        editorSegments.Vertices.resize(r.ReadInt32());
        for (auto& vertex : editorSegments.Vertices)
            vertex = r.ReadVector3();
        editorSegments.Segments.resize(r.ReadInt32());
        for (auto& segment : editorSegments.Segments) {
            for (auto& vertex : segment.Indices)
                vertex = r.ReadInt16();
            for (auto& side : segment.Sides) {
                side.ChildSegment = r.ReadInt16();
                side.TMap = r.ReadInt16();
                for (auto& uv : side.UVs) {
                    uv.x = r.ReadFloat();
                    uv.y = r.ReadFloat();
                }
            }
        }
        return editorSegments;
    }

    static void WriteFace(StreamWriter& f, Face& face) {
        f.Write((ubyte)face.VerticesIndex.size());
        for (auto index : face.VerticesIndex)
            f.Write(index);
        for (auto& data : face.VerticesData) {
            f.WriteFloat(data.U);
            f.WriteFloat(data.V);
            f.Write(data.Alpha);
        }
        f.Write(face.Flags);
        f.Write((int8)face.Portal);
        f.Write((uint16)face.Texture);
        if (HasFlag(face.Flags, FaceFlags::Lightmap)) {
            uint lightmapHandle = 0;
            f.Write((uint16)lightmapHandle);
            for (auto& data : face.VerticesData) {
                f.WriteFloat(data.U2);
                f.WriteFloat(data.V2);
            }
        }
        f.Write(face.LightMultiple);
        f.Write((ubyte)0); // not special
        /*
        f.Write((ubyte)type);
        f.Write((ubyte)instCount);
        f.Write((ubyte)smooth);
        f.Write((ubyte)smoothCount);
        foreach instCount
          f.WriteVector3(center);
          f.Write((uint16)color)
        if (smooth)
         foreach smoothCount
          f.WriteVector3(norm);
        */
    }

    static void WritePortal(StreamWriter& f, Portal& portal) {
        f.Write(portal.Flags);
        f.Write((uint16)portal.Face);
        f.Write(portal.OppositeRoom);
        f.Write(portal.OppositePortal);
        f.Write((int16)portal.bnode_index);
        f.WriteVector3(portal.PathPoint);
        f.Write(portal.combine_master);
    }

    static std::streampos WriteStartChunk(StreamWriter& f, const uint tag) {
        f.Write(tag);
        auto pos = f.Position();
        f.Write(0);
        return pos;
    }

    static void WriteEndChunk(StreamWriter& f, const std::streampos startPos) {
        uint32 size = (uint32)(f.Position() - startPos);
        uint32 pad = size & 3 ? 4 - (size & 3) : 0;
        for (uint32 i = 0; i < pad; i++)
            f.Write((ubyte)0);
        size += pad;
        f.Seek(startPos);
        f.Write(size);
        f.SeekForward(size - 4);
    }

    template <typename T>
    static void WriteNames(StreamWriter& f, const uint tag, const List<T>& items) {
        auto pos = WriteStartChunk(f, tag);
        f.Write((uint32)items.size());
        for (auto& item : items)
            f.WriteCString(item.Name, PAGENAME_LEN);
        WriteEndChunk(f, pos);
    }

    static void WriteRoom(StreamWriter& f, Room& room) {
        f.Write((uint32)room.Vertices.size());
        f.Write((uint32)room.Faces.size());
        f.Write((uint32)room.Portals.size());
        f.WriteCString(room.Name, PAGENAME_LEN);
        f.WriteVector3(room.PathPoint);
        for (auto& vertex : room.Vertices)
            f.WriteVector3(vertex);
        for (auto& face : room.Faces)
            WriteFace(f, face);
        for (auto& portal : room.Portals)
            WritePortal(f, portal);
        f.Write((uint32)room.Flags);
        f.Write((ubyte)room.PulseTime);
        f.Write((ubyte)room.PulseOffset);
        f.Write(room.MirrorFace);
        if (HasFlag(room.Flags, RoomFlags::Door)) {
            ubyte DoorwayFlags = 0, DoorwayKeys = 0;
            uint DoorNum = 0;
            float DoorPosition = 0.0f;
            f.Write(DoorwayFlags);
            f.Write(DoorwayKeys);
            f.Write(DoorNum);
            f.WriteFloat(DoorPosition);
        }
        f.Write((ubyte)0); // No volume lights
        /*if ((version >= 67) && (r.ReadByte() == 1)) {
            int w = r.ReadInt32();
            int h = r.ReadInt32();
            int d = r.ReadInt32();
            int size = w * h * d;
            if (size)
                room.VolumeLights = OutrageLevel::ReadCompressedBytes(r, size);
            else
                room.VolumeLights.clear();
            room.VolumeWidth = w;
            room.VolumeHeight = h;
            room.VolumeDepth = d;
        }*/
        f.WriteFloat(room.FogDepth);
        f.WriteVector3(room.FogColor);
        f.WriteCString("", PAGENAME_LEN); //room.AmbientSound);
        f.Write(room.EnvironmentReverb);
        f.WriteFloat(room.Damage);
        f.Write(room.DamageType);
    }

    void OutrageLevel::WriteRooms(StreamWriter& f)
    {
        auto pos = WriteStartChunk(f, CHUNK_ROOMS);
        uint totalVertices = 0, totalFaces = 0, totalFaceVertices = 0, totalPortals = 0;
        for (auto& room : Rooms) {
            totalVertices += (uint)room.Vertices.size();
            totalFaces += (uint)room.Faces.size();
            for (auto& face : room.Faces)
                totalFaceVertices += (uint)face.VerticesIndex.size();
            totalPortals += (uint)room.Portals.size();
        }
        f.Write((uint32)Rooms.size());
        f.Write(totalVertices);
        f.Write(totalFaces);
        f.Write(totalFaceVertices);
        f.Write(totalPortals);
        uint16 i = 0;
        for (auto& room : Rooms) {
            f.Write(i++);
            WriteRoom(f, room);
        }
        WriteEndChunk(f, pos);
    }

    template <typename T>
    static void WriteCompressed(StreamWriter& f, const span<T> data)
    {
        f.Write((ubyte)1);
        size_t i = 0;
        while (i < data.size()) {
            T val = data[i++];
            uint count = 1;
            while (count < 250 && i < data.size() && data[i] == val) {
                i++;
                count++;
            }
            f.Write(count == 1 ? (ubyte)0 : (ubyte)count);
            f.Write(val);
        }
    }

    void OutrageLevel::WriteTerrainHeight(StreamWriter& f)
    {
        auto pos = WriteStartChunk(f, CHUNK_TERRAIN_HEIGHT);
        List<ubyte> height;
        height.resize(TERRAIN_DEPTH * TERRAIN_WIDTH, 0);
        WriteCompressed(f, span{height});
        WriteEndChunk(f, pos);
    }

    void OutrageLevel::WriteTerrainTMapsFlags(StreamWriter& f)
    {
        auto pos = WriteStartChunk(f, CHUNK_TERRAIN_TMAPS_FLAGS);
        List<uint16> tmaps;
        tmaps.resize(TERRAIN_TEX_DEPTH * TERRAIN_TEX_WIDTH, 4);
        WriteCompressed(f, span{tmaps});
        List<ubyte> rotators;
        rotators.resize(TERRAIN_TEX_DEPTH * TERRAIN_TEX_WIDTH, 16);
        WriteCompressed(f, span{rotators});
        List<ubyte> flags;
        flags.resize(TERRAIN_DEPTH * TERRAIN_WIDTH, 32);
        WriteCompressed(f, span{flags});
        WriteEndChunk(f, pos);
    }

    void OutrageLevel::WriteTerrainSky(StreamWriter& f, const GameTable& table)
    {
        TerrainSky sky{.FogScalar = 0.85f, .Textured = 1, .DomeTexture = (int16)table.TextureNames.find("CloudySky")->second,
            .SkyColor = 0x080020, .HorizonColor = 0x802020, .FogColor = 0x040010, .Flags = 3, .Radius = 2500.0f};
        List<Satellite> satellites;
        auto pos = WriteStartChunk(f, CHUNK_TERRAIN_SKY);

        f.WriteFloat(sky.FogScalar);
        f.WriteFloat(sky.DamagePerSecond);
        f.Write(sky.Textured);
        f.Write(sky.DomeTexture);
        f.Write(sky.SkyColor);
        f.Write(sky.HorizonColor);
        f.Write(sky.FogColor);
        f.Write(sky.Flags);
        f.WriteFloat(sky.Radius);
        f.WriteFloat(sky.RotateRate);

        satellites.push_back(Satellite{.Texture = 111, .Position = {5254.595f, 5265.7324f, 949.16705f}, .Size = 500.0f});
        f.Write((uint32)satellites.size());
        
        for (auto& satellite : satellites) {
            f.Write(satellite.Texture);
            f.WriteVector3(satellite.Position);
            f.Write(satellite.Flags);
            f.WriteFloat(satellite.Size);
            f.WriteVector3(satellite.Color);
        }

        List<ubyte> light;
        light.resize(TERRAIN_DEPTH * TERRAIN_WIDTH, 128);
        WriteCompressed(f, span{light});
        WriteCompressed(f, span{light}); // r
        WriteCompressed(f, span{light}); // g
        WriteCompressed(f, span{light}); // b
        std::fill(light.begin(), light.end(), (ubyte)255);
        WriteCompressed(f, span{light}); // dynamic
   
        f.Write((uint32)0xfffffffe); // occlusion checksum
        List<ubyte> occlusion;
        occlusion.resize(OCCLUSION_SIZE * OCCLUSION_SIZE * 32, 0);
        WriteCompressed(f, span{occlusion});

        WriteEndChunk(f, pos);
    }


    void OutrageLevel::WriteTerrain(StreamWriter& f, const GameTable& table)
    {
        auto pos = WriteStartChunk(f, CHUNK_TERRAIN);
        WriteTerrainHeight(f);
        WriteTerrainTMapsFlags(f);
        WriteTerrainSky(f, table);
        f.Write(CHUNK_TERRAIN_END);
        f.Write((uint32)4);
        WriteEndChunk(f, pos);
    }

    static void WriteRotationFloat(StreamWriter& f, const Matrix3x3& m) {
        f.WriteVector3(Vector3(m.m[0]));
        f.WriteVector3(Vector3(m.m[1]));
        f.WriteVector3(-Vector3(m.m[2])); // flip Z due to LH data
    }

    void OutrageLevel::WriteObject(StreamWriter& f, const Outrage::Object& obj) {
        constexpr int OBJ_NAME_LEN = 19;
        f.Write((uint8)obj.Type);
        f.Write((int16)obj.ID);
        f.WriteCString(obj.Name, OBJ_NAME_LEN);
        f.Write((uint32)obj.Flags);
        if (obj.Type == ObjectType::Door)
            f.Write((int16)obj.Shields);
        f.Write((uint32)obj.RoomNum);
        f.WriteVector3(obj.Position);
        WriteRotationFloat(f, obj.Rotation);
        f.Write((ubyte)obj.Contains.Type);
        f.Write((ubyte)obj.Contains.ID);
        f.Write((ubyte)obj.Contains.Count);
        f.WriteFloat(obj.Lifespan);

        if (obj.Type == ObjectType(24)) { // soundsource
            f.WriteCString("", PAGENAME_LEN);
            f.WriteFloat(0.0f);
        }

        f.Write((ubyte)0); // CustomDefaultScriptName
        f.Write((ubyte)0); // CustomDefaultModuleName

        f.Write((ubyte)0); // no lightmap
        #if 0
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
        #endif
    }

    void OutrageLevel::WriteObjects(StreamWriter& f)
    {
        auto pos = WriteStartChunk(f, CHUNK_OBJECTS);
        f.Write((uint32)Objects.size());
        for (auto& obj : Objects) {
            f.Write((uint32)obj.Handle);
            WriteObject(f, obj);
        }
        WriteEndChunk(f, pos);
    }

    void OutrageLevel::WriteEditorSegments(StreamWriter& f)
    {
        auto pos = WriteStartChunk(f, CHUNK_ESEG);
        f.Write((int16)0); // version
        f.Write((int32)EditorSegments.Vertices.size());
        for (auto& vertex : EditorSegments.Vertices)
            f.WriteVector3(vertex);
        f.Write((int32)EditorSegments.Segments.size());
        for (auto& segment : EditorSegments.Segments) {
            for (auto& vertex : segment.Indices)
                f.Write((int16)vertex);
            for (auto& side : segment.Sides) {
                f.Write((int16)side.ChildSegment);
                f.Write((int16)side.TMap);
                for (auto& uv : side.UVs) {
                    f.WriteFloat(uv.x);
                    f.WriteFloat(uv.y);
                }
            }
        }
        WriteEndChunk(f, pos);
    }

    void OutrageLevel::Write(StreamWriter& f, const GameTable& table)
    {
        f.Write(LEVEL_FILE_TAG);
        f.Write(LEVEL_FILE_VERSION);

        f.Write(CHUNK_GAME_PATHS);
        f.Write(8);
        f.Write(0);

        f.Write(CHUNK_TERRAIN_SOUND);
        f.Write(8);
        f.Write(0);

        f.Write(CHUNK_NEW_LIGHTMAPS);
        f.Write(12);
        f.Write(0);
        f.Write(0);

        f.Write(CHUNK_PLAYER_STARTS);
        f.Write(8 + 32 * 4);
        f.Write(32);
        for (int i = 0; i < 32; i++)
            f.Write(0);

        WriteNames(f, CHUNK_TEXTURE_NAMES, table.Textures);
        WriteNames(f, CHUNK_GENERIC_NAMES, table.Generics);
        WriteNames(f, CHUNK_DOOR_NAMES, table.Doors);

        WriteRooms(f);

        WriteTerrain(f, table);

        f.Write(CHUNK_OBJECT_HANDLES);
        f.Write(8);
        f.Write(0);

        WriteObjects(f);

        f.Write(CHUNK_TRIGGERS);
        f.Write(8);
        f.Write(0);

        f.Write(CHUNK_BOA);
        f.Write(8);
        f.Write(0);

        f.Write(CHUNK_ROOM_AABB);
        f.Write(8);
        f.Write(0);

        f.Write(CHUNK_MATCEN_DATA);
        f.Write(8);
        f.Write(0);

        f.Write(CHUNK_LEVEL_GOALS);
        f.Write(12);
        f.Write(4);
        f.Write(0);

        f.Write(CHUNK_LEVEL_INFO);
        f.Write(8);
        f.Write(0);

        f.Write(CHUNK_EDITOR_INFO);
        f.Write(8);
        f.Write(0);
    }

    optional<OutrageLevel> OutrageLevel::Read(StreamReader& r, const GameTable& table)
    {
        if (r.ReadUInt32() != LEVEL_FILE_TAG) {
            return nullopt;
        }
        int version = r.ReadInt32();
        if (version > 132) {
            return nullopt;
        }
        if (version < 13) {
            return nullopt;
        }
        OutrageLevel level{};
        List<int> textureTranslate;
        List<int> genericTranslate;
        while (!r.EndOfStream()) {
            uint chunkType = r.ReadUInt32();
            size_t startPos = r.Position();
            int chunkSize = r.ReadInt32();
            if (chunkType == CHUNK_TEXTURE_NAMES) {
                textureTranslate = ReadTranslate(r, table.Textures, [](const TextureInfo& tex) { return tex.Name; });
            } else if (chunkType == CHUNK_GENERIC_NAMES) {
                genericTranslate = ReadTranslate(r, table.Generics, [](const GenericInfo& gen) { return gen.Name; });
            } else if (chunkType == CHUNK_ROOMS) {
                int roomCount = r.ReadInt32();
                if (version >= 85) {
                    /*int num6 = */r.ReadInt32();
                    /*int num7 = */r.ReadInt32();
                    /*int num8 = */r.ReadInt32();
                    /*int num9 = */r.ReadInt32();
                }
                level.Rooms.resize(roomCount);
                for (int i = 0; i < roomCount; i++) {
                    int roomNum = version < 96 ? i : r.ReadInt16();
                    if (roomNum >= level.Rooms.size())
                        level.Rooms.resize(roomNum + 1);
                    level.Rooms[roomNum] = ReadRoom(r, version, textureTranslate);
                }
            } else if (chunkType == CHUNK_OBJECTS) {
                int objectCount = r.ReadInt32();
                level.Objects.resize(objectCount);
                for (int i = 0;i < objectCount; i++) {
                    int handle, idx;
                    if (version >= 45) {
                        handle = r.ReadInt32();
                        if (version < 94)
                            handle = ((handle & ~1023) << 1) | (handle & 1023);
                        idx = handle & 2047;
                        if (idx >= level.Objects.size())
                            level.Objects.resize(idx + 1);
                    } else {
                        handle = i + 2048;
                        idx = i;
                    }
                    level.Objects[idx] = ReadObject(r, version, genericTranslate);
                    level.Objects[idx].Handle = handle;
                }
            } else if (chunkType == CHUNK_LEVEL_INFO) {
                level.Name = ReadString(r);
            } else if (chunkType == CHUNK_ESEG) {
                level.EditorSegments = ReadEditorSegments(r);
            }
            r.Seek(startPos + chunkSize);
        }
        return {level};
    }
    OutrageLevel::OutrageLevel()
    {
    /*
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
    */
    }
}

#if 0
#include <iostream>
using namespace d3read;
using std::cout, std::endl;

int main() {
    StreamReader r("../../tmp/level1.d3l");
    OutrageLevel *lvl = OutrageLevel::Read(r);
    cout << lvl->rooms.size() << endl;
    delete lvl;
}
#endif