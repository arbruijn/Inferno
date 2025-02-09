#include "pch.h"
#include "OutrageTable.h"

namespace Inferno::Outrage {
    constexpr auto PAGENAME_LEN = 35;

    enum PageType {
        PAGETYPE_TEXTURE = 1,
        PAGETYPE_DOOR = 5,
        PAGETYPE_SOUND = 7,
        PAGETYPE_GENERIC = 10,
    };

    constexpr int MAX_STRING_LEN = 256;

    SoundInfo ReadSoundPage(StreamReader& r) {
        constexpr int KNOWN_VERSION = 1;
        auto version = r.ReadInt16();
        if (version > KNOWN_VERSION)
            throw Exception("Unsupported texture info version");

        SoundInfo si{};
        si.Name = r.ReadCString(PAGENAME_LEN);
        si.FileName = r.ReadCString(PAGENAME_LEN);
        si.Flags = r.ReadInt32();
        si.LoopStart = r.ReadInt32();
        si.LoopEnd = r.ReadInt32();
        si.OuterConeVolume = r.ReadFloat();
        si.InnerConeAngle = r.ReadInt32();
        si.OuterConeAngle = r.ReadInt32();
        si.MaxDistance = r.ReadFloat();
        si.MinDistance = r.ReadFloat();
        si.ImportVolume = r.ReadFloat();
        return si;
    }

    TextureInfo ReadTexturePage(StreamReader& r) {
        constexpr int KNOWN_VERSION = 7;
        auto version = r.ReadInt16();
        if (version > KNOWN_VERSION)
            throw Exception("Unsupported texture info version");

        TextureInfo tex{};
        tex.Name = r.ReadCString(MAX_STRING_LEN);
        tex.FileName = r.ReadCString(MAX_STRING_LEN);
        r.ReadCString(MAX_STRING_LEN);
        tex.Color.x = r.ReadFloat();
        tex.Color.y = r.ReadFloat();
        tex.Color.z = r.ReadFloat();
        tex.Color.w = r.ReadFloat();

        tex.Speed = r.ReadFloat();
        tex.Slide.x = r.ReadFloat();
        tex.Slide.y = r.ReadFloat();
        tex.Reflectivity = r.ReadFloat();

        tex.Corona = r.ReadByte();
        tex.Damage = r.ReadInt32();

        tex.Flags = (TextureFlag)r.ReadInt32();

        if (tex.Procedural()) {
            for (int i = 0; i < 255; i++)
                /*tex.Procedural.Palette[i] = */r.ReadInt16();
                
            r.ReadByte(); // heat
            r.ReadByte(); // light
            r.ReadByte(); // thickness
            r.ReadFloat(); // eval time
            if (version >= 6) {
                r.ReadFloat(); // osc time
                r.ReadByte(); // osc value
            }
            int n = r.ReadInt16(); // elements
            for (int i = 0; i < n; i++) {
                r.ReadByte(); // type
                r.ReadByte(); // frequency
                r.ReadByte(); // speed
                r.ReadByte(); // size
                r.ReadByte(); // x1
                r.ReadByte(); // y1
                r.ReadByte(); // x2
                r.ReadByte(); // y2
            }
        }

        if (version >= 5) {
            if (version < 7)
                r.ReadInt16();
            else
                tex.Sound = r.ReadCString(MAX_STRING_LEN);
            r.ReadFloat();
        }
        return tex;
    }

    GameTable GameTable::Read(StreamReader& r) {
        GameTable table{};

        while (!r.EndOfStream()) {
            auto pageType = r.ReadByte();
            auto pageStart = r.Position();
            auto len = r.ReadInt32();
            if (len <= 0) throw Exception("bad page length");

            switch (pageType) {
                case PAGETYPE_TEXTURE:
                    table.Textures.push_back(ReadTexturePage(r));
                    break;

                case PAGETYPE_SOUND:
                    table.Sounds.push_back(ReadSoundPage(r));
                    break;
            }

            //auto readbytes = r.Position() - pageStart;
            r.Seek(pageStart + len); // seek to next chunk (prevents read errors due to individual chunks)
        }

        return table;
    }
}
