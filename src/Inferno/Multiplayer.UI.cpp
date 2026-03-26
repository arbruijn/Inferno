#include "pch.h"
#include "Game.UI.h"
#include "Game.UI.Controls.h"
#include "Game.UI.Input.h"
#include "Graphics/Render.h"
#include "Network/NetworkManager.h"
#include "Settings.h"
#include "SoundSystem.h"

namespace Inferno::UI {
    ScreenBase* ShowScreen(Ptr<ScreenBase> screen);

    class LobbyDialog : public DialogBase {
        Label* _statusLabel = nullptr;

        static std::vector<Network::LobbyPlayer> GetSortedPlayers() {
            std::vector<Network::LobbyPlayer> players;
            for (const auto& [_, player] : Network::NetworkManager::Instance().getLobbyPlayers())
                players.push_back(player);

            std::sort(players.begin(), players.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.playerId < rhs.playerId;
            });

            return players;
        }

    public:
        LobbyDialog() : DialogBase("lobby") {
            Size = Vector2(420, 260);
            CloseOnConfirm = false;
            ActionSound = "";

            _statusLabel = AddChild<Label>("", FontSize::MediumBlue);
            _statusLabel->Position = Vector2(DIALOG_PADDING, 48);
            _statusLabel->Color = GOLD_TEXT;

            auto backButton = AddChild<Button>("Back");
            backButton->Position = Vector2(DIALOG_PADDING, -DIALOG_PADDING);
            backButton->VerticalAlignment = AlignV::Bottom;
            backButton->ClickAction = [this] {
                Network::NetworkManager::Instance().disconnect();
                State = CloseState::Cancel;
            };

            auto closeButton = AddChild<Button>("Close");
            closeButton->Position = Vector2(-DIALOG_PADDING, -DIALOG_PADDING);
            closeButton->HorizontalAlignment = AlignH::Right;
            closeButton->VerticalAlignment = AlignV::Bottom;
            closeButton->ClickAction = [this] { State = CloseState::Accept; };
        }

        void OnUpdate() override {
            DialogBase::OnUpdate();

            auto& network = Network::NetworkManager::Instance();
            if (!network.isConnected()) {
                _statusLabel->SetText("status: disconnected");
                return;
            }

            if (network.isHost())
                _statusLabel->SetText(fmt::format("hosting on port {}", network.getPort()));
            else if (network.getPlayerId() == 0xFF)
                _statusLabel->SetText(fmt::format("connecting on port {}", network.getPort()));
            else
                _statusLabel->SetText(fmt::format("connected as player {}", network.getPlayerId()));
        }

        void OnDraw() override {
            DialogBase::OnDraw();

            const auto players = GetSortedPlayers();
            const auto scale = GetScale();
            float y = ScreenPosition.y + 84 * scale;

            if (players.empty()) {
                Render::DrawTextInfo dti;
                dti.Font = FontSize::Small;
                dti.Color = HELP_TEXT_COLOR;
                dti.Position = Vector2(ScreenPosition.x + DIALOG_PADDING * scale, y);
                Render::UICanvas->DrawRaw("waiting for players...", dti, Layer + 1);
                return;
            }

            for (const auto& player : players) {
                Render::DrawTextInfo dti;
                dti.Font = player.playerId == Network::NetworkManager::Instance().getPlayerId() ? FontSize::MediumGold : FontSize::Medium;
                dti.Color = player.playerId == Network::NetworkManager::Instance().getPlayerId() ? FOCUS_COLOR : WHITE_TEXT;
                dti.Position = Vector2(ScreenPosition.x + DIALOG_PADDING * scale, y);
                Render::UICanvas->DrawRaw(fmt::format("{}: {}", player.playerId, player.name), dti, Layer + 1);
                y += 24 * scale;
            }
        }
    };

    class MultiplayerDialog : public DialogBase {
        static constexpr size_t NAME_LIMIT = 24;

        TextBox* _nameInput = nullptr;
        TextBox* _hostInput = nullptr;
        Label* _statusLabel = nullptr;
        Label* _addressLabel = nullptr;

        static string TrimWhitespace(string text) {
            auto first = text.find_first_not_of(" \t\r\n");
            if (first == string::npos)
                return {};

            auto last = text.find_last_not_of(" \t\r\n");
            return text.substr(first, last - first + 1);
        }

        static string SanitizePlayerName(string text) {
            text = TrimWhitespace(std::move(text));

            if (text.empty())
                text = "Player";

            if (text.size() > NAME_LIMIT)
                text.resize(NAME_LIMIT);

            return text;
        }

        void ApplyPlayerName(bool saveToDisk) {
            auto name = SanitizePlayerName(_nameInput->GetText());
            _nameInput->SetText(name);

            const bool changed = Settings::Inferno.PlayerName != name;
            Settings::Inferno.PlayerName = name;
            Network::NetworkManager::Instance().setLocalPlayerName(name);

            if (changed && saveToDisk)
                Settings::Save();
        }

        void OpenLobbyIfConnected() {
            if (Network::NetworkManager::Instance().isConnected())
                ShowScreen(make_unique<LobbyDialog>());
        }

        void RefreshStatus() {
            auto& network = Network::NetworkManager::Instance();

            if (!network.isConnected()) {
                _statusLabel->SetText("status: offline");
                _addressLabel->SetText("host on your LAN, or join by IP/hostname");
                return;
            }

            if (network.isHost()) {
                _statusLabel->SetText(fmt::format("status: hosting on port {}", network.getPort()));

                auto addresses = network.getLocalAddresses();
                if (!addresses.empty())
                    _addressLabel->SetText(fmt::format("share {}:{}", addresses.front(), network.getPort()));
                else
                    _addressLabel->SetText(fmt::format("share your local IP and port {}", network.getPort()));

                return;
            }

            if (network.getPlayerId() == 0xFF) {
                _statusLabel->SetText(fmt::format("status: connecting on port {}", network.getPort()));
                _addressLabel->SetText("waiting for host to assign your player id");
                return;
            }

            _statusLabel->SetText(fmt::format("status: connected as player {}", network.getPlayerId()));
            _addressLabel->SetText(fmt::format("connected on port {}", network.getPort()));
        }

    public:
        MultiplayerDialog() : DialogBase("multiplayer") {
            Size = Vector2(420, 248);
            CloseOnConfirm = false;
            ActionSound = "";

            auto namePrompt = AddChild<Label>("your name", FontSize::MediumBlue);
            namePrompt->Position = Vector2(DIALOG_PADDING, 48);
            namePrompt->Color = DIALOG_TITLE_COLOR;

            _nameInput = AddChild<TextBox>(NAME_LIMIT);
            _nameInput->Position = Vector2(DIALOG_PADDING, 74);
            _nameInput->Size = Vector2(Size.x - DIALOG_PADDING * 2, CONTROL_HEIGHT);
            _nameInput->SetText(Settings::Inferno.PlayerName.empty() ? "Player" : Settings::Inferno.PlayerName);

            auto prompt = AddChild<Label>("host IP or hostname", FontSize::MediumBlue);
            prompt->Position = Vector2(DIALOG_PADDING, 108);
            prompt->Color = DIALOG_TITLE_COLOR;

            _hostInput = AddChild<TextBox>(64);
            _hostInput->Position = Vector2(DIALOG_PADDING, 134);
            _hostInput->Size = Vector2(Size.x - DIALOG_PADDING * 2, CONTROL_HEIGHT);
            _hostInput->SetText("127.0.0.1");

            _statusLabel = AddChild<Label>("", FontSize::MediumBlue);
            _statusLabel->Position = Vector2(DIALOG_PADDING, 172);
            _statusLabel->Color = GOLD_TEXT;

            _addressLabel = AddChild<Label>("", FontSize::Small);
            _addressLabel->Position = Vector2(DIALOG_PADDING, 198);
            _addressLabel->Color = HELP_TEXT_COLOR;

            auto hostButton = AddChild<Button>("Host Game");
            hostButton->Position = Vector2(DIALOG_PADDING, -DIALOG_PADDING);
            hostButton->VerticalAlignment = AlignV::Bottom;
            hostButton->ClickAction = [this] {
                ApplyPlayerName(true);
                auto& network = Network::NetworkManager::Instance();
                if (!network.isConnected())
                    network.startHost();

                RefreshStatus();
                OpenLobbyIfConnected();
            };

            auto joinButton = AddChild<Button>("Join Game");
            joinButton->Position = Vector2(0, -DIALOG_PADDING);
            joinButton->HorizontalAlignment = AlignH::Center;
            joinButton->VerticalAlignment = AlignV::Bottom;
            joinButton->ClickAction = [this] {
                ApplyPlayerName(true);
                auto& network = Network::NetworkManager::Instance();
                auto host = TrimWhitespace(_hostInput->GetText());
                if (host.empty())
                    host = "127.0.0.1";

                if (!network.isConnected())
                    network.joinGame(host);

                RefreshStatus();
                OpenLobbyIfConnected();
            };

            auto disconnectButton = AddChild<Button>("Disconnect");
            disconnectButton->Position = Vector2(-DIALOG_PADDING, -DIALOG_PADDING);
            disconnectButton->HorizontalAlignment = AlignH::Right;
            disconnectButton->VerticalAlignment = AlignV::Bottom;
            disconnectButton->ClickAction = [this] {
                ApplyPlayerName(true);
                Network::NetworkManager::Instance().disconnect();
                RefreshStatus();
            };

            RefreshStatus();
        }

        void OnUpdate() override {
            DialogBase::OnUpdate();
            ApplyPlayerName(false);
            RefreshStatus();
        }
    };

    void ShowMultiplayerDialog() {
        ShowScreen(make_unique<MultiplayerDialog>());
    }
}
