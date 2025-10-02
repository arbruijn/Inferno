#include "pch.h"

#include "OutrageRoom.h"
#include "Face.h"
#include "Level.h"
#include "OutrageTable.h"
#include "Streams.h"
#include "OutrageLevel.h"


namespace Inferno {
    
    namespace {
        // Estimation that treats the sides as planes instead of triangles
        bool PointInSegment(Level& level, SegID id, const Vector3& point) {
            if (!level.SegmentExists(id)) return false;

            for (auto& side : SideIDs) {
                auto face = Face::FromSide(level, id, side);
                if (face.Distance(point) < 0)
                    return false;
            }

            return true;
        }

        SegID FindContainingSegment(Level& level, const Vector3& point) {
            for (int id = 0; id < level.Segments.size(); id++) {
                auto& seg = level.GetSegment((SegID)id);
                if (fabsf(seg.Center.x - point.x) > 200 || fabsf(seg.Center.y - point.y) > 200 || fabsf(seg.Center.z - point.z) > 200)
                    continue;
                if (Vector3::Distance(seg.Center, point) > 200) continue;

                if (PointInSegment(level, (SegID)id, point))
                    return (SegID)id;
            }

            return SegID::None;
        }

        template <class T, typename Pred>
        int FindIndex(const List<T>& list, Pred f) {
            for (int i = 0; i < list.size(); i++)
                if (f(list[i]))
                    return i;
            return -1;
        }
    }

    namespace {
        constexpr int ROOM_NEW_HEADER_CHUNK = 5;
        constexpr int ROOMFILE_VERSION = 4;

        constexpr int ROOM_VERTEX_CHUNK = 1;
        constexpr int ROOM_FACES_CHUNK = 2;
        constexpr int ROOM_END_CHUNK = 3;
        constexpr int ROOM_TEXTURE_CHUNK = 4;
    }

    // D3 ORF face
    struct RoomFace {
        Vector3 Normal;
        List<short> Vertices; // Indices into the vertex array
        List<Vector2> UVs;
        int16 Texture = 0; // Index into room texture chunk
    };

    void LoadRoom(StreamReader& reader) {
        bool done = false;
        List<Vector3> vertices;
        List<RoomFace> faces;
        List<string> textures;
        int version = -1;

        while (!done) {
            auto command = reader.ReadInt32();
            auto len = reader.ReadInt32();

            switch (command) {
                case ROOM_NEW_HEADER_CHUNK:
                {
                    version = reader.ReadInt32();
                    auto numVerts = reader.ReadInt32();
                    auto numFaces = reader.ReadInt32();
                    vertices.resize(numVerts);
                    faces.resize(numFaces);
                    break;
                }

                case ROOM_VERTEX_CHUNK:
                {
                    for (auto& vert : vertices) {
                        vert.x = reader.ReadFloat();
                        vert.y = reader.ReadFloat();
                        vert.z = reader.ReadFloat();
                    }
                    break;
                }

                case ROOM_TEXTURE_CHUNK:
                {
                    auto count = reader.ReadInt32();
                    for (int i = 0; i < count; i++)
                        textures.push_back(reader.ReadCString(64));
                    break;
                }

                case ROOM_FACES_CHUNK:
                {
                    reader.ReadByte(); // light mult
                    auto nverts = reader.ReadInt32();

                    RoomFace face;
                    face.Normal.x = reader.ReadFloat();
                    face.Normal.y = reader.ReadFloat();
                    face.Normal.z = reader.ReadFloat();
                    face.Vertices.resize(nverts);
                    face.UVs.resize(nverts);

                    reader.ReadInt16(); // tex index

                    for (int i = 0; i < nverts; i++) {
                        face.Vertices[i] = reader.ReadInt16();
                        face.UVs[i].x = reader.ReadFloat();
                        face.UVs[i].y = reader.ReadFloat();
                        reader.ReadFloat();
                        reader.ReadFloat();
                        reader.ReadFloat();
                        reader.ReadFloat();
                        if (version >= 1) {
                            reader.ReadFloat(); // alpha
                        }
                    }

                    return; // Return due to bug with reading ending chunk
                }

                case ROOM_END_CHUNK:
                    done = true;
                    break;

                default:
                    // skip the ones we don't know
                    for (int i = 0; i < len; i++)
                        reader.ReadByte();
                    break;
            }
        }
    }

    void SaveRoom(StreamWriter& writer, const List<Vector3>& vertices, const List<RoomFace>& faces, const Outrage::GameTable& table, span<LevelTexID> textures) {
        // Write header
        writer.Write(ROOM_NEW_HEADER_CHUNK);
        auto headsize = (int)writer.Position();
        writer.Write(-1); // header length

        writer.Write(ROOMFILE_VERSION);
        writer.Write((int)vertices.size());
        writer.Write((int)faces.size());

        auto pos = (int)writer.Position();
        writer.Seek(headsize);
        writer.Write(pos - headsize - 4);
        writer.Seek(pos);

        {
            // write vertex info
            writer.Write(ROOM_VERTEX_CHUNK);
            auto vertsize = (int)writer.Position();
            writer.Write(-1); // placeholder

            for (auto& vert : vertices) {
                writer.WriteFloat(vert.x);
                writer.WriteFloat(vert.y);
                writer.WriteFloat(vert.z);
            }

            pos = (int)writer.Position();
            writer.Seek(vertsize);
            writer.Write<int>(pos - vertsize - 4);
            writer.Seek(pos);
        }

        {
            // write texture info
            writer.Write(ROOM_TEXTURE_CHUNK);
            auto texsize = (int)writer.Position();
            writer.Write(-1); // placeholder

            //texCount++;
            assert(textures.size() > 0);
            auto texCount = (int)textures.size();

            //auto maxIndex = 0;
            writer.Write(texCount); // number of textures

            for (auto& texture : textures) {
                if (auto entry = Seq::tryItem(table.Textures, (int)texture - 3000)) {
                    writer.WriteCString(entry->Name, 64);
                }
                else {
                    writer.WriteCString("Rainbow Texture", 64);
                }
            }

            pos = (int)writer.Position();
            writer.Seek(texsize);
            writer.Write(pos - texsize - 4);
            writer.Seek(pos);
        }

        writer.Write(ROOM_FACES_CHUNK);
        auto facesize = (int)writer.Position();
        writer.Write(-1); // placeholder

        for (auto& face : faces) {
            writer.Write((sbyte)4); // Light multiplier?
            writer.Write((int32)face.Vertices.size());

            writer.WriteFloat(face.Normal.x);
            writer.WriteFloat(face.Normal.y);
            writer.WriteFloat(face.Normal.z);
            writer.Write(face.Texture); // Texture index

            for (int t = 0; t < face.Vertices.size(); t++) {
                writer.Write(face.Vertices[t]);
                writer.WriteFloat(face.UVs[t].x);
                writer.WriteFloat(face.UVs[t].y);
                writer.WriteFloat(0.0f); // dummy data
                writer.WriteFloat(0.0f); // dummy data
                writer.WriteFloat(0.0f); // dummy data
                writer.WriteFloat(0.0f); // dummy data
                writer.WriteFloat(1.0f); // alpha
            }
        }

        pos = (int)writer.Position();
        writer.Seek(facesize);
        writer.Write(pos - facesize - 4);
        writer.Seek(pos);

        writer.Write(ROOM_END_CHUNK);
        writer.Write(4);
    }

    struct Room {
        List<Vector3> Vertices;
        List<RoomFace> Faces;
        List<LevelTexID> Textures;
    };

    Room ConvertSegmentsToRoom(Level& level, span<SegID> segs, const Outrage::GameTable& table) {
        //short vertexIndex = 0;
        List<Vector3> vertices;
        List<RoomFace> faces;
        List<LevelTexID> textures;

        for (auto& segid : segs) {
            auto& seg = level.GetSegment(segid);
            int16 vertexOffset = (int16)vertices.size();
            for (int i = 0; i < MAX_VERTICES; i++)
                vertices.push_back(level.Vertices[seg.Indices[i]]);
            for (auto& sid : SideIDs) {
                auto& side = seg.GetSide(sid);
                if (seg.SideHasConnection(sid) && Seq::contains(segs, seg.Connections[(int)sid]))
                    continue; // skip side if it is open and is inside the selection

                auto face = Face::FromSide(level, seg, sid);

                auto indices = side.GetRenderIndices();

                if (face.Side.TMap != LevelTexID::Unset && !Seq::contains(textures, face.Side.TMap))
                    textures.push_back(face.Side.TMap);

                auto texture = int16(Seq::indexOf(textures, face.Side.TMap).value_or(0));

                if (auto entry = Seq::tryItem(table.Textures, (int)face.Side.TMap - 3000)) {
                    if (entry->Name == "Rainbow Texture") 
                        continue; // Don't export faces with the rainbow texture applied
                }

                auto& sideVerts = SIDE_INDICES[(int)sid];

                if (side.Normals[0].Dot(side.Normals[1]) > 0.99999f) {
                    // planar face
                    RoomFace roomFace{};
                    for (int j = 0; j < 4; j++) {
                        roomFace.Vertices.push_back(vertexOffset + sideVerts[j]);
                        roomFace.UVs.push_back(side.UVs[j]);
                    }
                    roomFace.Normal = side.AverageNormal;
                    roomFace.Texture = texture;
                    faces.push_back(roomFace);
                }
                else {
                    for (int i = 0; i < 2; i++) {
                        RoomFace roomFace{};
                        for (int j = 0; j < 3; j++) {
                            roomFace.Vertices.push_back(vertexOffset + sideVerts[indices[j + i * 3]]);
                            roomFace.UVs.push_back(side.UVs[indices[j + i * 3]]);
                        }

                        roomFace.Normal = side.Normals[i];
                        roomFace.Texture = texture;
                        faces.push_back(roomFace);
                    }
                }
            }
        }

        auto deleteVertex = [&](short index) {
            //Remap vertices in faces
            for (short f = 0; f < faces.size(); f++) {
                auto& face = faces[f];

                for (short v = 0; v < face.Vertices.size(); v++) {
                    if (face.Vertices[v] == index)
                        throw Exception("Deleting a vertex still in use!");
                    else if (face.Vertices[v] > index)
                        face.Vertices[v]--;
                }
            }

            Seq::removeAt(vertices, index);
        };

        uint removed = 0;
        // Remove duplicate vertices
        for (short i = 0; i < vertices.size(); i++) {
            for (short j = 0; j < i; j++) {
                if (Vector3::Distance(vertices[i], vertices[j]) < 0.1f) {
                    //Replace the higher-numbered point with the lower-numbered in all the faces in this room
                    auto fp = faces.begin();
                    for (int f = 0; f < faces.size(); f++, fp++) {
                        auto& face = faces[f];

                        for (int v = 0; v < face.Vertices.size(); v++)
                            if (face.Vertices[v] == i)
                                face.Vertices[v] = j;
                    }

                    deleteVertex(i);
                    i--; //back up, since the point we're checking is now gone
                    removed++;
                    break; //don't keep checking for duplicates
                }
            }
        }
        return Room{.Vertices = vertices, .Faces = faces, .Textures = textures};
    }

    void WriteSegmentsToOrf(Level& level, span<SegID> segs, const filesystem::path& path, const Outrage::GameTable& table) {
        auto room = ConvertSegmentsToRoom(level, segs, table);
        {
            std::ofstream file(path, std::ios::binary);
            StreamWriter writer(file, false);

            if (room.Textures.empty()) room.Textures.push_back(LevelTexID(3000));
            SaveRoom(writer, room.Vertices, room.Faces, table, room.Textures);
        }

        //{
        //    StreamReader reader(path);
        //    LoadRoom(reader);
        //}
    }

    void ExportOutrageLevel(Level& level, StreamWriter& writer, const Outrage::GameTable& table) {
        List<SegID> segs;
        List<int> energySegs;
        for (int i = 0; i < level.Segments.size(); i++)
            if (level.Segments[i].Type != SegmentType::Energy)
                segs.push_back((SegID)i);
            else
                energySegs.push_back(i);

        List<Room> rooms;
        rooms.push_back(ConvertSegmentsToRoom(level, segs, table));

        List<bool> energySeen(level.Segments.size());
        for (int startSegNum : energySegs) {
            if (energySeen[startSegNum])
                continue;
            segs.clear();
            List<int>stack{};
            stack.push_back(startSegNum);
            energySeen[startSegNum] = true;
            while (!stack.empty()) {
                int segNum = stack.back();
                stack.pop_back();
                segs.push_back((SegID)segNum);
                auto& seg = level.Segments[segNum];
                for (int i = 0; i < MAX_SIDES; i++) {
                    int c = (int)seg.Connections[i];
                    if (c >= 0 && level.Segments[c].Type == SegmentType::Energy && !energySeen[c]) {
                        stack.push_back(c);
                        energySeen[c] = true;
                    }
                }
            }
            rooms.push_back(ConvertSegmentsToRoom(level, segs, table));
        }

        Vector3 center(2052.16089f, -101.79055f, 2049.29736f);
        Vector3 center2(2048.0f, -100.0f, 2048.0f);

        Outrage::OutrageLevel outrageLevel{};

        for (auto& room : rooms) {
            if (room.Textures.empty()) room.Textures.push_back(LevelTexID(3000));
            Outrage::Room levelRoom{.MirrorFace = -1, .FogDepth = 100.0f, .FogColor = Vector3(1.0f, 1.0f, 1.0f)};
            //Vector3 center(2050.0f, -110.0f, 2040.0f);
            levelRoom.PathPoint = Vector3(2052.161f, -101.79055f, 2049.29736f);
            for (auto& vertex : room.Vertices)
                levelRoom.Vertices.push_back(vertex + center);
            for (auto& face : room.Faces) {
                Outrage::Face levelFace{.Portal = -1, .LightMultiple = 4};
                levelFace.Texture = std::max(0, (int)room.Textures[face.Texture] - 3000);
                levelFace.VerticesIndex = face.Vertices;
                for (auto& uv : face.UVs)
                    levelFace.VerticesData.push_back({.U = uv.x, .V = uv.y, .Alpha = 255});
                levelRoom.Faces.push_back(std::move(levelFace));
            }
            outrageLevel.Rooms.push_back(std::move(levelRoom));
        }

        int h = -1;
        for (auto& obj : level.Objects) {
            h++;
            Outrage::Object oobj{};
            oobj.Type = obj.Type;
            oobj.ID = obj.ID;
            oobj.Handle = h | 2048;
            oobj.Position = obj.Position + center2;
            oobj.RoomNum = 0;
            oobj.Rotation = obj.Rotation;
            oobj.Flags = Outrage::ObjectFlags::PolygonObject;
            oobj.Contains.Type = ObjectType::None;
            outrageLevel.Objects.push_back(std::move(oobj));
        }
        outrageLevel.EditorSegments.Vertices = level.Vertices;
        outrageLevel.EditorSegments.Segments.resize(level.Segments.size());
        for (size_t i = 0; i < level.Segments.size(); i++) {
            auto& oseg = outrageLevel.EditorSegments.Segments[i];
            auto& iseg = level.Segments[i];
            for (int j = 0; j < MAX_VERTICES; j++)
                oseg.Indices[j] = iseg.Indices[j];
            for (int j = 0; j < MAX_SIDES; j++) {
                auto& oside = oseg.Sides[j];
                auto& iside = iseg.Sides[j];
                oside.ChildSegment = (int16)iseg.Connections[j];
                oside.TMap = (int16)iside.TMap;
                oside.UVs = iside.UVs;
            }
        }
        outrageLevel.Write(writer, table);
    }

    void WriteLevelToD3L(Level& level, const filesystem::path& path, const Outrage::GameTable& table) {
        std::ofstream file(path, std::ios::binary);
        StreamWriter writer(file, false);
        ExportOutrageLevel(level, writer, table);
    }

    Level ImportOutrageLevel(const filesystem::path& path, const Outrage::GameTable& table) {
        std::ifstream file(path, std::ios::binary);
        if (!file) throw Exception("File does not exist");

        auto size = filesystem::file_size(path);
        List<ubyte> buffer(size);
        if (!file.read((char*)buffer.data(), size))
            throw Exception("Error reading file");

        StreamReader r(buffer);
        auto optOutrageLevel = Outrage::OutrageLevel::Read(r, table);

        if (!optOutrageLevel)
            throw Exception("Cannot read level.");

        auto outrageLevel = *optOutrageLevel;

        if (outrageLevel.Rooms.size() != 1)
            throw Exception("Level has multiple rooms.");

        auto room = outrageLevel.Rooms[0];

        Level level{};

        if ((room.Vertices.size() % 8) != 0)
            throw Exception("Room vertices not a multiple of 8.");

        auto& oSegs = outrageLevel.EditorSegments;
        
        level.Vertices = oSegs.Vertices;
        
        level.Segments.resize(oSegs.Segments.size());

        for (size_t i = 0; i < oSegs.Segments.size(); i++) {
            auto& seg = level.Segments[i];
            auto& oSeg = oSegs.Segments[i];
            for (int j = 0; j < MAX_VERTICES; j++)
                seg.Indices[j] = (PointID)oSeg.Indices[j];
            seg.UpdateGeometricProps(level);
            for (int j = 0; j < MAX_SIDES; j++) {
                seg.Connections[j] = (SegID)oSeg.Sides[j].ChildSegment;
                seg.Sides[j].TMap = (LevelTexID)oSeg.Sides[j].TMap;
                for (int k = 0; k < 4; k++)
                    seg.Sides[j].UVs[k] = oSeg.Sides[j].UVs[k];
            }
        }
        
        level.Objects.resize(outrageLevel.Objects.size());

        for (size_t i = 0; i < outrageLevel.Objects.size(); i++) {
            auto& obj = level.Objects[i];
            auto& oObj = outrageLevel.Objects[i];
            obj.Type = oObj.Type;
            obj.ID = FindIndex(table.Generics, [&](auto& gen) { return gen.Name == oObj.Name; });
            obj.Position = oObj.Position;
            obj.Segment = FindContainingSegment(level, obj.Position);
        }

#if 0
        level.Vertices = room.Vertices;

        level.Segments.resize(room.Vertices.size() / 8);

        for (size_t i = 0; i < room.Faces.size(); ) {
            auto& face = room.Faces[i];
            int v = face.VerticesIndex[0];
            //int seg = v / 8;
            v %= 8;
            std::span<const int16> indices;
            if (face.VerticesIndex.size() == 4) {
                indices = face.VerticesIndex;
                /*
                if (face.VerticesIndex[1] == v + 1 && face.VerticesIndex[2] == v + 2 &&
                    face.VerticesIndex[3] == v + 3)
                    ;
                else
                    throw new Exception("Face 4 indices not matching");
                */
             } else if (face.VerticesIndex.size() == 3 && i + 1 < room.Faces.size() && room.Faces[i + 1].VerticesIndex.size() == 3) {
                auto& face2 = room.Faces[i + 1];
                if (face.VerticesIndex[0] == face2.VerticesIndex[0] && face.VerticesIndex[2] == face2.VerticesIndex[1])
                    indices = std::array<int16,4>{face.VerticesIndex[0], face.VerticesIndex[1], face.VerticesIndex[2], face2.VerticesIndex[2]};
                else if (face.VerticesIndex[2] == face2.VerticesIndex[0] == v + 3 &&
                        face.VerticesIndex[1] == face2.VerticesIndex[1])
                    indices = std::array<int16,4>{face.VerticesIndex[0], face.VerticesIndex[1], face2.VerticesIndex[2], face.VerticesIndex[2]};
                else
                    throw Exception("Face 3 indices not matching");
            } else
                throw Exception("Face size not 3 or 4");

        }
#endif

        level.FileName = path.filename().string();
        level.Path = path;
        return level;
    }
}
