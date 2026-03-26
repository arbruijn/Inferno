#include "pch.h"
#include "Game.UI.Input.h"
#include "Graphics/Render.h"
#include "Input.h"

namespace Inferno::UI {
    using Inferno::Input::Keys;

    namespace {
        uchar TranslateSymbol(uchar keycode) {
            switch (keycode) {
                case Keys::OemSemicolon: return ';';
                case Keys::OemPlus: return '=';
                case Keys::OemComma: return ',';
                case Keys::OemMinus: return '-';
                case Keys::OemPeriod: return '.';
                case Keys::OemQuestion: return '/';
                case Keys::OemTilde: return '`';
                case Keys::OemOpenBrackets: return '[';
                case Keys::OemPipe: return '\\';
                case Keys::OemCloseBrackets: return ']';
                case Keys::OemQuotes: return '\'';
                case Keys::OemBackslash: return '/';
                default: return '\0';
            }
        }

        uchar ShiftSymbol(uchar symbol) {
            switch (symbol) {
                case ';': return ':';
                case '=': return '+';
                case ',': return '<';
                case '.': return '>';
                case '-': return '_';
                case '/': return '?';
                case '`': return '~';
                case '[': return '{';
                case '\\': return '|';
                case ']': return '}';
                case '\'': return '"';
                default: return symbol;
            }
        }

        constexpr auto NUMERIC_SHIFT_TABLE = std::to_array<uchar>({ ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' });

        uchar ShiftNumber(uchar number) {
            number -= Keys::D0;
            if (!Seq::inRange(NUMERIC_SHIFT_TABLE, number)) return number;
            return NUMERIC_SHIFT_TABLE[number];
        }
    }

    TextBox::TextBox(size_t maxLength, FontSize font) : _font(font), _maxLength(maxLength) {}

    void TextBox::SetText(string_view text) {
        _text = text;
        Padding = Vector2(4, 4);
    }

    void TextBox::OnUpdate() {
        if (!Focused) return;

        auto pressed = Input::GetPressedKeys();
        auto repeated = Input::GetRepeatedKeys();

        for (uchar i = 0; i < 255; i++) {
            if (pressed[i] || repeated[i]) {
                bool isNumeric = i >= Keys::D0 && i <= Keys::D9;
                bool isNumpad = i >= Keys::NumPad0 && i <= Keys::NumPad9;
                bool isLetter = i >= Keys::A && i <= Keys::Z;
                auto symbol = TranslateSymbol(i);
                auto isSymbol = symbol != '\0';

                if (i == Keys::Delete || i == Keys::Back) {
                    if (!_text.empty()) {
                        _text.pop_back();
                        break;
                    }
                }

                if (_text.size() >= _maxLength)
                    break;

                if (isNumpad) {
                    constexpr uchar numpadOffset = Keys::NumPad0 - Keys::D0;
                    _text += uchar(i - numpadOffset);
                }
                if (isSymbol) {
                    if (Input::ShiftDown)
                        symbol = ShiftSymbol(symbol);

                    _text += symbol;
                }
                else if (isNumeric || isLetter || isSymbol || i == Keys::Space) {
                    if (isLetter && Input::ShiftDown) {
                        constexpr uchar shift = 'a' - 'A';
                        _text += uchar(i + shift);
                    }
                    else if (isNumeric && Input::ShiftDown) {
                        _text += ShiftNumber(i);
                    }
                    else {
                        _text += i;
                    }
                }
            }
        }
    }

    void TextBox::OnDraw() {
        {
            Render::CanvasBitmapInfo cbi;
            cbi.Position = ScreenPosition;
            cbi.Size = ScreenSize;
            cbi.Texture = Render::Materials->White().Handle();
            cbi.Color = Focused ? ACCENT_COLOR : BORDER_COLOR;
            Render::UICanvas->DrawBitmap(cbi, Layer);
        }

        {
            Render::CanvasBitmapInfo cbi;
            const auto border = Vector2(1, 1) * GetScale();
            cbi.Position = ScreenPosition + border;
            cbi.Size = ScreenSize - border * 2;
            cbi.Texture = Render::Materials->White().Handle();
            cbi.Color = Color(0, 0, 0, 1);
            Render::UICanvas->DrawBitmap(cbi, Layer);
        }

        {
            Render::DrawTextInfo dti;
            dti.Font = Focused ? FontSize::MediumGold : _font;
            dti.Color = Focused ? FocusColor : TextColor;
            dti.Position = ScreenPosition / GetScale() + Margin + Padding;
            dti.EnableTokenParsing = false;
            Render::UICanvas->DrawText(_text, dti, Layer + 1);
        }

        if (!Focused) return;

        _cursorTimer += Clock.GetFrameTimeSeconds();
        while (_cursorTimer > 1) _cursorTimer -= 1;

        if (_cursorTimer > 0.5f) {
            auto offset = MeasureString(_text, FontSize::MediumGold);

            Render::DrawTextInfo dti;
            dti.Font = FontSize::MediumGold;
            dti.Color = FocusColor;
            dti.Position = ScreenPosition / GetScale() + Margin + Padding;
            dti.Position.x += offset.x;
            Render::UICanvas->DrawText("_", dti, Layer + 1);
        }
    }
}
