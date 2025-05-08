#include "OutrageTable.h"

namespace Inferno {
    // Descent 3 level reader
    class D3LevelReader {
        StreamReader _reader;
    public:
        D3LevelReader(span<ubyte> data) : _reader(data) {}
        Level Read(const Outrage::GameTable &);
    };
}