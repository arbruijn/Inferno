#include "pch.h"
#include "Level.h"
#include "Streams.h"
#include "Utility.h"
#include "D3LevelReader.h"
#include "D3Level.h"
#include "OutrageTable.h"
#include "../Inferno/logging.h"
#include "FmtTypes.h"

using namespace Inferno::D3;


namespace Inferno {
    Level D3LevelReader::Read(const Outrage::GameTable& table) {
        List<int> textureTranslate;
        List<int> genericTranslate;

        auto d3l_opt = D3Level::Read(_reader, table);
        if (!d3l_opt)
            return Level{};
        D3Level d3l = std::move(d3l_opt.value());

        Level level;
        level.Version = -2;
        level.Name = d3l.name;
        LevelLimits d3limits(0);
        //.Objects = 1500, .Segments = 400, .Matcens = 60, .Vertices = 10000,
        //    .Walls = 100, .WallSwitches = 50, .WallLinks = 100, .FuelCenters = 400, .Reactor = 0,
        //    .Keys = 4, .Players = 32, .Triggers = 100, .FlickeringLights = 0 };
        level.Limits = d3limits;

        //int vertexCount = 0;
        //for (auto& room : d3l.rooms)
        //    vertexCount += (int)room.verts.size();
        level.Segments.resize(d3l.rooms.size());
        //level.Vertices.resize(vertexCount);
        
        //level.TextureNames.resize(d3l.texture_xlate.size());
        //for (int i = 0; i < d3l.texture_xlate.size(); i++)
        //	level.TextureNames[i] = d3l.texture_xlate[i].name;
        
        //vertexCount = 0;
        level.Vertices.clear();
        for (int i = 0; i < d3l.rooms.size(); i++) {
            auto& room = d3l.rooms[i];
            auto& seg = level.Segments[i];

            seg.Indices.resize(room.verts.size());
            int vertexCount = (int)level.Vertices.size();
            for (int j = 0; j < room.verts.size(); j++)
                seg.Indices[j] = (uint16)(vertexCount + j);
            level.Vertices.insert(level.Vertices.end(), room.verts.begin(), room.verts.end());
            //vertexCount += (int)room.verts.size();

            seg.Connections.resize(room.faces.size());
            seg.Sides.resize(room.faces.size());
            fill(seg.Connections.begin(), seg.Connections.end(), SegID::None);
            for (int j = 0; j < room.faces.size(); j++) {
                auto& face = room.faces[j];
                auto& side = seg.Sides[j];
                int vertexCount = (int)face.face_verts.size();
                side.TMap = (LevelTexID)face.tmapIdx;
                side.Indices = face.face_verts;
                side.UVs.resize(vertexCount);
                side.Light.resize(vertexCount);
                fill(side.Light.begin(), side.Light.end(), Color(1, 1, 1));
                //side.LockLight.resize(vertexCount);
                for (int k = 0; k < vertexCount; k++)
                    side.UVs[k] = Vector2(face.face_uvls[k].u, face.face_uvls[k].v);
                //SPDLOG_INFO("Room {} face {} v0 {} seg-v0 {} v {}", i, j, side.Indices[0],
                //	seg.Indices[side.Indices[0]],
                //	level.Vertices[seg.Indices[side.Indices[0]]]);
            }
        }

        level.Objects.resize(d3l.objects.size());
        for (int i = 0; i < d3l.objects.size(); i++) {
            auto& d3obj = d3l.objects[i];
            auto& obj = level.Objects[i];
            if (d3obj.RoomNum & 0x80000000 || d3obj.Type == ObjectType::Reactor) { // external, reactor in d3l1???
                obj.Type = ObjectType::None;
                continue;
            }
            obj.Type = d3obj.Type;
            obj.ID = d3obj.ID;
            obj.Position = d3obj.Position;
            obj.Rotation = d3obj.Rotation;
            obj.Lifespan = d3obj.Lifespan == 0 ? FLT_MAX : d3obj.Lifespan;
            obj.Segment = (SegID)d3obj.RoomNum;
            obj.Contains = d3obj.Contains;
            obj.IsGeneric = IsGeneric(obj.Type);
        }

        //ReadLevelInfo(_reader, level);
        //ReadSegments(level);
        //ReadGameData(level);
        //ReadDynamicLights(level);

        for (auto& seg : level.Segments) {
            seg.UpdateGeometricProps(level);
        }

        return level;
    }
}
