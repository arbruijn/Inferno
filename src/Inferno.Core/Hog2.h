#pragma once

#include <utility>
#include "Types.h"
#include "Streams.h"
#include "HogFile.h"

// Descent 3 HOG2 file
namespace Inferno {
    class Hog2 : public HogFile {
        static constexpr int PSFILENAME_LEN = 35;
        static constexpr int HOG_HDR_SIZE = 64;

        Dictionary<string, int> _lookup;
    public:
        //filesystem::path Path;

        /*
        struct Entry : public HogEntry {
            //string name;
            uint Flags;
            //uint len;
            uint Timestamp;
            //int64 offset;
        };
        */

        static Hog2 Read(const filesystem::path& path) {
            Hog2 hog;
            hog.Path = path;

            StreamReader r(path);
            auto id = r.ReadString(4);
            if (id != "HOG2")
                throw Exception("Not a HOG2 file");

            uint nfiles = r.ReadUInt32();
            size_t file_data_offset = r.ReadUInt32();

            hog.Entries.reserve(nfiles);

            r.Seek(4 + HOG_HDR_SIZE);
            size_t offset = file_data_offset;
            for (uint i = 0; i < nfiles; i++) {
                auto& entry = hog.Entries.emplace_back();
                entry.Name = String::ToLower(r.ReadString(PSFILENAME_LEN + 1));
                entry.Flags = r.ReadUInt32();
                entry.Size = r.ReadUInt32();
                entry.Timestamp = r.ReadUInt32();
                entry.Offset = offset;
                offset += entry.Size;

                hog._lookup.insert({ entry.Name, i });
            }

            return hog;
        }

        //List<Entry> Entries;

        List<ubyte> ReadEntry(int index) {
            if (!Seq::inRange(Entries, index))
                throw Exception("Invalid entry index");

            StreamReader r(Path);
            const auto& entry = Entries[index];
            r.Seek(entry.Offset);
            List<ubyte> data(entry.Size);
            r.ReadBytes(data);
            return data;
        }

        Option<List<ubyte>> ReadEntry(string name) {
            name = String::ToLower(name);
            if (!_lookup.contains(name))
                return {};
            
            return ReadEntry(_lookup[name]);
        }
    };

    class Hog2Writer {
        std::ofstream _stream;
        StreamWriter _writer;
        size_t _maxEntries;
        uint _offset;
        List<HogEntry> _entries{};
        static constexpr int PSFILENAME_LEN = 35;
        static constexpr int HOG_HDR_SIZE = 64;
        static constexpr int HOG_USED_HDR_SIZE = 8;
        static constexpr int HOG_ENTRY_SIZE = PSFILENAME_LEN + 1 + 4 + 4 + 4;

    public:
        Hog2Writer(const filesystem::path& path, size_t maxEntries) : _stream(path, std::ios::binary), _writer(_stream) {
            _writer.WriteString("HOG2", 4);
            uint dataOfs = (uint)(4 + 8 + maxEntries * HOG_ENTRY_SIZE);
            _writer.Write((uint)0);
            _writer.Write(dataOfs);
            _writer.WriteBytes(Array<ubyte, HOG_HDR_SIZE - HOG_USED_HDR_SIZE>{});
            _writer.WriteBytes(List<ubyte>(HOG_ENTRY_SIZE * maxEntries));
            _maxEntries = maxEntries;
        }

        void WriteEntry(std::string& name, List<ubyte> data, uint flags, uint timestamp) {
            HogEntry entry { .Name = name, .Flags = flags, .Timestamp = timestamp };
            entry.Size = data.size();
            _entries.push_back(std::move(entry));
            _writer.Write(data);
        }

        void Finish() {
            _writer.Seek(4);
            _writer.Write((uint)_entries.size());
            _writer.Seek(4 + HOG_HDR_SIZE);
            for (auto& entry : _entries) {
                _writer.WriteString(entry.Name, PSFILENAME_LEN + 1);
                _writer.Write(entry.Flags);
                _writer.Write((uint)entry.Size);
                _writer.Write(entry.Timestamp);
            }
        }
    };
}