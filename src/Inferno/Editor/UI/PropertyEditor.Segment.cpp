#include "pch.h"
#include "PropertyEditor.h"
#include "../Editor.h"

namespace Inferno::Editor {

    inline bool TriggerTypesDropdown(int& value) {
        static const char* TriggerTypeLabels[] = {
            "None", "Open Door", "Close Door", "Matcen", "Exit", "Secret Exit", "Illusion Off",
            "Illusion On", "Unlock Door", "Lock Door", "Open Wall", "Close Wall", "Illusory Wall",
            "Light Off", "Light On"
        };

        bool changed = false;

        if (ImGui::BeginCombo("##triggertype", TriggerTypeLabels[value], ImGuiComboFlags_HeightLarge)) {

            for (int i = 0; i < std::size(TriggerTypeLabels); i++) {
                const bool isSelected = i == value;
                if (ImGui::Selectable(TriggerTypeLabels[i], isSelected)) {
                    value = i;
                    changed = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    bool TriggerTargetsPicker(Trigger& trigger, TriggerID tid) {
        bool changed = false;
        ImGui::ColumnLabel("Targets");
        ImGui::BeginChild("trigger-targets", { -1, 130 }, true);

        static int selectedIndex = 0;

        for (int i = 0; i < trigger.Targets.Count(); i++) {
            auto& target = trigger.Targets[i];
            string targetLabel = fmt::format("{}:{}", (int)target.Segment, (int)target.Side);
            if (ImGui::Selectable(targetLabel.c_str(), selectedIndex == i, ImGuiSelectableFlags_AllowDoubleClick)) {
                selectedIndex = i;
                if (ImGui::IsMouseDoubleClicked(0)) {
                    Editor::Selection.SetSelection(target);
                }
            }
        }

        ImGui::EndChild();

        if (ImGui::Button("Add##TriggerTarget", { 100, 0 })) {
            if (Editor::Marked.Faces.empty())
                ShowWarningMessage(L"Please mark faces to add as targets.");

            for (auto& mark : Editor::Marked.Faces) {
                AddTriggerTarget(Game::Level, tid, mark);
                changed = true;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Remove##TriggerTarget", { 100, 0 })) {
            RemoveTriggerTarget(Game::Level, tid, selectedIndex);
            if (selectedIndex > trigger.Targets.Count()) selectedIndex--;
            changed = true;
        }
        ImGui::NextColumn();
        return changed;
    }

    bool TriggerPropertiesD1(WallID wid) {
        bool changed = false;
        auto wall = Game::Level.TryGetWall(wid);
        DisableControls disable(!wall);

        auto trigger = wall ? Game::Level.TryGetTrigger(wall->Trigger) : nullptr;

        ImGui::AlignTextToFramePadding();
        auto open = ImGui::TreeNodeEx("Trigger", ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::NextColumn();

        if (!trigger) {
            if (ImGui::Button("Add", { 100, 0 }) && wall)
                wall->Trigger = AddTrigger(Game::Level, wid, TriggerType::OpenDoor);
        }
        else {
            if (ImGui::Button("Remove", { 100, 0 }))
                RemoveTrigger(Game::Level, wall->Trigger);
        }

        ImGui::NextColumn();

        if (open) {
            if (trigger && wall) {
                ImGui::ColumnLabel("ID");
                ImGui::Text("%i", wall->Trigger);
                ImGui::NextColumn();

                changed |= TriggerTargetsPicker(*trigger, wall->Trigger);

                ImGui::ColumnLabel("Open door");
                changed |= FlagCheckbox("##No Message", TriggerFlagD1::OpenDoor, trigger->FlagsD1);
                ImGui::NextColumn();

                ImGui::ColumnLabel("Exit");
                changed |= FlagCheckbox("##Exit", TriggerFlagD1::Exit, trigger->FlagsD1);
                ImGui::NextColumn();

                ImGui::ColumnLabel("Secret exit");
                changed |= FlagCheckbox("##Secret exit", TriggerFlagD1::SecretExit, trigger->FlagsD1);
                ImGui::NextColumn();

                ImGui::ColumnLabel("Matcen");
                changed |= FlagCheckbox("##Matcen", TriggerFlagD1::Matcen, trigger->FlagsD1);
                ImGui::NextColumn();

                ImGui::ColumnLabel("Illusion off");
                changed |= FlagCheckbox("##IllusionOff", TriggerFlagD1::IllusionOff, trigger->FlagsD1);
                ImGui::NextColumn();

                ImGui::ColumnLabel("Illusion on");
                changed |= FlagCheckbox("##IllusionOn", TriggerFlagD1::IllusionOn, trigger->FlagsD1);
                ImGui::NextColumn();
            }
            else {
                ImGui::TextDisabled("No trigger");
                ImGui::NextColumn();
                ImGui::NextColumn();
            }

            ImGui::TreePop();
        }

        return changed;
    }

    bool TriggerProperties(WallID wallId) {
        bool changed = false;
        auto wall = Game::Level.TryGetWall(wallId);
        auto tid = Game::Level.GetTriggerID(wallId);
        auto trigger = Game::Level.TryGetTrigger(wallId);
        DisableControls disable(!wall);

        ImGui::AlignTextToFramePadding();
        auto open = ImGui::TreeNodeEx("Trigger", ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::NextColumn();

        {
            // Shift values by 1 to use 0 as "None"
            auto type = trigger ? (int)trigger->Type + 1 : 0;

            ImGui::SetNextItemWidth(-1);
            if (TriggerTypesDropdown(type)) {
                if (type == 0) {
                    RemoveTrigger(Game::Level, tid);
                }
                else {
                    auto tt = TriggerType(type - 1);
                    if (trigger) {
                        trigger->Type = tt;
                    }
                    else {
                        tid = AddTrigger(Game::Level, wallId, tt);
                    }
                }
                changed = true;
            }

            trigger = Game::Level.TryGetTrigger(wallId);
        }

        ImGui::NextColumn();

        if (open) {
            if (trigger) {
                ImGui::ColumnLabel("ID");
                ImGui::Text("%i", tid);
                ImGui::NextColumn();

                changed |= TriggerTargetsPicker(*trigger, tid);

                ImGui::ColumnLabel("No message");
                changed |= FlagCheckbox("##No Message", TriggerFlag::NoMessage, trigger->Flags);
                ImGui::NextColumn();

                ImGui::ColumnLabel("One shot");
                changed |= FlagCheckbox("##One shot", TriggerFlag::OneShot, trigger->Flags);
                ImGui::NextColumn();
            }
            else {
                ImGui::TextDisabled("No trigger");
                ImGui::NextColumn();
                ImGui::NextColumn();
            }

            ImGui::TreePop();
        }

        return changed;
    }

    void FlickeringProperties(Tag tag) {
        auto light = Game::Level.GetFlickeringLight(tag);

        ImGui::AlignTextToFramePadding();
        auto open = ImGui::TreeNodeEx("Flickering light", ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::NextColumn();

        if (!light) {
            DisableControls disable(!CanAddFlickeringLight(Game::Level, tag));
            if (ImGui::Button("Add", { 100, 0 }))
                Commands::AddFlickeringLight();
        }
        else {
            if (ImGui::Button("Remove", { 100, 0 }))
                Commands::RemoveFlickeringLight();
        }

        light = Game::Level.GetFlickeringLight(tag);

        ImGui::NextColumn();

        if (open) {
            if (light) {
                auto delay = light->Delay * 1000;
                auto orig = *light;
                ImGui::ColumnLabel("Delay");

                ImGui::SetNextItemWidth(-1);
                if (ImGui::DragFloat("##Delay", &delay, 10.0f, 10, 1000, "%.0f ms"))
                    light->Delay = delay / 1000;
                ImGui::NextColumn();

                char mask[33]{};
                for (int i = 0; i < 32; i++)
                    mask[31 - i] = (light->Mask >> i) & 0x1 ? '1' : '0';

                ImGui::ColumnLabel("Mask");
                ImGui::SetNextItemWidth(-1);
                if (ImGui::InputTextEx("##Mask", nullptr, mask, 33, { -1, 0 }, 0)) {
                    for (int i = 0; i < 32; i++) {
                        if (mask[31 - i] == '1')
                            light->Mask |= 1 << i;
                        else
                            light->Mask &= ~(1 << i);
                    }
                }

                if (ImGui::Button("Shift Left", { 100, 0 }))
                    light->ShiftLeft();

                ImGui::SameLine(0, 5);
                if (ImGui::Button("Shift Right", { 100, 0 }))
                    light->ShiftRight();

                if (ImGui::Button("Defaults..."))
                    ImGui::OpenPopup("FlickerDefaults");

                ImGui::SetNextWindowSize({ 100, -1 });
                if (ImGui::BeginPopup("FlickerDefaults")) {
                    if (ImGui::Selectable("On")) light->Mask = FlickeringLight::Defaults::On;
                    if (ImGui::Selectable("Off")) light->Mask = 0;
                    if (ImGui::Selectable("Strobe / 4")) light->Mask = FlickeringLight::Defaults::Strobe4;
                    if (ImGui::Selectable("Strobe / 8")) light->Mask = FlickeringLight::Defaults::Strobe8;
                    if (ImGui::Selectable("Flicker")) light->Mask = FlickeringLight::Defaults::Flicker;
                    ImGui::EndPopup();
                }
                ImGui::NextColumn();

                // Update selected faces
                if (orig.Delay != light->Delay || orig.Mask != light->Mask) {
                    for (auto& face : GetSelectedFaces()) {
                        if (auto l = Game::Level.GetFlickeringLight(face)) {
                            if (orig.Delay != light->Delay) l->Delay = light->Delay;
                            if (orig.Mask != light->Mask) l->Mask = light->Mask;
                        }
                    }
                }
            }
            else {
                ImGui::TextDisabled("No light");
                ImGui::NextColumn();
                ImGui::NextColumn();
            }
            ImGui::TreePop();
        }
    }

    bool SegmentTypeDropdown(SegmentType& type) {
        bool changed = false;

        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##segtype", SegmentTypeLabels[(int)type])) {
            for (int i = 0; i < std::size(SegmentTypeLabels); i++) {
                if (i == 2) continue;

                const bool isSelected = (int)type == i;
                if (ImGui::Selectable(SegmentTypeLabels[i], isSelected)) {
                    changed = true;
                    type = (SegmentType)i;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    string GetMatcenRobotLabel(const Matcen& matcen) {
        string label;

        const uint maxRobots = Game::Level.IsDescent1() ? 25 : 64;
        for (uint i = 0; i < maxRobots; i++) {
            bool flagged = i < 32
                ? matcen.Robots & (1 << (i % 32))
                : matcen.Robots2 & (1 << (i % 32));

            if (flagged)
                label += (label.empty() ? "" : ", ") + Resources::GetRobotName(i);
        }

        return label;
    }

    void MatcenProperties(MatcenID id, MatcenEditor& editor) {
        auto matcen = Game::Level.TryGetMatcen(id);
        if (!matcen) {
            ImGui::Text("Matcen data is missing!");
            ImGui::NextColumn();
            ImGui::NextColumn();
            return;
        }

        ImGui::ColumnLabel("Robots");
        auto robotLabel = GetMatcenRobotLabel(*matcen);
        if (!robotLabel.empty())
            ImGui::TextWrapped(robotLabel.c_str());

        if (ImGui::Button("Edit", { 100, 0 })) {
            editor.ID = id;
            editor.Show();
        }

        ImGui::NextColumn();
    }

    bool SideLighting(SegmentSide& side) {
        ImGui::AlignTextToFramePadding();
        auto open = ImGui::TreeNodeEx("Lighting", ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::NextColumn();
        ImGui::NextColumn();

        bool changed = false;

        if (open) {
            {
                // Emission override
                bool overrideChanged = false;
                bool hasOverride = side.LightOverride.has_value();
                auto light = side.LightOverride.value_or(GetLightColor(side));

                if (ImGui::Checkbox("Emission", &hasOverride)) {
                    side.LightOverride = hasOverride ? Option<Color>(light) : std::nullopt;
                    overrideChanged = true;
                }
                ImGui::NextColumn();

                DisableControls disable(!hasOverride);
                ImGui::SetNextItemWidth(-1);
                if (ImGui::ColorEdit3("##customcolor", &light.x, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float)) {
                    side.LightOverride = light;
                    overrideChanged = true;
                }

                if (overrideChanged) {
                    // Also update marked faces
                    for (auto& tag : GetSelectedFaces()) {
                        if (auto marked = Game::Level.TryGetSide(tag))
                            marked->LightOverride = side.LightOverride;
                    }
                }

                ImGui::NextColumn();
            }

            {
                // Radius override
                bool overrideChanged = false;
                bool hasOverride = side.LightRadiusOverride.has_value();
                auto radius = side.LightRadiusOverride.value_or(Settings::Lighting.Radius);

                if (ImGui::Checkbox("Radius", &hasOverride)) {
                    side.LightRadiusOverride = hasOverride ? Option<float>(radius) : std::nullopt;
                    overrideChanged = true;
                }
                ImGui::NextColumn();

                DisableControls disable(!hasOverride);
                ImGui::SetNextItemWidth(-1);
                if (ImGui::SliderFloat("##radius", &radius, 0, 30, "%.1f")) {
                    side.LightRadiusOverride = radius;
                    overrideChanged = true;
                }

                if (overrideChanged) {
                    // Also update marked faces
                    for (auto& tag : GetSelectedFaces()) {
                        if (auto marked = Game::Level.TryGetSide(tag))
                            marked->LightRadiusOverride = side.LightRadiusOverride;
                    }
                }

                ImGui::NextColumn();
            }

            {
                // Light plane override
                bool overrideChanged = false;
                bool hasOverride = side.LightPlaneOverride.has_value();
                auto plane = side.LightPlaneOverride.value_or(Settings::Lighting.LightPlaneTolerance);

                if (ImGui::Checkbox("Light plane", &hasOverride)) {
                    side.LightPlaneOverride = hasOverride ? Option<float>(plane) : std::nullopt;
                    overrideChanged = true;
                }
                ImGui::NextColumn();

                DisableControls disable(!hasOverride);
                ImGui::SetNextItemWidth(-1);
                if (ImGui::SliderFloat("##lightplane", &plane, -0.01f, -1)) {
                    side.LightPlaneOverride = plane;
                    overrideChanged = true;
                }

                if (overrideChanged) {
                    // Also update marked faces
                    for (auto& tag : GetSelectedFaces()) {
                        if (auto marked = Game::Level.TryGetSide(tag))
                            marked->LightPlaneOverride = side.LightPlaneOverride;
                    }
                }

                ImGui::NextColumn();
            }

            auto VertexLightSlider = [&changed, &side](const char* label, int point) {
                if (point == (int)Editor::Selection.Point)
                    ImGui::PushStyleColor(ImGuiCol_Text, { 0, 1, 0, 1 });

                ImGui::Checkbox(label, &side.LockLight[point]);

                if (point == (int)Editor::Selection.Point)
                    ImGui::PopStyleColor();

                ImGui::NextColumn();

                ImGui::SetNextItemWidth(-1);
                DisableControls disable(!side.LockLight[point]);
                if (ImGui::ColorEdit3(fmt::format("##{}", label).c_str(), &side.Light[point].x, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float)) {
                    changed = true;
                }
                ImGui::NextColumn();
            };

            VertexLightSlider("Point 0", 0);
            VertexLightSlider("Point 1", 1);
            VertexLightSlider("Point 2", 2);
            VertexLightSlider("Point 3", 3);

            ImGui::TreePop();
        }

        return changed;
    }

    bool SideUVs(SegmentSide& side) {
        bool changed = false;
        ImGui::AlignTextToFramePadding();
        auto open = ImGui::TreeNodeEx("UVs", ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::NextColumn();
        ImGui::NextColumn();

        if (open) {
            ImGui::ColumnLabel("UV 0");
            ImGui::SetNextItemWidth(-1);
            changed |= ImGui::DragFloat2("##P0", &side.UVs[0].x, 0.01f);
            ImGui::NextColumn();

            ImGui::ColumnLabel("UV 1");
            ImGui::SetNextItemWidth(-1);
            changed |= ImGui::DragFloat2("##P1", &side.UVs[1].x, 0.01f);
            ImGui::NextColumn();

            ImGui::ColumnLabel("UV 2");
            ImGui::SetNextItemWidth(-1);
            changed |= ImGui::DragFloat2("##P2", &side.UVs[2].x, 0.01f);
            ImGui::NextColumn();

            ImGui::ColumnLabel("UV 3");
            ImGui::SetNextItemWidth(-1);
            changed |= ImGui::DragFloat2("##P3", &side.UVs[3].x, 0.01f);
            ImGui::NextColumn();

            ImGui::TreePop();
        }

        return changed;
    }

    bool WallTypeDropdown(const char* label, WallType& value) {
        static const char* WallTypeLabels[] = {
            "None", "Destroyable", "Door", "Illusion", "Fly-Through", "Closed", "Wall Trigger", "Cloaked"
        };

        auto& seg = Game::Level.GetSegment(Editor::Selection.Tag());
        auto wallTypes = Game::Level.IsDescent1() ? 6 : 8;

        bool changed = false;
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo(label, WallTypeLabels[(int)value])) {
            for (int i = 0; i < wallTypes; i++) {
                // Hide non-wall triggers for sides without connections. INVERSE FOR CONNECTIONS
                if (!seg.SideHasConnection(Editor::Selection.Side) &&
                    ((WallType)i != WallType::None && (WallType)i != WallType::WallTrigger))
                    continue;

                const bool isSelected = (uint8)value == i;
                if (ImGui::Selectable(WallTypeLabels[i], isSelected)) {
                    value = (WallType)i;
                    changed = true;
                    Events::LevelChanged(); // Fly-through can affect rendering
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    bool KeyDropdown(WallKey& value) {
        static const char* KeyLabels[] = { "None", "Blue", "Gold", "Red" };
        static const WallKey KeyValues[] = { WallKey::None, WallKey::Blue, WallKey::Gold, WallKey::Red };

        int selection = [&value] {
            if ((int)value & (int)WallKey::Blue) return 1;
            if ((int)value & (int)WallKey::Gold) return 2;
            if ((int)value & (int)WallKey::Red) return 3;
            return 0;
        }();

        bool changed = false;

        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##Key", KeyLabels[(int)selection])) {
            for (int i = 0; i < std::size(KeyLabels); i++) {
                const bool isSelected = selection == i;
                if (ImGui::Selectable(KeyLabels[i], isSelected)) {
                    value = KeyValues[i];
                    changed = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    bool WallClipDropdown(WClipID& id) {
        bool changed = false;

        auto label = std::to_string((int)id);
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##segs", label.c_str(), ImGuiComboFlags_HeightLarge)) {
            for (int i = 0; i < Resources::GameData.WallClips.size(); i++) {
                const bool isSelected = (int)id == i;
                auto itemLabel = std::to_string((int)i);
                auto& clip = Resources::GameData.WallClips[i];
                TexturePreview(clip.Frames[0], { 32, 32 });

                ImGui::SameLine();
                ImGui::AlignTextToFramePadding();
                if (ImGui::Selectable(itemLabel.c_str(), isSelected)) {
                    changed = true;
                    id = (WClipID)i;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    void OnChangeWallClip(const Wall& wall) {
        SetTextureFromWallClip(Game::Level, wall.Tag, wall.Clip);
        if (auto clip = Resources::TryGetWallClip(wall.Clip)) {
            Render::LoadTextureDynamic(clip->Frames[0]);
            Events::LevelChanged();
        }
    }

    bool WallLightDropdown(Option<bool>& value) {
        static const char* Labels[] = { "Default", "No", "Yes" };
        bool changed = false;

        int index = 0;
        if (value) index = *value ? 2 : 1;

        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##wallLightDropdown", Labels[index])) {
            for (int i = 0; i < std::size(Labels); i++) {
                const bool isSelected = i == index;
                if (ImGui::Selectable(Labels[i], isSelected)) {
                    if (i == 0) value.reset();
                    else value = (i == 2);
                    changed = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    // Returns true if any wall properties changed
    bool WallProperties(WallID id) {
        auto wall = Game::Level.TryGetWall(id);
        bool wallChanged = false;

        ImGui::AlignTextToFramePadding();
        auto open = ImGui::TreeNodeEx("Wall type", ImGuiTreeNodeFlags_SpanAvailWidth);
        auto wallType = wall ? wall->Type : WallType::None;
        ImGui::NextColumn();

        if (WallTypeDropdown("##WallType", wallType)) {
            if (!wall && wallType != WallType::None) {
                Commands::AddWallType(wallType);
            }
            else {
                if (wallType == WallType::None) {
                    Commands::RemoveWall();
                }
                else {
                    wall->Type = wallType;
                    wallChanged = true;
                }
            }

            // Wall might have been added or deleted so fetch it again
            wall = Game::Level.TryGetWall(Editor::Selection.Tag());
        }

        ImGui::NextColumn();

        if (open) {
            if (wall) {
                ImGui::ColumnLabel("ID");
                ImGui::Text("%i", id);
                ImGui::NextColumn();

                ImGui::ColumnLabel("Edit both sides");
                ImGui::Checkbox("##bothsides", &Settings::EditBothWallSides);
                ImGui::NextColumn();

                switch (wall->Type) {
                    case WallType::Destroyable:
                        ImGui::ColumnLabel("Clip");
                        if (WallClipDropdown(wall->Clip)) {
                            wallChanged = true;
                            OnChangeWallClip(*wall);
                        }

                        if (auto clip = Resources::TryGetWallClip(wall->Clip))
                            TexturePreview(clip->Frames[0]);
                        ImGui::NextColumn();

                        ImGui::ColumnLabel("Hit points");
                        ImGui::SetNextItemWidth(-1);
                        if (ImGui::InputFloat("##Hit points", &wall->HitPoints, 1, 10, "%.0f"))
                            wallChanged = true;

                        //FlagCheckbox("Destroyed", WallFlag::Blasted, wall.flags); // Same as creating an illusionary wall on the final frame of a destroyable effect
                        ImGui::NextColumn();
                        break;

                    case WallType::Door:
                        ImGui::ColumnLabel("Clip");
                        if (WallClipDropdown(wall->Clip)) {
                            wallChanged = true;
                            OnChangeWallClip(*wall);
                        }

                        if (auto clip = Resources::TryGetWallClip(wall->Clip))
                            TexturePreview(clip->Frames[0]);

                        ImGui::NextColumn();

                        ImGui::ColumnLabel("Key");
                        wallChanged |= KeyDropdown(wall->Keys);
                        ImGui::NextColumn();

                        ImGui::ColumnLabel("Opened");
                        wallChanged |= FlagCheckbox("##Opened", WallFlag::DoorOpened, wall->Flags);
                        ImGui::NextColumn();

                        ImGui::ColumnLabel("Locked");
                        wallChanged |= FlagCheckbox("##Locked", WallFlag::DoorLocked, wall->Flags);
                        ImGui::NextColumn();

                        ImGui::ColumnLabel("Auto Close");
                        wallChanged |= FlagCheckbox("##Auto Close", WallFlag::DoorAuto, wall->Flags);
                        ImGui::NextColumn();

                        ImGui::ColumnLabel("Buddy Proof");
                        wallChanged |= FlagCheckbox("##Buddy Proof", WallFlag::BuddyProof, wall->Flags);
                        ImGui::NextColumn();
                        break;

                    case WallType::Illusion:
                        ImGui::ColumnLabelEx("Off", "Set the wall to start invisible.\nTrigger with 'illusion on' to make visible.");
                        wallChanged |= FlagCheckbox("##Off", WallFlag::IllusionOff, wall->Flags);
                        ImGui::NextColumn();
                        break;

                    case WallType::Cloaked:
                    {
                        ImGui::ColumnLabel("Cloak");
                        auto cloakValue = wall->CloakValue() * 100;
                        ImGui::SetNextItemWidth(-1);
                        if (ImGui::InputFloat("##cloak", &cloakValue, Wall::CloakStep * 110, Wall::CloakStep * 500, "%.0f%%")) {
                            wall->CloakValue(cloakValue / 100);
                            wallChanged = true;
                        }
                        ImGui::NextColumn();

                        break;
                    }
                }

                ImGui::ColumnLabel("Blocks Light");
                if (WallLightDropdown(wall->BlocksLight)) {
                    for (auto& wid : GetSelectedWalls())
                        Game::Level.GetWall(wid).BlocksLight = wall->BlocksLight;
                }
            }
            else {
                ImGui::TextDisabled("No wall");
                ImGui::NextColumn();
            }

            ImGui::NextColumn();
            ImGui::TreePop();
        }

        return wallChanged;
    }

    string TextureFlagToString(TextureFlag flags) {
        if ((int)flags == 0) return "None";

        string str;
        auto AppendFlag = [&](TextureFlag flag, string name) {
            if (bool((ubyte)flags & (ubyte)flag)) {
                if (str.empty()) str = name;
                else str += ", " + name;
            }
        };

        AppendFlag(TextureFlag::Volatile, "Volatile");
        AppendFlag(TextureFlag::Water, "Water");
        AppendFlag(TextureFlag::ForceField, "ForceField");
        AppendFlag(TextureFlag::GoalBlue, "GoalBlue");
        AppendFlag(TextureFlag::GoalRed, "GoalRed");
        AppendFlag(TextureFlag::GoalHoard, "GoalHoard");
        return str;
    }

    void TextureProperties(const char* label, LevelTexID ltid, bool isOverlay) {
        ImGui::AlignTextToFramePadding();
        auto open = ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::NextColumn();

        const auto ti = Resources::TryGetTextureInfo(ltid);

        if (isOverlay && ltid == LevelTexID::Unset) {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("None");
        }
        else if (ti) {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", ti ? ti->Name.c_str() : "None");
        }

        if (isOverlay && ltid > LevelTexID(0)) {
            ImGui::SameLine();
            if (ImGui::SmallButton("Clear"))
                Events::SelectTexture(LevelTexID::None, LevelTexID::Unset);
        }

        ImGui::NextColumn();

        if (open) {
            if (ti) {
                ImGui::ColumnLabel("Level TexID");
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%i", ltid);
                ImGui::NextColumn();

                ImGui::ColumnLabel("TexID");
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%i", ti->ID);
                ImGui::NextColumn();

                //ImGui::ColumnLabel("Size");
                //ImGui::AlignTextToFramePadding();
                //ImGui::Text("%i x %i", ti->Width, ti->Height);
                //ImGui::NextColumn();

                ImGui::ColumnLabel("Average Color");
                ImGui::AlignTextToFramePadding();
                ImGui::ColorButton("##color", { ti->AverageColor.x, ti->AverageColor.y, ti->AverageColor.z, 1 });
                ImGui::NextColumn();

                ImGui::ColumnLabel("Transparent");
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s %s", ti->Transparent ? "Yes" : "No", ti->SuperTransparent ? "(super)" : "");
                ImGui::NextColumn();
            }

            if (auto lti = Resources::TryGetLevelTextureInfo(ltid)) {
                ImGui::ColumnLabel("Lighting");
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%.2f", lti->Lighting);
                ImGui::NextColumn();

                ImGui::ColumnLabel("Effect clip");
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%i", lti->EffectClip);
                ImGui::NextColumn();

                ImGui::ColumnLabel("Damage");
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%.1f", lti->Damage);
                ImGui::NextColumn();

                /*ImGui::ColumnLabel("Volatile");
                auto isVolatile = (bool)(lti->Flags & TextureFlag::Volatile);
                ImGui::Checkbox("Volatile", &isVolatile);
                ImGui::NextColumn();*/

                ImGui::ColumnLabel("Flags");
                ImGui::AlignTextToFramePadding();
                ImGui::Text(TextureFlagToString(lti->Flags).c_str());
                ImGui::NextColumn();
            }

            ImGui::TreePop();
        }

        //if (ImGui::Button("Find usage"))
        //    Selection.SelectByTexture(_texture);

        //if (ImGui::Button("Export"))
        //    ExportBitmap(_texture);
    }

    // Updates the wall connected to this source
    void UpdateOtherWall(Tag source) {
        if (!Settings::EditBothWallSides) return;

        // Update other wall if mode is enabled
        auto wall = Game::Level.TryGetWall(source);
        auto otherSide = Game::Level.GetConnectedSide(source);
        auto otherWall = Game::Level.TryGetWall(otherSide);

        if (wall && otherWall) {
            // Copy relevant values
            otherWall->Clip = wall->Clip;
            otherWall->Type = wall->Type;
            otherWall->HitPoints = wall->HitPoints;
            otherWall->Flags = wall->Flags;
            otherWall->Keys = wall->Keys;
            otherWall->cloak_value = wall->cloak_value;
            OnChangeWallClip(*otherWall);
        }
    }

    void PropertyEditor::SegmentProperties() {
        ImGui::ColumnLabel("Segment");
        if (SegmentDropdown(Selection.Segment))
            Editor::Selection.SetSelection(Selection.Segment);

        ImGui::NextColumn();

        auto [seg, side] = Game::Level.GetSegmentAndSide(Selection.Tag());
        bool changed = false;

        ImGui::ColumnLabel("Segment type");
        auto segType = seg.Type;
        if (SegmentTypeDropdown(segType))
            Commands::SetSegmentType(segType);

        ImGui::NextColumn();

        if (seg.Type == SegmentType::Matcen)
            MatcenProperties(seg.Matcen, _matcenEditor);

        ImGui::Separator();
        ImGui::ColumnLabel("Side");
        SideDropdown(Selection.Side);
        ImGui::NextColumn();

        {
            ImGui::ColumnLabel("Overlay angle");
            static const std::array angles = { "0 deg", "90 deg", "180 deg", "270 deg" };
            auto rotation = std::clamp((int)side.OverlayRotation, 0, 3);
            ImGui::SetNextItemWidth(-1);
            if (ImGui::SliderInt("##overlay", &rotation, 0, 3, angles[rotation])) {
                side.OverlayRotation = (OverlayRotation)std::clamp(rotation, 0, 3);
                for (auto& tag : GetSelectedFaces()) {
                    if (auto markedSide = Game::Level.TryGetSide(tag))
                        markedSide->OverlayRotation = side.OverlayRotation;
                }
                Editor::History.SnapshotLevel("Change overlay angle");
                Events::LevelChanged();
            }
            ImGui::NextColumn();
        }

        if (WallProperties(side.Wall)) {
            UpdateOtherWall(Editor::Selection.Tag());

            // Only snapshot wall changes if it wasn't deleted. Deleting a wall makes its own snapshot
            if (auto wall = Game::Level.TryGetWall(Editor::Selection.Tag())) {
                ChangeMarkedWalls(Game::Level, *wall);
                Editor::History.SnapshotLevel("Change Wall");
            }
        }

        bool triggerChanged = false;
        if (Game::Level.IsDescent1())
            triggerChanged |= TriggerPropertiesD1(side.Wall);
        else
            triggerChanged |= TriggerProperties(side.Wall);

        if (triggerChanged) {
            Editor::History.SnapshotSelection();
            Editor::History.SnapshotLevel("Change Trigger");
        }

        ImGui::Separator();

        if (!Game::Level.IsDescent1()) {
            FlickeringProperties(Selection.Tag());
            ImGui::Separator();
        }

        {
            auto& connection = seg.Connections[(int)Selection.Side];
            DisableControls disableExit(connection > SegID::None);
            ImGui::ColumnLabel("End of exit tunnel");

            bool isExit = connection == SegID::Exit;
            if (ImGui::Checkbox("##endofexit", &isExit)) {
                connection = isExit ? SegID::Exit : SegID::None;
                changed = true;
            }
            ImGui::NextColumn();
        }

        ImGui::Separator();
        TextureProperties("Base Texture", side.TMap, false);
        TextureProperties("Overlay Texture", side.TMap2, true);
        changed |= SideLighting(side);
        changed |= SideUVs(side);

        if (changed) {
            // copy to marked
            Events::LevelChanged();
        }

        //ImGui::AlignTextToFramePadding();
        //ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
        //ImGui::TreeNodeEx("Side", flags, "Side %i", Selection.Side);

        //for (int obj_i = 0; obj_i < 3; obj_i++)
        //    ShowObject("Object", obj_i);

    }
}
