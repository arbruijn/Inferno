#include "pch.h"
#include "Game.Input.h"
#include "Editor/Editor.h"
#include "Game.Bindings.h"
#include "Game.h"
#include "Game.Reactor.h"
#include "Game.UI.h"
#include "Resources.h"
#include "Settings.h"
#include "Editor/Events.h"
#include "Game.Automap.h"
#include "Graphics.h"
#include "HUD.h"
#include "SoundSystem.h"
#include "SystemClock.h"

namespace Inferno {
    using Keys = Input::Keys;

    namespace {
        struct CheatEntry {
            std::string_view Code;
            std::function<void()> Action;
        };

        void EnableCheats() {
            if (Game::Cheater)
                return;

            Game::Cheater = true;
            PrintHudMessage("cheats enabled");
            Sound::Play2D({ SoundID::Cheater });
        }

        void GiveAllPrimaryWeapons() {
            auto& player = Game::Player;
            static constexpr std::array weapons = {
                PrimaryWeaponIndex::Laser,
                PrimaryWeaponIndex::Vulcan,
                PrimaryWeaponIndex::Spreadfire,
                PrimaryWeaponIndex::Plasma,
                PrimaryWeaponIndex::Fusion,
                /*PrimaryWeaponIndex::SuperLaser,
                PrimaryWeaponIndex::Gauss,
                PrimaryWeaponIndex::Helix,
                PrimaryWeaponIndex::Phoenix,
                PrimaryWeaponIndex::Omega*/
            };

            for (auto weapon : weapons) {
                player.GiveWeapon(weapon);
                player.PrimaryAmmo[(int)weapon] = player.Ship.Weapons[(int)weapon].Ammo;
            }
        }

        void GiveScourgeWeapons() {
            auto& player = Game::Player;
            static constexpr std::array weapons = {
                PrimaryWeaponIndex::Vulcan,
                PrimaryWeaponIndex::Spreadfire,
                PrimaryWeaponIndex::Plasma,
                //SecondaryWeaponIndex::Concussion
                //SecondaryWeaponIndex::Homing,
                //SecondaryWeaponIndex::ProxMine
            };

            for (auto weapon : weapons) {
                player.GiveWeapon(weapon);
                player.PrimaryAmmo[(int)weapon] = player.Ship.Weapons[(int)weapon].Ammo;
            }
        }

        void GiveAllSecondaryWeapons() {
            auto& player = Game::Player;
            for (int i = 0; i < 5 /*((int)SecondaryWeaponIndex::None*/; ++i) {
                player.SecondaryWeapons |= (1 << i);
                player.SecondaryAmmo[i] = (i == (int)SecondaryWeaponIndex::ProximityMine || i == (int)SecondaryWeaponIndex::SmartMine) ? 99 : 200;
            }
        }

        bool TryApplyCheat(string_view code) {
            if (code == "GABBAGABBAHEY") {
                EnableCheats();
                return true;
            }

            if (!Game::Cheater)
                return false;

            static const std::array cheats = {
                CheatEntry{ "RACERX", [] {
                    Settings::Cheats.Invulnerable = true;
                    if (auto player = Game::Level.TryGetObject(Game::Player.Reference))
                        Game::MakeInvulnerable(*player, -1, false);
                }},
                CheatEntry{ "MITZI", [] {
                    Game::Player.SetPowerup(PowerupFlag::BlueKey, true);
                    Game::Player.SetPowerup(PowerupFlag::GoldKey, true);
                    Game::Player.SetPowerup(PowerupFlag::RedKey, true);
                }},
                CheatEntry{ "GUILE", [] {
                    Settings::Cheats.Cloaked = true;
                    if (auto player = Game::Level.TryGetObject(Game::Player.Reference))
                        Game::CloakObject(*player, -1, false);
                }},
                CheatEntry{ "TWILIGHT", [] {
                    Game::Player.Shields = MAX_SHIELDS;
                }},
                CheatEntry{ "FARMERJOE", [] {
                    UI::ShowCheatWarpDialog();
                }},
                CheatEntry{ "BUGGIN", [] {
                    Game::SetTimeScale(2.0f, 0.5f);
                }},
                CheatEntry{ "BRUIN", [] {
                    Game::Player.GiveExtraLife(1);
                }},
                CheatEntry{ "BIOPSYTOYS", [] {
                    for (auto& obj : Game::Level.Objects) {
                        if (obj.Type == ObjectType::Reactor) {
                            DestroyObject(obj);
                            break;
                        }
                    }
                }},
                CheatEntry{ "ASTRAL", [] {
                    Settings::Cheats.Ghost = true;
                    PrintHudMessage("ghost mode");
                }},
                CheatEntry{ "LUNACY", [] {
                    Settings::Cheats.Lunacy = true;
                    PrintHudMessage("lunacy");
                }},
                CheatEntry{ "BIGRED", [] {
                    GiveAllPrimaryWeapons();
                    GiveAllSecondaryWeapons();
                    Game::Player.Energy = MAX_ENERGY;
                }},
                CheatEntry{ "SCOURGE", [] {
                    GiveScourgeWeapons();
                }},
                CheatEntry{ "PORGYS", [] {
                    GiveAllPrimaryWeapons();
                    GiveAllSecondaryWeapons();
                    Game::Player.Energy = MAX_ENERGY;
                    Game::Player.Shields = MAX_SHIELDS;
                }},
                CheatEntry{ "AHIMSA", [] {
                    Settings::Cheats.Ahimsa = true;
                    PrintHudMessage("robots will not shoot");
                }},
            };

            for (auto& cheat : cheats) {
                if (code == cheat.Code) {
                    EnableCheats();
                    cheat.Action();
                    Sound::Play2D({ SoundID::Cheater });
                    return true;
                }
            }

            return false;
        }

        void ProcessCheatCodes() {
            auto typed = Input::ConsumeTextInput();
            if (typed.empty())
                return;

            static std::string buffer;
            constexpr size_t maxCodeLen = 13;

            for (char ch : typed) {
                if (!std::isalpha((unsigned char)ch))
                    continue;

                buffer.push_back((char)std::toupper((unsigned char)ch));
                if (buffer.size() > maxCodeLen)
                    buffer.erase(0, buffer.size() - maxCodeLen);

                for (size_t len = std::min(buffer.size(), maxCodeLen); len > 0; --len) {
                    auto code = string_view(buffer).substr(buffer.size() - len);
                    if (TryApplyCheat(code))
                        break;
                }
            }
        }
    }

    void CheckDeveloperHotkeys() {
        auto state = Game::GetState();
        if (state == GameState::Game)
            ProcessCheatCodes();
        else
            Input::ConsumeTextInput();

        if (!Settings::Inferno.EnableDevHotkeys)
            return;

        if (state == GameState::Game) {
            if (Input::OnKeyPressed(Keys::Back) && Input::AltDown)
                Game::BeginSelfDestruct();

            if (Input::OnKeyPressed(Keys::OemPipe) && Input::AltDown) {
                Game::WarpPlayerToExit();
                Game::BeginSelfDestruct();
                Inferno::Sound::Play2D({ SoundID::Cheater });
            }

            if (Input::OnKeyPressed(Keys::M) && Input::AltDown) {
                Game::Automap.RevealFullMap();
                PrintHudMessage("full map!");
                Inferno::Sound::Play2D({ SoundID::Cheater });
            }

            //if (Input::OnKeyPressed(Keys::R)) {
            //    static bool toggle = false;

            //    if (toggle)
            //        Game::SetTimeScale(1, 1.0f);
            //    else
            //        Game::SetTimeScale(0.5f, 0.75f);

            //    toggle = !toggle;
            //}
        }

        if (Input::OnKeyPressed(Keys::F2)) {
            if (state == GameState::MainMenu) {
                Game::SetState(GameState::Editor);
            }
            else {
                Game::PlayingFromEditor = true;
                Game::Player.Lives = Player::INITIAL_LIVES;
                Game::Player.stats.StartLevel();
                Game::SetState(state != GameState::Editor ? GameState::Editor : GameState::LoadLevel);
            }
        }

        if (Input::OnKeyPressed(Keys::F3))
            Settings::Editor.HideUI = !Settings::Editor.HideUI;

        if (Input::OnKeyPressed(Keys::F5)) {
            Sound::UnloadNamedSounds(); // Force a reload of sounds
            Resources::LoadDataTables(Game::Level);
            Graphics::ReloadResources();
            Editor::Events::MaterialsChanged();
        }

        if (Input::OnKeyPressed(Keys::F6))
            Graphics::ReloadTextures();

        //if (Input::OnKeyPressed(Keys::F7)) {
        //    Settings::Graphics.HighRes = !Settings::Graphics.HighRes;
        //    Graphics::ReloadTextures();
        //}

        if (Input::OnKeyPressed(Keys::F9)) {
            Settings::Graphics.NewLightMode = !Settings::Graphics.NewLightMode;
        }

        if (Input::OnKeyPressed(Keys::F10)) {
            Settings::Graphics.ToneMapper++;
            if (Settings::Graphics.ToneMapper > 2) Settings::Graphics.ToneMapper = 0;
        }
    }

    void HandleEditorDebugInput(float /*dt*/) {
        if (Game::Level.Objects.empty()) return;

        auto& obj = Game::Level.Objects[0];
        auto& physics = obj.Physics;

        physics.Thrust = Vector3::Zero;
        physics.AngularThrust = Vector3::Zero;
        auto maxAngularThrust = Resources::GameData.PlayerShip.MaxRotationalThrust;
        auto maxThrust = Resources::GameData.PlayerShip.MaxThrust;

        if (Input::IsKeyDown(Keys::Add))
            physics.Thrust += obj.Rotation.Forward() * maxThrust;

        if (Input::IsKeyDown(Keys::Subtract))
            physics.Thrust += obj.Rotation.Backward() * maxThrust;

        if (Input::IsKeyDown(Keys::NumPad1))
            physics.Thrust += obj.Rotation.Left() * maxThrust;

        if (Input::IsKeyDown(Keys::NumPad3))
            physics.Thrust += obj.Rotation.Right() * maxThrust;

        // pitch
        if (Input::IsKeyDown(Keys::NumPad5))
            physics.AngularThrust.x = -maxAngularThrust;
        if (Input::IsKeyDown(Keys::NumPad8))
            physics.AngularThrust.x = maxAngularThrust;

        // yaw
        if (Input::IsKeyDown(Keys::NumPad4))
            physics.AngularThrust.y = -maxAngularThrust;
        if (Input::IsKeyDown(Keys::NumPad6))
            physics.AngularThrust.y = maxAngularThrust;

        // roll
        if (Input::IsKeyDown(Keys::NumPad7))
            physics.AngularThrust.z = -maxAngularThrust;
        if (Input::IsKeyDown(Keys::NumPad9))
            physics.AngularThrust.z = maxAngularThrust;

        // Weapons
        if (Input::IsKeyDown(Keys::Enter))
            Game::Player.FirePrimary();
        if (Input::IsKeyDown(Keys::Decimal))
            Game::Player.FireSecondary();
    }

    void GenericCameraController(Camera& camera, float speed, bool orbit) {
        auto dt = Clock.GetFrameTimeSeconds();

        if (Game::Bindings.Held(GameAction::FirePrimary))
            camera.Zoom(dt * speed);

        if (Game::Bindings.Held(GameAction::FireSecondary))
            camera.Zoom(dt * -speed);

        if (Input::MouseButtonPressed(Input::MouseButtons::WheelUp))
            camera.ZoomIn();

        if (Input::MouseButtonPressed(Input::MouseButtons::WheelDown))
            camera.ZoomOut();

        if (Game::Bindings.Held(GameAction::Forward))
            camera.MoveForward(dt * speed);

        if (Game::Bindings.Held(GameAction::Reverse))
            camera.MoveBack(dt * speed);

        if (Game::Bindings.Held(GameAction::SlideLeft))
            camera.MoveLeft(dt * speed);

        if (Game::Bindings.Held(GameAction::SlideRight))
            camera.MoveRight(dt * speed);

        if (Game::Bindings.Held(GameAction::SlideDown))
            camera.MoveDown(dt * speed);

        if (Game::Bindings.Held(GameAction::SlideUp))
            camera.MoveUp(dt * speed);

        if (Game::Bindings.Held(GameAction::RollLeft))
            camera.Roll(dt * 2);

        if (Game::Bindings.Held(GameAction::RollRight))
            camera.Roll(dt * -2);

        // Controller inputs
        camera.MoveRight(Game::Bindings.LinearAxis(GameAction::LeftRightAxis) * speed * dt);
        camera.MoveRight(-Game::Bindings.LinearAxis(GameAction::SlideLeft) * speed * dt);
        camera.MoveRight(Game::Bindings.LinearAxis(GameAction::SlideRight) * speed * dt);

        camera.MoveForward(Game::Bindings.LinearAxis(GameAction::ForwardReverseAxis) * speed * dt);
        camera.MoveForward(Game::Bindings.LinearAxis(GameAction::Forward) * speed * dt);
        camera.MoveForward(-Game::Bindings.LinearAxis(GameAction::Reverse) * speed * dt);

        camera.MoveUp(Game::Bindings.LinearAxis(GameAction::UpDownAxis) * speed * dt);
        camera.MoveUp(Game::Bindings.LinearAxis(GameAction::SlideUp) * speed * dt);
        camera.MoveUp(-Game::Bindings.LinearAxis(GameAction::SlideDown) * speed * dt);

        camera.Roll(Game::Bindings.LinearAxis(GameAction::RollAxis) * dt);
        camera.Roll(-Game::Bindings.LinearAxis(GameAction::RollLeft) * dt * 2);
        camera.Roll(-Game::Bindings.LinearAxis(GameAction::RollRight) * dt * 2);

        //auto& mouse = Game::Bindings.GetMouse();
        //float yawInvert = 1;
        //float pitchInvert = 1;

        //for (auto& binding : mouse.GetBinding(GameAction::YawAxis)) {
        //    if (binding.type == BindType::None) continue;
        //    yawInvert = binding.GetInvertSign();
        //}

        //for (auto& binding : mouse.GetBinding(GameAction::PitchAxis)) {
        //    if (binding.type == BindType::None) continue;
        //    pitchInvert = -binding.GetInvertSign();
        //}

        auto& mouse = Game::Bindings.GetMouse();

        // Mouse control settings are separate from regular flight controls
        float invertx = Settings::Inferno.AutomapInvertX ? -1.0f : 1.0f;
        float inverty = Settings::Inferno.AutomapInvertY ? -1.0f : 1.0f;
        auto sensitivity = mouse.sensitivity.automap * 0.005f;
        auto& delta = Input::MouseDelta;
        float linearSensitivity = 3.0f; // controller sensitivity

        if (orbit) {
            // mouse
            camera.Orbit(-delta.x * sensitivity.y * invertx, -delta.y * inverty * sensitivity.x);

            // controller
            camera.Orbit(Game::Bindings.LinearAxis(GameAction::YawAxis) * dt * linearSensitivity,
                         Game::Bindings.LinearAxis(GameAction::PitchAxis) * dt * linearSensitivity);
        }
        else {
            // mouse
            camera.Rotate(delta.x * sensitivity.y * invertx, -delta.y * inverty * sensitivity.x);

            // controller
            camera.Rotate(Game::Bindings.LinearAxis(GameAction::YawAxis) * dt * linearSensitivity,
                          Game::Bindings.LinearAxis(GameAction::PitchAxis) * dt * linearSensitivity);
        }
    }

    void HandleWeaponKeys() {
        if (Game::GetState() != GameState::Game)
            return; // Not in game

        auto& player = Game::Player;

        if (player.IsDead || Game::Level.Objects.empty())
            return; // No player input without focus or while dead

        if (Game::Bindings.Pressed(GameAction::Weapon1))
            player.SelectPrimary(PrimaryWeaponIndex::Laser);

        if (Game::Bindings.Pressed(GameAction::Weapon2))
            player.SelectPrimary(PrimaryWeaponIndex::Vulcan);

        if (Game::Bindings.Pressed(GameAction::Weapon3))
            player.SelectPrimary(PrimaryWeaponIndex::Spreadfire);

        if (Game::Bindings.Pressed(GameAction::Weapon4))
            player.SelectPrimary(PrimaryWeaponIndex::Plasma);

        if (Game::Bindings.Pressed(GameAction::Weapon5))
            player.SelectPrimary(PrimaryWeaponIndex::Fusion);

        if (Game::Bindings.Pressed(GameAction::Weapon6))
            player.SelectSecondary(SecondaryWeaponIndex::Concussion);

        if (Game::Bindings.Pressed(GameAction::Weapon7))
            player.SelectSecondary(SecondaryWeaponIndex::Homing);

        if (Game::Bindings.Pressed(GameAction::Weapon8))
            player.SelectSecondary(SecondaryWeaponIndex::ProximityMine);

        if (Game::Bindings.Pressed(GameAction::Weapon9))
            player.SelectSecondary(SecondaryWeaponIndex::Smart);

        if (Game::Bindings.Pressed(GameAction::Weapon10))
            player.SelectSecondary(SecondaryWeaponIndex::Mega);

        if (Game::Bindings.Pressed(GameAction::FireFlare))
            player.FireFlare();

        if (Game::Bindings.Pressed(GameAction::CycleBomb))
            player.CycleBombs();

        if (Game::Bindings.Pressed(GameAction::CyclePrimary))
            player.CyclePrimary();

        if (Game::Bindings.Pressed(GameAction::CycleSecondary))
            player.CycleSecondary();

        if (Game::Bindings.Pressed(GameAction::DropBomb))
            player.DropBomb();

        if (Game::Bindings.Pressed(GameAction::Headlight))
            player.ToggleHeadlight();
    }

    void HandleFixedUpdateInput(float /*dt*/) {
        bool firePrimary = false;
        bool fireSecondary = false;

        if (Game::GetState() == GameState::Game) {
            firePrimary = Game::Bindings.Held(GameAction::FirePrimary);
            fireSecondary = Game::Bindings.Held(GameAction::FireSecondary);
        }

        Game::Player.UpdateFireState(firePrimary, fireSecondary);
    }

    void RotateObjectMouselook(Object& object, float yaw, float pitch) {
        auto qyaw = Quaternion::CreateFromAxisAngle(object.Rotation.Up(), yaw);
        auto qpitch = Quaternion::CreateFromAxisAngle(object.Rotation.Right(), pitch);

        Matrix rotation = object.Rotation;
        rotation = Matrix::Transform(rotation, qyaw);
        rotation = Matrix::Transform(rotation, qpitch);

        object.Rotation = Matrix3x3(rotation);
    }

    void HandleShipInput(float dt) {
        if (dt <= 0 || Game::Level.Objects.empty())
            return;

        auto& player = Game::Level.Objects[0];
        auto& physics = player.Physics;
        // Reset previous inputs
        physics.Thrust = Vector3::Zero;
        physics.AngularThrust = Vector3::Zero;

        if (!Input::HasFocus || Game::Player.IsDead)
            return; // No player input without focus or while dead

        //auto ht0 = GetHoldTime(true, 0, frameTime);
        //auto ht1 = GetHoldTime(true, 1, frameTime);

        const auto maxAngularThrust = Resources::GameData.PlayerShip.MaxRotationalThrust;
        const auto maxThrust = Resources::GameData.PlayerShip.MaxThrust;
        const auto maxPitch = Settings::Inferno.HalvePitchSpeed ? maxAngularThrust / 2 : maxAngularThrust;

        Vector3 thrust;

        if (Game::Bindings.Held(GameAction::Forward))
            thrust.z += maxThrust;

        if (Game::Bindings.Held(GameAction::Reverse))
            thrust.z -= maxThrust;

        if (Game::Bindings.Held(GameAction::SlideLeft))
            thrust.x -= maxThrust;

        if (Game::Bindings.Held(GameAction::SlideRight))
            thrust.x += maxThrust;

        if (Game::Bindings.Held(GameAction::SlideDown))
            thrust.y -= maxThrust;

        if (Game::Bindings.Held(GameAction::SlideUp))
            thrust.y += maxThrust;

        //auto pitchAxis = Game::Bindings.LinearAxis(GameAction::PitchAxis);
        thrust.x += Game::Bindings.LinearAxis(GameAction::LeftRightAxis) * maxThrust;
        thrust.y += Game::Bindings.LinearAxis(GameAction::UpDownAxis) * maxThrust;
        thrust.z += Game::Bindings.LinearAxis(GameAction::ForwardReverseAxis) * maxThrust;

        physics.AngularThrust.x += Game::Bindings.LinearAxis(GameAction::PitchAxis) * maxPitch * -1;
        physics.AngularThrust.y += Game::Bindings.LinearAxis(GameAction::YawAxis) * maxAngularThrust;
        physics.AngularThrust.z += Game::Bindings.LinearAxis(GameAction::RollAxis) * maxAngularThrust;

        // Check for triggers bound to movements
        thrust.x += Game::Bindings.LinearAxis(GameAction::SlideLeft) * maxThrust;
        thrust.x -= Game::Bindings.LinearAxis(GameAction::SlideRight) * maxThrust;

        thrust.y += Game::Bindings.LinearAxis(GameAction::SlideUp) * maxThrust;
        thrust.y -= Game::Bindings.LinearAxis(GameAction::SlideDown) * maxThrust;

        thrust.z += Game::Bindings.LinearAxis(GameAction::Forward) * maxThrust;
        thrust.z -= Game::Bindings.LinearAxis(GameAction::Reverse) * maxThrust;

        physics.AngularThrust.z += Game::Bindings.LinearAxis(GameAction::RollLeft) * maxAngularThrust;
        physics.AngularThrust.z += Game::Bindings.LinearAxis(GameAction::RollRight) * maxAngularThrust;

        // Afterburner thrust
        bool abActive = Game::Bindings.Held(GameAction::Afterburner);
        float afterburnerThrust = Game::Player.UpdateAfterburner(dt, abActive);
        if (afterburnerThrust > 1)
            thrust.z = maxThrust * afterburnerThrust;

        // Clamp linear thrust
        Vector3 minLinear = { -maxThrust, -maxThrust, -maxThrust };
        Vector3 maxLinear = { maxThrust, maxThrust, afterburnerThrust > 1 ? maxThrust * 2 : maxThrust };
        thrust.Clamp(minLinear, maxLinear);

        physics.Thrust += player.Rotation.Right() * thrust.x;
        physics.Thrust += player.Rotation.Up() * thrust.y;
        physics.Thrust += player.Rotation.Forward() * thrust.z;

        float yaw = 0, pitch = 0;

        if (Settings::Inferno.EnableMouse) {
            auto& mouse = Game::Bindings.GetMouse();
            constexpr auto mouseSensitivityMultiplier = 0.012f; // Sensitivity is saved between 0 and 2, scale it down.

            for (auto& binding : mouse.GetBinding(GameAction::YawAxis)) {
                if (binding.type == BindType::None) continue;

                float value = 0;
                if (binding.id == (int)Input::MouseAxis::MouseX)
                    value = Input::MouseDelta.x;
                if (binding.id == (int)Input::MouseAxis::MouseY)
                    value = Input::MouseDelta.y;

                yaw = value * binding.GetInvertSign() * mouse.sensitivity.rotation.y * mouseSensitivityMultiplier;
            }

            for (auto& binding : mouse.GetBinding(GameAction::PitchAxis)) {
                if (binding.type == BindType::None) continue;

                float value = 0;
                if (binding.id == (int)Input::MouseAxis::MouseX)
                    value = Input::MouseDelta.x;
                if (binding.id == (int)Input::MouseAxis::MouseY)
                    value = Input::MouseDelta.y;

                pitch = value * binding.GetInvertSign() * mouse.sensitivity.rotation.x * mouseSensitivityMultiplier;
            }
        }

        if (Settings::Inferno.UseMouselook) {
            constexpr float mouselookMultiplier = 0.25;
            RotateObjectMouselook(player, yaw * mouselookMultiplier, pitch * mouselookMultiplier);
        }
        else {
            physics.AngularThrust.x += pitch * Game::TICK_RATE / dt;
            physics.AngularThrust.y += yaw * Game::TICK_RATE / dt;
        }

        if (Game::Bindings.Held(GameAction::PitchUp))
            physics.AngularThrust.x -= 1; // pitch

        if (Game::Bindings.Held(GameAction::PitchDown))
            physics.AngularThrust.x += 1; // pitch

        if (Game::Bindings.Held(GameAction::YawLeft))
            physics.AngularThrust.y -= 1;

        if (Game::Bindings.Held(GameAction::YawRight))
            physics.AngularThrust.y += 1;

        // roll
        if (Game::Bindings.Held(GameAction::RollLeft))
            physics.AngularThrust.z -= 1;

        if (Game::Bindings.Held(GameAction::RollRight))
            physics.AngularThrust.z += 1;

        // Clamp angular speeds
        Vector3 maxAngVec(maxPitch, maxAngularThrust, maxAngularThrust);
        physics.AngularThrust.Clamp(-maxAngVec, maxAngVec);
    }

    void HandleInput(float dt) {
        if (!Input::HasFocus) return;

        if (Input::OnKeyPressed(Keys::F1))
            Game::ShowDebugOverlay = !Game::ShowDebugOverlay;

        switch (Game::GetState()) {
            case GameState::Automap:
                if (Game::Bindings.Pressed(GameAction::Automap) || Game::Bindings.Pressed(GameAction::Pause)) {
                    Game::SetState(GameState::Game);
                }

                HandleAutomapInput();
                return;

            case GameState::Briefing:
                if (PsxMovieVisible)
                    HandlePsxMovieInput();
                else
                    HandleBriefingInput();
                return;

            case GameState::PhotoMode:
                if (Input::MenuActions.IsSet(MenuAction::Cancel) || Game::Bindings.Pressed(GameAction::Pause))
                    Game::SetState(GameState::Game); // Photo mode should only be activated from within game

                GenericCameraController(Game::MainCamera, 90);
                break;

            case GameState::Game:
                if (!Game::Player.IsDead) {
                    if (Game::Bindings.Pressed(GameAction::Automap)) {
                        Game::SetState(GameState::Automap);
                    }

                    if (Game::Bindings.Pressed(GameAction::Pause)) {
                        Game::SetState(GameState::PauseMenu);
                    }

                    HandleShipInput(dt);
                    HandleWeaponKeys();
                }
                break;

            case GameState::EscapeSequence:
                if (Game::Bindings.Pressed(GameAction::Pause)) {
                    Sound::StopMusic();
                    StopEscapeSequence();
                }

                break;
        }
    }
}
