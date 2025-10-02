#pragma once
#include "Level.h"
#include "OutrageTable.h"
#include "Types.h"

namespace Inferno {
    void WriteSegmentsToOrf(Level& level, span<SegID> segs, const std::filesystem::path& path, const Outrage::GameTable& table);
    void WriteLevelToD3L(Level& level, const filesystem::path& path, const Outrage::GameTable& table);
    Level ImportOutrageLevel(const filesystem::path& path, const Outrage::GameTable& table);
    void ExportOutrageLevel(Level& level, StreamWriter& writer, const Outrage::GameTable& table);
}
