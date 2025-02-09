#include "pch.h"
#include "Resources.h"
#include "FileSystem.h"
#include "Sound.h"
#include "Pig.h"
#include <fstream>
#include <mutex>
#include "Game.h"
#include "logging.h"
#include "Graphics/Render.h"

namespace Inferno::Resources {
    List<string> RobotNames;
    List<string> PowerupNames;

    HogFile Hog;
    SoundFile SoundsD1, SoundsD2;
    Palette LevelPalette;
    PigFile Pig;
    Dictionary<TexID, PigBitmap> CustomTextures;
    List<PigBitmap> Textures;

    std::mutex PigMutex;

    void LoadRobotNames(filesystem::path path) {
        try {
            std::ifstream file(path);
            string line;
            while (std::getline(file, line))
                RobotNames.push_back(line);
        }
        catch (...) {
            SPDLOG_ERROR(L"Error reading robot names from `{}`", path.wstring());
        }
    }

    string GetRobotName(uint id) {
        if (id >= RobotNames.size()) return "Unknown";
        return RobotNames[id];
    }

    void LoadPowerupNames(filesystem::path path) {
        try {
            std::ifstream file(path);
            string line;
            while (std::getline(file, line))
                PowerupNames.push_back(line);
        }
        catch (...) {
            SPDLOG_ERROR(L"Error reading powerup names from `{}`", path.wstring());
        }
    }

    Option<string> GetPowerupName(uint id) {
        if (id >= PowerupNames.size() || PowerupNames[id] == "(not used)") return {};
        return { PowerupNames[id] };
    }

    void Init() {
        // Load some default resources.
        LoadPowerupNames("powerups.txt");
        LoadRobotNames("robots.txt");
    }

    const WallClip& GetWallClip(WClipID id) {
        return GameData.WallClips[(int)id];
    }

    const WallClip* TryGetWallClip(WClipID id) {
        if (!Seq::inRange(GameData.WallClips, (int)id)) return nullptr;
        return &GameData.WallClips[(int)id];
    }

    //const WallClip* TryGetWallClip(LevelTexID id) {
    //    for (int i = 0; i < GameData.WallClips.size(); i++) {
    //        if (GameData.WallClips[i].NumFrames < 0) continue;
    //        if (GameData.WallClips[i].Frames[0] == id)
    //            return &GameData.WallClips[i];
    //    }

    //    return nullptr;
    //}

    WClipID GetWallClipID(LevelTexID id) {
        for (int i = 0; i < GameData.WallClips.size(); i++) {
            if (GameData.WallClips[i].Frames[0] == id)
                return WClipID(i);
        }

        return WClipID::None;
    }

    const EffectClip& GetEffectClip(EClipID id) {
        return GameData.Effects[(int)id];
    }

    const EffectClip* TryGetEffectClip(LevelTexID id) {
        auto tid = LookupLevelTexID(id);
        if (tid == TexID::None) return nullptr;
        return TryGetEffectClip(tid);
    }

    const EffectClip* TryGetEffectClip(TexID id) {
        for (auto& clip : GameData.Effects) {
            if (clip.VClip.Frames[0] == id)
                return &clip;
        }

        return nullptr;
    }

    EClipID GetEffectClip(TexID tid) {
        for (int i = 0; i < GameData.Effects.size(); i++) {
            if (GameData.Effects[i].VClip.Frames[0] == tid)
                return EClipID(i);
        }

        return EClipID::None;
    }

    EClipID GetEffectClip(LevelTexID id) {
        auto tid = LookupLevelTexID(id);
        if (tid == TexID::None) return EClipID::None;
        return GetEffectClip(tid);
    }

    VClip DefaultVClip{};

    const VClip& GetVideoClip(VClipID id) {
        if (GameData.VClips.size() <= (int)id) return DefaultVClip;
        return GameData.VClips[(int)id];
    }

    Model DefaultModel{};
    RobotInfo DefaultRobotInfo{};

    const Inferno::Model& GetModel(ModelID id) {
        if ((int)id >= GameData.Models.size()) return DefaultModel;
        return GameData.Models[(int)id];
    }

    const RobotInfo& GetRobotInfo(uint id) {
        if (id >= GameData.Robots.size()) return DefaultRobotInfo;
        return GameData.Robots[id];
    }

    List<TexID> CopyLevelTextureLookup() {
        return GameData.AllTexIdx;
    }

    TexID LookupLevelTexID(LevelTexID tid) {
        auto id = (int)tid;
        if (!Seq::inRange(GameData.AllTexIdx, id)) return TexID::None;
        return TexID((int)GameData.AllTexIdx[id]);
    }

    const LevelTexture* TryGetLevelTextureInfo(LevelTexID id) {
        if (!Seq::inRange(GameData.TexInfo, (int)id)) return nullptr; // fix for invalid ids in some levels
        return &GameData.TexInfo[(int)id];
    }

    const LevelTexture DefaultTexture{};

    const LevelTexture& GetLevelTextureInfo(LevelTexID id) {
        if (!Seq::inRange(GameData.TexInfo, (int)id)) return DefaultTexture; // fix for invalid ids in some levels
        return GameData.TexInfo[(int)id];
    }

    const LevelTexture& GetLevelTextureInfo(TexID id) {
        if (!Seq::inRange(GameData.LevelTexIdx, (int)id)) return DefaultTexture;
        auto ltid = GameData.LevelTexIdx[(int)id];
        return GetLevelTextureInfo(ltid);
    }

    LevelTexID GetDestroyedTexture(LevelTexID id) {
        if (id <= LevelTexID::Unset) return LevelTexID::None;

        auto& info = Resources::GetLevelTextureInfo(id);
        if (info.EffectClip != EClipID::None)
            return GetEffectClip(info.EffectClip).DestroyedTexture;
        else
            return info.DestroyedTexture;
    }

    const PigEntry& GetTextureInfo(TexID id) {
        return Pig.Get(id);
    }

    const PigEntry* TryGetTextureInfo(TexID id) {
        if (id <= TexID::Invalid || (int)id >= Pig.Entries.size()) return nullptr;
        return &Pig.Get(id);
    }

    const PigEntry& GetTextureInfo(LevelTexID id) {
        return Pig.Get(LookupLevelTexID(id));
    }

    const PigEntry* TryGetTextureInfo(LevelTexID id) {
        return TryGetTextureInfo(LookupLevelTexID(id));
    }

    int GetSoundCount() { return (int)GameData.Sounds.size(); }

    int GetSoundIndex(SoundID id) {
        return GameData.Sounds[(int)id];
    }

    Sound::SoundResource GetSoundResource(SoundID id) {
        if (Game::Level.IsDescent1())
            return { .D1 = GameData.Sounds[(int)id] };
        else
            return { .D2 = GameData.Sounds[(int)id] };
    }

    TexID LookupModelTexID(const Model& m, int16 i) {
        if (i >= m.TextureCount || m.FirstTexture + i >= GameData.ObjectBitmapPointers.size()) return TexID::None;
        auto ptr = GameData.ObjectBitmapPointers[m.FirstTexture + i];
        return GameData.ObjectBitmaps[ptr];
    }

    string ReplaceExtension(string src, string ext) {
        auto offset = src.find('.');
        if (!ext.starts_with('.')) ext = "." + ext;
        if (offset == string::npos) return src + ext;
        return src.substr(0, offset) + ext;
    }

    void UpdateAverageTextureColor() {
        SPDLOG_INFO("Update average texture color");

        for (auto& entry : Pig.Entries) {
            auto& bmp = ReadBitmap(entry.ID);
            entry.AverageColor = GetAverageColor(bmp.Data);
        }
        //for (auto& tid : GameData.LevelTexIdx) {
        //    auto id = LookupLevelTexID(tid);
        //    if (!Seq::inRange(Pig.Entries, (int)id)) continue;

        //    auto& entry = Pig.Entries[(int)id];
        //    auto& bmp = ReadBitmap(id);
        //    entry.AverageColor = GetAverageColor(bmp.Data);
        //}
    }

    void LoadDescent2Resources(Level& level) {
        std::scoped_lock lock(PigMutex);
        SPDLOG_INFO("Loading Descent 2 level: '{}'\r\n Version: {} Segments: {} Vertices: {}", level.Name, level.Version, level.Segments.size(), level.Vertices.size());
        StreamReader reader(FileSystem::FindFile(L"descent2.ham"));
        auto ham = ReadHam(reader);
        auto hog = HogFile::Read(FileSystem::FindFile(L"descent2.hog"));
        auto pigName = ReplaceExtension(level.Palette, ".pig");
        auto pig = ReadPigFile(FileSystem::FindFile(pigName));

        auto paletteData = hog.ReadEntry(level.Palette);
        auto palette = ReadPalette(paletteData);
        auto textures = ReadAllBitmaps(pig, palette);

        if (level.IsVertigo()) {
            auto d2xhog = HogFile::Read(FileSystem::FindFile(L"d2x.hog"));
            auto data = d2xhog.ReadEntry("d2x.ham");
            StreamReader d2xreader(data);
            AppendVHam(d2xreader, ham);
        }

        auto pog = ReplaceExtension(level.FileName, ".pog");
        if (Game::Mission && Game::Mission->Exists(pog)) {
            SPDLOG_INFO("POG data found");
            auto data = Game::Mission->ReadEntry(pog);
            CustomTextures = ReadPoggies(pig.Entries, data, palette);
        }

        // Everything loaded okay, set the internal data
        LevelPalette = std::move(palette);
        Pig = std::move(pig);
        Hog = std::move(hog);
        GameData = std::move(ham);
        Textures = std::move(textures);

        // Read hxm
        auto hxm = ReplaceExtension(level.FileName, ".hxm");

        if (Game::Mission && Game::Mission->Exists(hxm)) {
            SPDLOG_INFO("Loading HXM data...");
            auto hxmData = Game::Mission->ReadEntry(hxm);
            StreamReader hxmReader(hxmData);
            ReadHXM(hxmReader, GameData);
        }
    }

    void LoadSounds() {
        if (FoundDescent1()) {
            try {
                // Unfortunately have to parse the whole pig file because there's no specialized method
                // for just reading sounds
                auto hog = HogFile::Read(FileSystem::FindFile(L"descent.hog"));
                auto paletteData = hog.ReadEntry("palette.256");
                auto palette = ReadPalette(paletteData);

                auto path = FileSystem::FindFile(L"descent.pig");
                StreamReader reader(path);
                auto [ham, pig, sounds] = ReadDescent1GameData(reader, palette);
                sounds.Path = path;
                SoundsD1 = std::move(sounds);
            }
            catch (const std::exception&) {
                SPDLOG_ERROR("Unable to read D1 sound data");
            }
        }

        if (auto s22 = FileSystem::TryFindFile(L"descent2.s22")) {
            SoundsD2 = ReadSoundFile(*s22);
        }
    }

    // Some levels don't have the D1 reactor model set
    void FixD1ReactorModel(Level& level) {
        for (auto& obj : level.Objects) {
            if (obj.Type == ObjectType::Reactor) {
                obj.ID = 0;
                obj.Render.Model.ID = ModelID(39);
            }
        }
    }

    void LoadDescent1Resources(Level& level) {
        std::scoped_lock lock(PigMutex);
        SPDLOG_INFO("Loading Descent 1 level: '{}'\r\n Version: {} Segments: {} Vertices: {}", level.Name, level.Version, level.Segments.size(), level.Vertices.size());
        auto hog = HogFile::Read(FileSystem::FindFile(L"descent.hog"));
        auto paletteData = hog.ReadEntry("palette.256");
        auto palette = ReadPalette(paletteData);

        auto path = FileSystem::FindFile(L"descent.pig");
        StreamReader reader(path);
        auto [ham, pig, sounds] = ReadDescent1GameData(reader, palette);
        pig.Path = path;
        sounds.Path = path;
        //ReadBitmap(pig, palette, TexID(61)); // cockpit
        auto textures = ReadAllBitmaps(pig, palette);

        auto dtx = ReplaceExtension(level.FileName, ".dtx");
        if (Game::Mission && Game::Mission->Exists(dtx)) {
            SPDLOG_INFO("DTX data found");
            auto data = Game::Mission->ReadEntry(dtx);
            CustomTextures = ReadDTX(pig.Entries, data, palette);
        }

        FixD1ReactorModel(level);

        // Everything loaded okay, set the internal data
        Textures = std::move(textures);
        LevelPalette = std::move(palette);
        Pig = std::move(pig);
        Hog = std::move(hog);
        GameData = std::move(ham);
    }

    void UpdateObjectRadii(Level& level) {
        SPDLOG_INFO(L"Updating object radii");
        for (auto& obj : level.Objects) {
            switch (obj.Type) {
                case ObjectType::Robot:
                {
                    auto& info = Resources::GetRobotInfo(obj.ID);
                    auto& model = Resources::GetModel(info.Model);
                    obj.Radius = model.Radius;
                    break;
                }
                case ObjectType::Coop:
                case ObjectType::Player:
                case ObjectType::Reactor:
                {
                    auto& model = Resources::GetModel(obj.Render.Model.ID);
                    obj.Radius = model.Radius;
                    break;
                }
            }
        }
    }

    void ResetResources() {
        LevelPalette = {};
        Pig = {};
        Hog = {};
        GameData = {};
        CustomTextures.clear();
        Textures.clear();
    }

    // Some old levels didn't properly set the render model ids.
    void FixObjectModelIds(Level& level) {
        for (auto& obj : level.Objects) {
            switch (obj.Type) {
                case ObjectType::Robot:
                    obj.Render.Model.ID = Resources::GetRobotInfo(obj.ID).Model;
                    break;

                case ObjectType::Weapon:
                    obj.Render.Model.ID = Models::PlaceableMine;
                    break;

                case ObjectType::Player:
                    obj.Render.Model.ID = level.IsDescent1() ? Models::D1Player : Models::D2Player;
                    break;

                case ObjectType::Coop:
                    obj.Render.Model.ID = level.IsDescent1() ? Models::D1Coop : Models::D2Coop;
                    break;

            }
        }
    }

    void LoadLevel(Level& level) {
        try {
            ResetResources();

            if (level.IsDescent2()) {
                LoadDescent2Resources(level);
            }
            else if (level.IsDescent1()) {
                LoadDescent1Resources(level);
            }
            else {
                throw Exception("Unsupported level version");
            }

            UpdateAverageTextureColor();

            FixObjectModelIds(level);
        }
        catch (const std::exception& e) {
            SPDLOG_ERROR(e.what());
        }
    }

    const PigBitmap& ReadBitmap(TexID id) {
        //std::scoped_lock lock(PigMutex);
        if (Textures.empty()) {
            assert(!Textures.empty());
            static const PigBitmap empty(64, 64, "default");
            return empty;
        }

        if (CustomTextures.contains(id)) return CustomTextures[id];
        if (!Seq::inRange(Textures, (int)id)) id = (TexID)0;
        return Textures[(int)id];
    }

    List<ubyte> ReadFile(string file) {
        // Search mounted mission first
        if (Game::Mission && Game::Mission->Exists(file))
            return Game::Mission->ReadEntry(file);

        // Then main hog file
        if (Hog.Exists(file))
            return Hog.ReadEntry(file);

        SPDLOG_ERROR("File not found: {}", file);
        throw Exception("File not found");
    }

    Level ReadLevel(string name) {
        SPDLOG_INFO("Reading level {}", name);
        auto data = ReadFile(name);
        auto level = Level::Deserialize(data);
        level.FileName = name;
        return level;
    }

    bool FoundDescent1() { return FileSystem::TryFindFile("descent.hog").has_value(); }
    bool FoundDescent2() { return FileSystem::TryFindFile("descent2.hog").has_value(); }
    bool FoundDescent3() { return FileSystem::TryFindFile("d3.hog").has_value(); }
    bool FoundVertigo() { return FileSystem::TryFindFile("d2x.hog").has_value(); }
    bool FoundMercenary() { return FileSystem::TryFindFile("merc.hog").has_value(); }

    bool HasCustomTextures() {
        return !CustomTextures.empty();
    }

    // Opens a file stream from the data paths or the loaded hogs
    Option<StreamReader> OpenFile(const string& name) {
        // Check file system first, then hogs
        if (auto path = FileSystem::TryFindFile(name))
            return StreamReader(*path);
        else if (auto data = Descent3Hog.ReadEntry(name))
            return StreamReader(std::move(*data), name);

        return {};
    }

    void LoadVClips() {
        for (auto& tex : GameTable.Textures) {
            if (!tex.Animated()) continue;

            if (auto r = OpenFile(tex.FileName)) {
                auto vc = Outrage::VClip::Read(*r);
                if (vc.Frames.size() > 0)
                    vc.FrameTime = tex.Speed / vc.Frames.size();
                vc.FileName = tex.FileName;
                VClips.push_back(std::move(vc));
            }
        }
    }

    void MountDescent3() {
        try {
            if (auto path = FileSystem::TryFindFile("d3.hog")) {
                SPDLOG_INFO(L"Loading {} and Table.gam", path->wstring());
                Descent3Hog = Hog2::Read(*path);
                if (auto r = OpenFile("Table.gam"))
                    GameTable = Outrage::GameTable::Read(*r);

                LoadVClips();
            }

            //if (auto path = FileSystem::TryFindFile("merc.hog")) {
            //    Mercenary = Hog2::Read(*path);
            //}
        }
        catch (const std::exception& e) {
            SPDLOG_ERROR("Error loading Descent 3\n{}", e.what());
        }
    }

    Option<Outrage::Bitmap> ReadOutrageBitmap(const string& name) {
        if (auto r = OpenFile(name))
            return Outrage::Bitmap::Read(*r);

        return {};
    }

    Option<Outrage::Model> ReadOutrageModel(const string& name) {
        if (auto r = OpenFile(name))
            return Outrage::Model::Read(*r);

        return {};
    }

    Dictionary<string, Outrage::Model> OutrageModels;

    Outrage::Model const* GetOutrageModel(const string& name) {
        if (OutrageModels.contains(name))
            return &OutrageModels[name];

        if (auto model = ReadOutrageModel(name)) {
            for (auto& texture : model->Textures) {
                model->TextureHandles.push_back(Render::NewTextureCache->ResolveFileName(texture));
            }
            OutrageModels[name] = std::move(*model);
            return &OutrageModels[name];
        }

        return nullptr;
    }
};
