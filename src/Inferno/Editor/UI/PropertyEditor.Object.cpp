#include "pch.h"
#include "PropertyEditor.h"
#include "Game.h"
#include "../Editor.h"

namespace Inferno::Editor {
    const char* GetObjectTypeName(ObjectType type) {
        if (type == ObjectType::None) return "None";

        constexpr std::array objectTypeLabels = {
            "None", // or "Wall"
            "Fireball",
            "Robot",
            "Hostage",
            "Player",
            "Mine", // Or weapon
            "Camera",
            "Powerup",
            "Debris",
            "Reactor",
            "Unused",
            "Clutter",
            "Ghost",
            "Light",
            "Player (Co-op)",
            "Marker"
        };

        if ((uint8)type >= objectTypeLabels.size()) return "Unknown";
        return objectTypeLabels[(int)type];
    }

    string GetObjectName(Object& obj) {
        switch (obj.Type) {
            case ObjectType::Coop: return fmt::format("Coop player {}", obj.ID);
            case ObjectType::Player: return fmt::format("Player {}", obj.ID);
            case ObjectType::Hostage: return "Hostage";
            case ObjectType::Powerup:
            {
                if (auto name = Resources::GetPowerupName(obj.ID))
                    return name.value();
                else
                    return "Unknown powerup";
            }
            case ObjectType::Reactor: return "Reactor";
            case ObjectType::Robot: return Resources::GetRobotName(obj.ID);
            case ObjectType::Weapon: return "Mine";
            case ObjectType::SecretExitReturn: return "Secret exit return";
            default: return "Unknown object";
        }
    }

    bool ContainsDropdown(const char* label, ObjectType& containsType) {
        bool changed = false;

        if (ImGui::BeginCombo(label, GetObjectTypeName(containsType), ImGuiComboFlags_HeightLarge)) {
            static constexpr std::array availableTypes = {
                ObjectType::None,
                ObjectType::Robot,
                ObjectType::Powerup
            };

            for (auto type : availableTypes) {
                const bool isSelected = containsType == type;
                if (ImGui::Selectable(GetObjectTypeName(type), isSelected)) {
                    containsType = type;
                    changed = true;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    bool PowerupDropdown(const char* label, int8& id, VClipID* vclipID = nullptr) {
        auto name = Resources::GetPowerupName(id);
        auto preview = name ? *name : "Unknown";
        auto powerupCount = Game::Level.IsDescent1() ? 26 : Resources::GameData.Powerups.size();
        bool changed = false;

        if (ImGui::BeginCombo(label, preview.c_str(), ImGuiComboFlags_HeightLarge)) {
            for (int8 i = 0; i < powerupCount; i++) {
                const bool isSelected = id == i;
                auto itemName = Resources::GetPowerupName(i);
                if (!itemName) continue;

                if (ImGui::Selectable(itemName->c_str(), isSelected)) {
                    id = i;
                    if (vclipID) {
                        *vclipID = Resources::GameData.Powerups[i].VClip;
                        Render::LoadTextureDynamic(*vclipID);
                        changed = true;
                    }
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    const std::map<AIBehavior, const char*> BehaviorLabels = {
        { AIBehavior::Normal, "Normal" },
        { AIBehavior::Still, "Still" },
        { AIBehavior::RunFrom, "Drop bombs" },
        { AIBehavior::Station, "Station" },
        { AIBehavior::Behind, "Get behind" },
        { AIBehavior::Snipe, "Snipe" },
        { AIBehavior::Follow, "Follow" }
    };

    bool AIBehaviorDropdown(const char* label, AIBehavior& value) {
        bool changed = false;

        if (!BehaviorLabels.contains(value))
            value = AIBehavior::Normal; // hack to prevent crash on invalid objects. This can occur after changing an object to a robot.

        if (ImGui::BeginCombo(label, BehaviorLabels.at(value))) {
            for (auto& [item, text] : BehaviorLabels) {
                const bool isSelected = item == value;
                if (ImGui::Selectable(text, isSelected)) {
                    value = item;
                    changed = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    bool LevelTextureDropdown(const char* label, LevelTexID& current) {
        string currentLabel = current == LevelTexID::None ? "None" : Resources::GetTextureInfo(current).Name;
        bool changed = false;

        if (ImGui::BeginCombo(label, currentLabel.c_str(), ImGuiComboFlags_HeightLarge)) {
            {
                // Prepend the None case
                const bool isSelected = LevelTexID::None == current;
                if (ImGui::Selectable("None", isSelected))
                    current = LevelTexID::None;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            for (auto& lti : Resources::GameData.TexInfo) {
                auto& ti = Resources::GetTextureInfo(lti.ID);
                // Remove animated textures except for the base
                if (ti.Animated && ti.Frame != 0)
                    continue;

                //TexturePreview(lti.ID, { 32, 32 });
                //ImGui::SameLine();

                const bool isSelected = lti.ID == current;
                auto itemLabel = fmt::format("{}: {}", lti.ID, ti.Name);
                if (ImGui::Selectable(itemLabel.c_str(), isSelected)) {
                    current = lti.ID;
                    changed = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    bool RobotDropdown(const char* label, int8& id) {
        bool changed = false;

        auto robotCount = Game::Level.IsDescent1() ? 24 : Resources::GameData.Robots.size();

        if (ImGui::BeginCombo(label, Resources::GetRobotName(id).c_str(), ImGuiComboFlags_HeightLarge)) {
            for (int8 i = 0; i < robotCount; i++) {
                const bool isSelected = id == i;
                if (ImGui::Selectable(Resources::GetRobotName(i).c_str(), isSelected)) {
                    id = i;
                    changed = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    bool RobotProperties(Object& obj) {
        bool changed = false;

        ImGui::ColumnLabel("Robot");
        ImGui::SetNextItemWidth(-1);
        if (RobotDropdown("##Robot", obj.ID)) {
            auto& robot = Resources::GameData.Robots[(int)obj.ID];
            obj.Render.Model.ID = robot.Model;
            Render::LoadModelDynamic(robot.Model);
            changed = true;
        }
        ImGui::NextColumn();

        ImGui::ColumnLabel("Robot ID");
        ImGui::Text("%i", obj.ID);
        ImGui::NextColumn();

        ImGui::ColumnLabel("Behavior");
        ImGui::SetNextItemWidth(-1);
        changed |= AIBehaviorDropdown("##Behavior", obj.Control.AI.Behavior);
        ImGui::NextColumn();

        ImGui::Separator();

        ImGui::ColumnLabel("Contains");
        ImGui::SetNextItemWidth(-1);
        changed |= ContainsDropdown("##Contains", obj.Contains.Type);
        ImGui::NextColumn();

        if (obj.Contains.Type == ObjectType::Robot) {
            ImGui::ColumnLabel("Robot");
            ImGui::SetNextItemWidth(-1);
            changed |= RobotDropdown("##RobotContains", obj.Contains.ID);
            ImGui::NextColumn();
        }
        else if (obj.Contains.Type == ObjectType::Powerup) {
            ImGui::ColumnLabel("Object");
            ImGui::SetNextItemWidth(-1);
            changed |= PowerupDropdown("##ObjectContains", obj.Contains.ID);
            ImGui::NextColumn();
        }

        if (obj.Contains.Type == ObjectType::Robot || obj.Contains.Type == ObjectType::Powerup) {
            auto count = (int)obj.Contains.Count;
            ImGui::ColumnLabel("Count");
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputInt("##Count", &count)) {
                obj.Contains.Count = (int8)std::clamp(count, 0, 100);
                changed = true;
            }
            ImGui::NextColumn();
        }

        return changed;
    }

    bool ReactorModelDropdown(Object& obj) {
        bool changed = false;

        auto idStr = std::to_string(obj.ID);
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##reactor", idStr.c_str())) {
            for (int8 i = 0; i < Resources::GameData.Reactors.size(); i++) {
                const bool isSelected = obj.ID == i;
                auto iStr = std::to_string(i);
                if (ImGui::Selectable(iStr.c_str(), isSelected)) {
                    obj.ID = i;
                    auto& reactor = Resources::GameData.Reactors[(int)obj.ID];
                    obj.Render.Model.ID = reactor.Model;
                    Render::LoadModelDynamic(reactor.Model);
                    changed = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    inline bool ObjectDropdown(ObjID& id) {
        bool changed = false;
        auto label = fmt::format("{}: {}", id, GetObjectName(Game::Level.Objects[(int)id]));

        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##objs", label.c_str(), ImGuiComboFlags_HeightLarge)) {
            for (int i = 0; i < Game::Level.Objects.size(); i++) {
                const bool isSelected = (int)id == i;
                auto itemLabel = fmt::format("{}: {}", i, GetObjectName(Game::Level.Objects[i]));
                if (ImGui::Selectable(itemLabel.c_str(), isSelected)) {
                    changed = true;
                    id = (ObjID)i;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    void PropertyEditor::ObjectProperties() {
        DisableControls disable(!Resources::HasGameData());

        ImGui::ColumnLabel("Object ID");
        if (ObjectDropdown(Selection.Object))
            Editor::Selection.SetSelection(Selection.Object);
        ImGui::NextColumn();

        auto& obj = Game::Level.GetObject(Selection.Object);

        ImGui::ColumnLabel("Segment");
        SegmentDropdown(obj.Segment);
        ImGui::NextColumn();

        ImGui::Separator();

        ImGui::ColumnLabel("Type");
        ImGui::SetNextItemWidth(-1);

        if (obj.Type == ObjectType::SecretExitReturn) {
            ImGui::Text("Secret Exit Return");
        }
        else if (ImGui::BeginCombo("##Type", GetObjectTypeName(obj.Type))) {
            static constexpr std::array availableTypes = {
                ObjectType::Robot,
                ObjectType::Powerup,
                ObjectType::Hostage,
                ObjectType::Player,
                ObjectType::Coop,
                ObjectType::Reactor,
                ObjectType::Weapon
            };

            auto typeCount = Game::Level.IsDescent1() ? availableTypes.size() - 1 : availableTypes.size();
            for (int i = 0; i < typeCount; i++) {
                auto type = availableTypes[i];
                const bool isSelected = obj.Type == type;
                if (ImGui::Selectable(GetObjectTypeName(type), isSelected)) {
                    InitObject(Game::Level, obj, type);
                    Commands::ChangeMarkedObjects();
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::NextColumn();

        switch (obj.Type) {
            case ObjectType::Powerup:
                ImGui::ColumnLabel("Powerup");
                ImGui::SetNextItemWidth(-1);
                if (PowerupDropdown("##Powerup", obj.ID, &obj.Render.VClip.ID))
                    Commands::ChangeMarkedObjects();

                ImGui::NextColumn();
                break;

            case ObjectType::Robot:
                if (RobotProperties(obj))
                    Commands::ChangeMarkedObjects();
                break;

            case ObjectType::Reactor:
                ImGui::ColumnLabel("Model");
                if (ReactorModelDropdown(obj))
                    Editor::History.SnapshotLevel("Change Reactor Model");

                ImGui::NextColumn();
                break;

            case ObjectType::Weapon: // mines
                ImGui::ColumnLabel("Angular velocity");
                ImGui::SetNextItemWidth(-1);
                if (ImGui::SliderFloat3("##angular", &obj.Movement.Physics.AngularVelocity.x, -1.57f, 1.57f, "%.2f"))
                    Commands::ChangeMarkedObjects();

                ImGui::NextColumn();
                break;

            case ObjectType::Player:
            case ObjectType::Coop:
                ImGui::ColumnLabelEx("ID", "Saving the level sets the ID");
                ImGui::Text("%i", obj.ID);
                ImGui::NextColumn();
                break;
        }

        if (obj.Render.Type == RenderType::Polyobj && obj.Type != ObjectType::SecretExitReturn) {
            ImGui::Separator();
            ImGui::ColumnLabel("Texture override");
            ImGui::SetNextItemWidth(-1);
            if (LevelTextureDropdown("##Texture", obj.Render.Model.TextureOverride)) {
                Render::LoadTextureDynamic(obj.Render.Model.TextureOverride);
                Commands::ChangeMarkedObjects();
            }
            TexturePreview(obj.Render.Model.TextureOverride);
            ImGui::NextColumn();

            ImGui::ColumnLabel("Polymodel");
            ImGui::Text("%i", obj.Render.Model.ID);
            ImGui::NextColumn();
        }
    }
}