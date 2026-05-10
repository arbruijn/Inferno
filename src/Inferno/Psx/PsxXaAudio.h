#pragma once

#include "Types.h"

namespace Inferno::Psx {
    // Plays XA audio sectors from the Descent 1 psx.bin image.
    bool PlayXaAudio(string_view filename, uint8 channel);

    // Stops the currently active XA stream, if any.
    void StopXaAudio();
}
