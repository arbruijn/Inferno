#pragma once

#include "Game.UI.Controls.h"

namespace Inferno::UI {
    class TextBox : public ControlBase {
        string _text;
        FontSize _font;
        float _cursorTimer = 0;
        size_t _maxLength;

    public:
        bool NumericMode = false;
        bool EnableSymbols = false; // Enable non-numeric, non-alphabetical characters
        Color TextColor = Color(1, 1, 1);
        Color FocusColor = FOCUS_COLOR;

        TextBox(size_t maxLength = 100, FontSize font = FontSize::Medium);

        void SetText(string_view text);
        const string& GetText() const { return _text; }

        void OnUpdate() override;
        void OnDraw() override;
    };
}
