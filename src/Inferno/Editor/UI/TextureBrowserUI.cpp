#include "pch.h"
#include "TextureBrowserUI.h"
#include "../Editor.h"
#include "Graphics/Render.h"
#include "Resources.h"

namespace Inferno::Editor {
    struct TextureFilter {
        short Min, Max;
        FilterGroup Group;
    };

    constexpr FilterGroup ParseFilterGroup(const string& group) {
        switch (String::Hash(group)) {
            case String::Hash("GrayRock"): return FilterGroup::GrayRock;
            case String::Hash("BrownRock"): return FilterGroup::BrownRock;
            case String::Hash("RedRock"): return FilterGroup::RedRock;
            case String::Hash("GreenRock"): return FilterGroup::GreenRock;
            case String::Hash("YellowRock"): return FilterGroup::YellowRock;
            case String::Hash("BlueRock"): return FilterGroup::BlueRock;
            case String::Hash("Ice"): return FilterGroup::Ice;
            case String::Hash("Stones"): return FilterGroup::Stones;
            case String::Hash("Grass"): return FilterGroup::Grass;
            case String::Hash("Sand"): return FilterGroup::Sand;
            case String::Hash("Lava"): return FilterGroup::Lava;
            case String::Hash("Water"): return FilterGroup::Water;
            case String::Hash("Steel"): return FilterGroup::Steel;
            case String::Hash("Concrete"): return FilterGroup::Concrete;
            case String::Hash("Brick"): return FilterGroup::Brick;
            case String::Hash("Tarmac"): return FilterGroup::Tarmac;
            case String::Hash("Wall"): return FilterGroup::Wall;
            case String::Hash("Floor"): return FilterGroup::Floor;
            case String::Hash("Ceiling"): return FilterGroup::Ceiling;
            case String::Hash("Grate"): return FilterGroup::Grate;
            case String::Hash("Fan"): return FilterGroup::Fan;
            case String::Hash("Light"): return FilterGroup::Light;
            case String::Hash("Energy"): return FilterGroup::Energy;
            case String::Hash("Forcefield"): return FilterGroup::Forcefield;
            case String::Hash("Sign"): return FilterGroup::Sign;
            case String::Hash("Switch"): return FilterGroup::Switch;
            case String::Hash("Tech"): return FilterGroup::Tech;
            case String::Hash("Door"): return FilterGroup::Door;
            case String::Hash("Label"): return FilterGroup::Label;
            case String::Hash("Monitor"): return FilterGroup::Monitor;
            case String::Hash("Stripes"): return FilterGroup::Stripes;
            case String::Hash("Moving"): return FilterGroup::Moving;
            default:
                SPDLOG_WARN("Unknown filter group: {}", group);
                return FilterGroup::None;
        };
    }

    List<TextureFilter> ParseFilter(const filesystem::path& path) {
        List<TextureFilter> filters;

        try {
            std::ifstream file(path);
            if (!file) throw Exception("Unable to read filter file");

            string line;
            while (std::getline(file, line)) {
                // format: "0-0 GrayRock|Concrete"
                auto tokens = String::Split(line, ' ');
                if (tokens.size() != 2) {
                    SPDLOG_WARN("Expected two tokens in texture filter line: {}", line);
                    continue;
                }

                auto group = FilterGroup::None;
                for (auto& g : String::Split(tokens[1], '|'))
                    group |= ParseFilterGroup(g);

                auto rangeTokens = String::Split(tokens[0], '-');

                filters.push_back(TextureFilter{
                    .Min = (short)std::stoi(rangeTokens[0]),
                    .Max = (short)std::stoi(rangeTokens[1]),
                    .Group = group
                });
            }
        }
        catch (...) {
            SPDLOG_ERROR(L"Error reading texture filter from `{}`", path.wstring());
        }

        return filters;
    }

    List<TextureFilter> D1Filter, D2Filter;

    Set<LevelTexID> GetInUseBaseTextures(const Level& level) {
        Set<LevelTexID> texIds;

        for (auto& seg : level.Segments) {
            for (auto& sideId : SideIDs) {
                auto& side = seg.GetSide(sideId);
                if (!seg.SideHasConnection(sideId) || seg.SideIsWall(sideId))
                    texIds.insert(side.TMap);

                if (side.HasOverlay())
                    texIds.insert(side.TMap2);
            }
        }

        return texIds;
    }

    List<LevelTexID> FilterLevelTextures(FilterGroup filter, bool showInUse, bool showEverything) {
        Set<LevelTexID> ids;

        if (showEverything) {
            for (int i = 0; i < Resources::GameData.TexInfo.size(); i++) {
                ids.insert(LevelTexID(i));
            }

            return Seq::ofSet(ids);
        }

        for (auto& entry : Game::Level.IsDescent1() ? D1Filter : D2Filter) {
            if (!bool(entry.Group & filter)) continue;

            for (int16 i = entry.Min; i <= entry.Max; i++) {
                LevelTexID id{ i };
                auto& info = Resources::GetTextureInfo(id);
                if (info.Frame == 0) // omit frames of doors
                    ids.insert(id);
            }
        }

        if (showInUse) {
            auto inUse = GetInUseBaseTextures(Game::Level);
            for (auto& x : inUse) ids.insert(x);
        }

        return Seq::ofSet(ids);
    }

    void TextureBrowserUI::UpdateTextureList(FilterGroup filter, bool loadMaterials) {
        if (Settings::Editor.Descent3Mode) {
            Set<TexID> ids;

            if (_showInUse) {
                auto inUse = GetInUseBaseTextures(Game::Level);
                for (LevelTexID tex : inUse) {
                    if ((int)tex >= Render::OUTRAGE_TEX_INDEX)
                        ids.insert((TexID)tex); // treat level ids as tex ids for d3
                    else
                        ids.insert(Resources::LookupTexID(tex));
                }
            }

            for (size_t i = 0; i < Resources::GameTable.Textures.size(); i++) {
                auto& info = Resources::GameTable.Textures[i];

                bool passesFilter =
                    (_filterD3.Mine && HasFlag(info.Flags, Outrage::TextureFlag::Mine)) ||
                    (_filterD3.Terrain && HasFlag(info.Flags, Outrage::TextureFlag::Terrain)) ||
                    (_filterD3.Procedural && HasFlag(info.Flags, Outrage::TextureFlag::Procedural)) ||
                    (_filterD3.Animated && HasFlag(info.Flags, Outrage::TextureFlag::Animated)) ||
                    (_filterD3.Light && HasFlag(info.Flags, Outrage::TextureFlag::Light));

                if (_showEverything || passesFilter) {
                    ids.insert(TexID(i + Render::OUTRAGE_TEX_INDEX));
                }
            }

            auto newTextures = Seq::ofSet(ids);
            _textureIdsD3 = newTextures;
            Render::Materials->LoadOutrageTextures(_textureIdsD3);
            return;
        }

        //SPDLOG_INFO("Updating texture browser");
        auto ids = FilterLevelTextures(filter, _showInUse, _showEverything);
        auto tids = Seq::map(ids, Resources::LookupTexID);
        if (loadMaterials)
            Render::Materials->LoadMaterialsAsync(tids);

        // Update ids immediately. They will display as loading completes.
        _textureIds.clear();
        Seq::append(_textureIds, ids);
        //Seq::insert(Render::Materials->KeepLoaded, tids); // so browser textures don't get discarded after a prune
    }

    TextureBrowserUI::TextureBrowserUI() : WindowBase(Name, &Settings::Editor.Windows.Textures) {
        Events::LevelLoaded += [this] { UpdateTextureList(_filter, true); };
        Events::LevelChanged += [this] { UpdateTextureList(_filter, false); };
        //Events::SelectSegment += [this] { UpdateSelectedTexture(); };

        D1Filter = ParseFilter("d1filter.txt");
        D2Filter = ParseFilter("d2filter.txt");
    }

    void TextureBrowserUI::DrawFilter() {
        constexpr int ColumnWidth = 170;
        float contentWidth = ImGui::GetWindowContentRegionMax().x;
        float availableWidth = ImGui::GetWindowPos().x + contentWidth;
        bool twoColumn = availableWidth >= ColumnWidth * 2 - 20; // + padding

        if (ImGui::Checkbox("Show in use textures", &_showInUse))
            UpdateTextureList(_filter, true);

        if (ImGui::Checkbox("Show everything", &_showEverything))
            UpdateTextureList(_filter, true);

        ImGui::HelpMarker("This includes animation frames and textures\nnot in the normal filters");

        auto& s = _state;
        constexpr auto flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;

        ImGui::Separator();
        ImGui::AlignTextToFramePadding();
        bool isOpen = ImGui::TreeNodeEx("##filters", flags);
        ImGui::SameLine();
        bool allChecked = s.SelectAll();
        if (ImGui::Checkbox("##toggle", &allChecked))
            s.SelectAll(allChecked);
        ImGui::SameLine();
        ImGui::Text("Filters");

        if (isOpen) {
            auto toggleGroupButtons = [&](const char* label, const std::function<void(bool)>& fn,
                                          const std::function<bool()>& getCheckState) {
                ImGui::PushID(label);
                ImGui::AlignTextToFramePadding();
                bool open = ImGui::TreeNodeEx("##label", flags);
                ImGui::SameLine();
                bool checked = getCheckState();
                if (ImGui::Checkbox("##toggle", &checked))
                    fn(checked);
                ImGui::SameLine();
                ImGui::Text(label);
                ImGui::PopID();
                return open;
            };

            if (toggleGroupButtons("Rock", [&s](bool b) { s.SelectRock(b); }, [&s] { return s.SelectRock(); })) {
                ImGui::Checkbox("Gray Rock", &s.GrayRock);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Brown Rock", &s.BrownRock);

                ImGui::Checkbox("Green Rock", &s.GreenRock);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Yellow Rock", &s.YellowRock);

                ImGui::Checkbox("Blue Rock", &s.BlueRock);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Red Rock", &s.RedRock);

                ImGui::TreePop();
            }

            if (toggleGroupButtons("Natural Materials", [&s](bool b) { s.SelectNatural(b); }, [&s] { return s.SelectNatural(); })) {
                ImGui::Checkbox("Ice", &s.Ice);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Stones", &s.Stones);
                ImGui::Checkbox("Grass", &s.Grass);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Sand", &s.Sand);
                ImGui::Checkbox("Lava", &s.Lava);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Water", &s.Water);
                ImGui::TreePop();
            }

            if (toggleGroupButtons("Structural Materials", [&s](bool b) { s.SelectBuilding(b); }, [&s] { return s.SelectBuilding(); })) {
                ImGui::Checkbox("Steel", &s.Steel);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Concrete", &s.Concrete);
                ImGui::Checkbox("Bricks", &s.Brick);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Tarmac", &s.Tarmac);
                ImGui::Checkbox("Walls", &s.Wall);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Floors", &s.Floor);
                ImGui::Checkbox("Ceilings", &s.Ceiling);
                ImGui::TreePop();
            }

            if (toggleGroupButtons("Doors, Fans and Grates", [&s](bool b) { s.SelectMisc(b); }, [&s] { return s.SelectMisc(); })) {
                ImGui::Checkbox("Grates", &s.Grate);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Fans", &s.Fan);
                ImGui::Checkbox("Doors", &s.Door);
                ImGui::TreePop();
            }

            if (toggleGroupButtons("Technical Materials", [&s](bool b) { s.SelectTechnical(b); }, [&s] { return s.SelectTechnical(); })) {
                ImGui::Checkbox("Lights", &s.Light);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Energy", &s.Energy);
                ImGui::Checkbox("Forcefield", &s.ForceField);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Tech", &s.Tech);
                ImGui::Checkbox("Switches", &s.Switches);
                ImGui::TreePop();
            }

            if (toggleGroupButtons("Signs and Monitors", [&s](bool b) { s.SelectSigns(b); }, [&s] { return s.SelectSigns(); })) {
                ImGui::Checkbox("Labels", &s.Labels);
                if (twoColumn) ImGui::SameLine(ColumnWidth, -1);
                ImGui::Checkbox("Monitors", &s.Monitors);
                ImGui::Checkbox("Stripes", &s.Stripes);
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
        ImGui::Separator();

        auto newState = s.GetState();
        if (newState != _filter) {
            _filter = newState;
            UpdateTextureList(_filter, true);
        }
    }

    // Scales a color to be visible
    ImVec4 GetScaledColor(Color color) {
        auto mag = std::max({ color.x, color.y, color.z });
        if (mag > 0.001f)
            color *= 1 / mag;

        return { color.x, color.y, color.z, 1 };
    }

    Outrage::TextureInfo* GetSelectionTextureInfoD3() {
        if (!Settings::Editor.Descent3Mode) return nullptr;

        if (auto seg = Game::Level.TryGetSegment(Editor::Selection.Segment)) {
            auto [tmap1, tmap2] = seg->GetTexturesForSide(Editor::Selection.Side);

            return Seq::tryItem(Resources::GameTable.Textures, (int)tmap1 - Render::OUTRAGE_TEX_INDEX);
        }

        return nullptr;
    }

    static void PreviewRenderTarget(ID3D12GraphicsCommandList *cmd, int width, int height) {
        auto& target = Render::Adapter->GetHdrRenderTarget();
        auto& depthBuffer = Render::Adapter->GetHdrDepthBuffer();
        target.Transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
        target.ClearColor = Settings::Editor.Background;
        cmd->ClearRenderTargetView(target.GetRTV(), target.ClearColor, 0, nullptr);
        cmd->ClearDepthStencilView(depthBuffer.GetDSV(), D3D12_CLEAR_FLAG_DEPTH, depthBuffer.ClearDepth, 0, 0, nullptr);

        auto rtv = target.GetRTV();
        auto dsv = depthBuffer.GetDSV();
        cmd->OMSetRenderTargets(1, &rtv, false, &dsv);

        D3D12_RECT scissor{};
        scissor.right = (LONG)width;
        scissor.bottom = (LONG)height;
        cmd->RSSetScissorRects(1, &scissor);
        D3D12_VIEWPORT viewport{};
        viewport.Width = (float)width;
        viewport.Height = (float)height;
        viewport.MinDepth = D3D12_MIN_DEPTH;
        viewport.MaxDepth = D3D12_MAX_DEPTH;
        cmd->RSSetViewports(1, &viewport);
        Render::Camera.SetViewport(width, height);
        Render::Camera.Position = Vector3(0.0f, 0.0f, 20.0f);
        Render::Camera.Target = Vector3::Zero;
        Render::Camera.Up = Vector3::UnitY;
        //Render::Camera.LookAtPerspective(Settings::Editor.FieldOfView);
        //Render::Camera.LookAtOrthographic();
        Render::Camera.View = DirectX::XMMatrixLookAtLH(Render::Camera.Position, Render::Camera.Target, Render::Camera.Up);
        //Render::Camera.Projection = Matrix::CreateOrthographic(Render::Camera.Viewport.width * 2, Render::Camera.Viewport.height * 2, 0.1f, 200.0f); //Render::Camera.Viewport.minDepth, Render::Camera.Viewport.maxDepth);
        //Render::Camera.Projection = Matrix::CreateOrthographic(1000.0f, 1000.0f, 0.1f, 200.0f); //Render::Camera.Viewport.minDepth, Render::Camera.Viewport.maxDepth);
        Render::Camera.Projection = Matrix::CreateOrthographicOffCenter(0, 64.0f, 64.0f, 0, 0.2f, 200.0f); //(1000.0f, 1000.0f, 0.1f, 200.0f); //Render::Camera.Viewport.minDepth, Render::Camera.Viewport.maxDepth);
        Render::ViewProjection = Render::Camera.ViewProj();
        //Render::CameraFrustum = Render::Camera.GetFrustum();
    }

    static void PreviewRenderTargetDone(ID3D12GraphicsCommandList* cmd, float x, float y, int width, int height, Matrix &proj) {
        Render::Adapter->SceneColorBuffer.Transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        Render::_tempBatch->SetViewport(Render::Adapter->GetScreenViewport());
        Render::_tempBatch->Begin(cmd, DirectX::SpriteSortMode_Immediate); //, proj);
        auto& src = Render::Adapter->GetHdrRenderTarget();
        RECT srcRect{.left = 0, .top = 0, .right = width, .bottom = height};
        //x = 0; y = 0;
        RECT dstRect{.left = (LONG)x, .top = (LONG)y, .right = (LONG)(x + width), .bottom = (LONG)(y + height)};
        Render::_tempBatch->Draw(src.GetSRV(), DirectX::XMUINT2{ (uint32_t)src.GetWidth(), (uint32_t)src.GetHeight() }, DirectX::XMFLOAT2(x, y), &srcRect,
            DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2{}, DirectX::XMFLOAT2(1.0f, 1.0f));
        //Render::_tempBatch->Draw(src.GetSRV(), DirectX::XMUINT2{ (uint32_t)src.GetWidth(), (uint32_t)src.GetHeight() }, dstRect, &srcRect);

        //DirectX::XMFLOAT2{ x, y });
        Render::_tempBatch->End();
    }

    static D3D12_RECT preview_win;

    void PreviewDrawModel(ID3D12GraphicsCommandList* cmd, const Object& object, ModelID modelId, float alpha, TexID texOverride, int width, int height) {
        auto& effect = Render::Effects->Object;
        effect.Apply(cmd);
        auto& model = Resources::GetModel(modelId);
        auto& meshHandle = Render::_meshBuffer->GetHandle(modelId);

        effect.Shader->SetSampler(cmd, Render::GetTextureSampler());
        ObjectShader::Constants constants = {};
        constants.Eye = Render::Camera.Position;

        //auto& seg = Game::Level.GetSegment(object.Segment);
        constants.Colors[0] = Color(1, 1, 1); //Settings::Editor.RenderMode == RenderMode::Shaded ? seg.VolumeLight : Color(1, 1, 1);

        //Matrix transform = object.GetTransform(t);
        Matrix transform = Matrix::Lerp(object.GetLastTransform(), object.GetTransform(), alpha);
        transform.Forward(-transform.Forward()); // flip z axis to correct for LH models

        // Draw model radius (debug)
        //auto facingMatrix = Matrix::CreateBillboard(object.Position(), Camera.Position, Camera.Up);
        //Debug::DrawCircle(object.Radius, facingMatrix, Color(0, 1, 0));

        int submodelIndex = 0;
        for (auto& submodel : model.Submodels) {
            // accumulate the offsets for each submodel
            auto submodelOffset = Vector3::Zero;
            auto* smc = &submodel;
            while (smc->Parent != ROOT_SUBMODEL) {
                submodelOffset += smc->Offset;
                smc = &model.Submodels[smc->Parent];
            }

            auto world = Matrix::CreateTranslation(submodelOffset) * transform;
            constants.World = world;
            constants.Projection = world * Render::ViewProjection;
            //constants.Time = (float)ElapsedTime;
            effect.Shader->SetConstants(cmd, constants);
            //effect.Shader->SetWorldViewProjection(cmd, world * Render::ViewProjection);

            // get the mesh associated with the submodel
            auto& subMesh = meshHandle.Meshes[submodelIndex++];

            // Draw submodel radii (debug)
            //auto submodelFacingMatrix = Matrix::CreateBillboard(Vector3::Transform(submodelOffset, transform), Camera.Position, Camera.Up);
            //Debug::DrawCircle(submodel.Radius, submodelFacingMatrix, { 0.6, 0.6, 1.0, 1.0 });

            for (int i = 0; i < subMesh.size(); i++) {
                auto mesh = subMesh[i];
                if (!mesh) continue;

                TexID tid = texOverride;
                if (texOverride == TexID::None)
                    tid = mesh->Texture; //mesh->EffectClip == EClipID::None ? mesh->Texture : Resources::GetEffectClip(mesh->EffectClip).VClip.GetFrame(Render::ElapsedTime);

                const Material2D& material = tid == TexID::None ? Render::Materials->White() : Render::Materials->Get(tid);
                //effect.Shader->SetDiffuse(cmd, material.Handle());
                effect.Shader->SetMaterial(cmd, material);

                cmd->IASetVertexBuffers(0, 1, &mesh->VertexBuffer);
                cmd->IASetIndexBuffer(&mesh->IndexBuffer);
                cmd->DrawIndexedInstanced(mesh->IndexCount, 1, 0, 0, 0);
            }
        }
    }

    void ObjCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd, void* ctx_void) {
        ID3D12GraphicsCommandList *ctx = (ID3D12GraphicsCommandList*)ctx_void;
        //if (cmd->UserCallback != NULL) { cmd->UserCallback(parent_list, cmd); } else { DrawObject(parent_list->CmdBuffer }
        ImDrawData* draw_data=ImGui::GetDrawData();
        //draw_data->CmdLists[0][0].

        float L = cmd->ClipRect.x, T = cmd->ClipRect.y;
        float W = cmd->ClipRect.z, H = cmd->ClipRect.w;

        auto proj = Matrix::CreatePerspective(W, H, 0.2f, 1000.0f);
        // Setup viewport
        CD3DX12_VIEWPORT vp(L, T, W, H);
        ctx->RSSetViewports(1, &vp);

        auto OldCamPos = Render::Camera.Position;
        auto OldCamTarget = Render::Camera.Target;

        Render::Camera.SetViewport(W, H);
        Render::ViewProjection = Render::Camera.ViewProj();

        //auto modelId = (ModelID)(int)cmd->TextureId;
        //auto& model = Resources::GetModel(modelId);
        auto model = Resources::GetOutrageModel(Resources::GameTable.Generics[cmd->IdxOffset].ModelName);
        float radius = model->Radius;
        Object object{};
        //Inferno::Render::DrawOutrageModel(obj, ctx, 0, false);
        //obj.Position = {cmd->ClipRect.x, cmd->ClipRect.y, 0};
        object.Radius = radius;
        object.Position.z = 100.0f; //model.Radius * 2.0f;

        PreviewRenderTarget(ctx, (int)W, (int)H);

        Render::Camera.Position = Vector3(0.0f, 0.0f, radius * 1.66f);
        //Render::Camera.LookAtOrthographic();  //Perspective(Settings::Editor.FieldOfView);
        Render::Camera.View = DirectX::XMMatrixLookAtLH(Render::Camera.Position, Render::Camera.Target, Render::Camera.Up);
        Render::Camera.Projection = Matrix::CreateOrthographic(radius * 2.0f, radius * 2.0f, 2.0f, 200.0f); //(1000.0f, 1000.0f, 0.1f, 200.0f); //Render::Camera.Viewport.minDepth, Render::Camera.Viewport.maxDepth);
        Render::ViewProjection = Render::Camera.ViewProj();

        //auto modelId = Resources::GameData.PlayerShip.Model;
        //PreviewDrawModel(ctx, object, modelId, 1.0f, TexID::None, (int)W, (int)H);
        Render::DrawOutrageModel(object, ctx, cmd->IdxOffset, false);
        Render::DrawOutrageModel(object, ctx, cmd->IdxOffset, true);

        auto backBuffer = Render::Adapter->GetBackBuffer();
        auto rtv = backBuffer->GetRTV();
        ctx->OMSetRenderTargets(1, &rtv, false, nullptr);
        Render::Effects->UserInterface.Apply(ctx);

        //D3D12_RECT scissor{.left = (LONG)min.x, .top = (LONG)min.y, .right = (LONG)max.x, .bottom = (LONG)max.y};
        //    .right = (LONG)backBuffer->GetWidth(), .bottom = (LONG)backBuffer->GetHeight()};
        ctx->RSSetScissorRects(1, &preview_win);
        D3D12_VIEWPORT viewport{.Width = (float)backBuffer->GetWidth(), .Height = (float)backBuffer->GetHeight()};
        ctx->RSSetViewports(1, &viewport);

        float DL = draw_data->DisplayPos.x;
        float DR = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float DT = draw_data->DisplayPos.y;
        float DB = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        auto dproj = Matrix::CreateOrthographicOffCenter(DL, DR, DB, DT, 0.0, -2.0f);
        Render::Effects->UserInterface.Apply(ctx);
        Render::Shaders->UserInterface.SetWorldViewProjection(ctx, dproj);

        PreviewRenderTargetDone(ctx, L, T, W, H, dproj);

        Render::Camera.Position = OldCamPos;
        Render::Camera.Target = OldCamTarget;

        #if 0
        Render::Debug::BeginFrame();
        //Render::Debug::DrawFacingCircle(object.Position, object.Radius, Color(0, 1, 0));

        auto facingMatrix = Matrix::CreateBillboard(object.Position, Render::Camera.Position, Render::Camera.Up);
        Render::Debug::DrawCircle(object.Radius, facingMatrix, Color(0, 1, 0));

        Render::Debug::EndFrame(ctx);
        #endif

    }

    #if 0
    void ObjCallback2(const ImDrawList* parent_list, const ImDrawCmd* cmd, void* ctx_void) {
        ID3D12GraphicsCommandList *ctx = (ID3D12GraphicsCommandList*)ctx_void;
        //if (cmd->UserCallback != NULL) { cmd->UserCallback(parent_list, cmd); } else { DrawObject(parent_list->CmdBuffer }
        ImDrawData* draw_data=ImGui::GetDrawData();
        //draw_data->CmdLists[0][0].

        float L = cmd->ClipRect.x, T = cmd->ClipRect.y;
        float W = cmd->ClipRect.z, H = cmd->ClipRect.w;
        PreviewRenderTargetDone(ctx, L, T, W, H);
    }
    #endif

    extern const char* GetObjectTypeName(ObjectType type);

    void TextureBrowserUI::Descent3ObjectBrowser() {
        float contentWidth = ImGui::GetWindowContentRegionMax().x;
        float availableWidth = ImGui::GetWindowPos().x + contentWidth;

        //constexpr int ColumnWidth = 170;
        //bool twoColumn = availableWidth >= ColumnWidth * 2 - 20; // + padding

        if (ImGui::Checkbox("Show in use textures", &_showInUse))
            UpdateTextureList(_filter, true);

        if (ImGui::Checkbox("Show everything", &_showEverything))
            UpdateTextureList(_filter, true);

        ImGui::Text("Filter:");

        if (ImGui::Checkbox("Mine", &_filterD3.Mine))
            UpdateTextureList(_filter, true);

        if (ImGui::Checkbox("Lights", &_filterD3.Light))
            UpdateTextureList(_filter, true);

        if (ImGui::Checkbox("Procedural", &_filterD3.Procedural))
            UpdateTextureList(_filter, true);

        if (ImGui::Checkbox("Animated", &_filterD3.Animated))
            UpdateTextureList(_filter, true);

        static ObjectType objType = ObjectType::Robot;
        if (ImGui::BeginCombo("Type", GetObjectTypeName(objType))) {
            static constexpr std::array availableTypes = {
                ObjectType::Robot,
                ObjectType::Powerup,
                ObjectType::Clutter,
                ObjectType::Building
            };
            for (auto type : availableTypes)
                if (ImGui::Selectable(GetObjectTypeName(type), objType == type))
                    objType = type;
            ImGui::EndCombo();
        }

        ImGui::Separator();

        #if 0
        if (auto info = GetSelectionTextureInfoD3()) {
            ImGui::Text(info->Name.c_str());
            const auto& color = info->Color;
            auto scaledColor = GetScaledColor(color);

            ImGui::ColorButton("light", scaledColor, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);
            ImGui::SameLine();
            ImGui::Text("%.2f, %.2f, %.2f", color.x, color.y, color.z);
        }
        else {
            ImGui::Text("No texture selected");
        }
        #else
        auto& obj = Game::Level.GetObject(Selection.Object);
        if (obj.IsGeneric)
            ImGui::Text(Resources::GameTable.Generics[obj.ID].Name.c_str());
        else
            ImGui::Text("No object selected");
        #endif

        ImGui::Separator();

        ImGui::BeginChild("textures");

        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 2, 2 });

        ImVec2 tileSize{};
        switch (Settings::Editor.TexturePreviewSize) {
            case TexturePreviewSize::Small: tileSize = { 48, 48 };
                break;
            case TexturePreviewSize::Large: tileSize = { 96, 96 };
                break;
            default: tileSize = { 72, 72 };
        }

        tileSize.x *= Shell::DpiScale;
        tileSize.y *= Shell::DpiScale;

        constexpr ImVec4 bg = { 0.1f, 0.1f, 0.1f, 1.0f };
        constexpr int borderThickess = 2;

        auto tmap1 = LevelTexID::None, tmap2 = LevelTexID::Unset;
        if (auto seg = Game::Level.TryGetSegment(Editor::Selection.Segment)) {
            std::tie(tmap1, tmap2) = seg->GetTexturesForSide(Editor::Selection.Side);
        }

        #if 0
        for (auto& id : _textureIdsD3) {
            auto ltid = LevelTexID(id);
            auto& material = Render::Materials->Get(id);
            if (!material || material.State != TextureState::Resident)
                continue; // don't show invalid textures (usually TID 910)
            //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2, 2 });
            //ImGui::PushStyleColor(ImGuiCol_BorderShadow, { 1, 0, 0, 1 });

            ImVec4 borderColor = ltid == tmap1 ? ImVec4(1, 1, 1, 0.8f) : ImVec4(1, 1, 1, 0);
#endif

        const auto& winRect = ImGui::GetCurrentWindowRead()->ClipRect;
        preview_win = D3D12_RECT{.left = (LONG)winRect.Min.x, .top = (LONG)winRect.Min.y, .right = (LONG)winRect.Max.x, .bottom = (LONG)winRect.Max.y};

        //auto& list = Resources::GameData.Robots;
        auto& all = Resources::GameTable.Generics;
        List<size_t> list{};
        for (size_t i = 0; i < all.size(); i++)
            if (all[i].Type == objType)
                list.push_back(i);
        uint n = list.size();
        uint i = 0;
        while (i < n) {
            ImVec4 borderColor = ImVec4(1, 1, 1, 0);


            ImGui::PushStyleColor(ImGuiCol_Button, borderColor);

            //auto btnPos = ImGui::GetCursorPos();
            auto btnPos = ImGui::GetCursorScreenPos();
            //ImGui::ImageButton((ImTextureID)material.Pointer(), tileSize, { 0, 0 }, { 1, 1 }, borderThickess, bg);
            ImGui::PushID(i);
            if (ImGui::InvisibleButton("##", tileSize)) {
                auto& obj = Game::Level.GetObject(Selection.Object);
                #if 0
                if (obj.Type == ObjectType::Robot) {
                    obj.ID = i;
                    auto& robot = Resources::GameData.Robots[(int)obj.ID];
                    obj.Render.Model.ID = robot.Model;
                    obj.Radius = GetObjectRadius(obj);
                    obj.Movement.Physics.Mass = robot.Mass;
                    obj.Movement.Physics.Drag = robot.Drag;

                    Render::LoadModelDynamic(robot.Model);
                    ForMarkedObjects([&obj](Object& o) {
                        if (o.Type != obj.Type) return;
                        o.ID = obj.ID;
                        o.Render.Model.ID = obj.Render.Model.ID;
                        o.Radius = obj.Radius;
                        o.Movement.Physics.Mass = obj.Movement.Physics.Mass;
                        o.Movement.Physics.Drag = obj.Movement.Physics.Drag;
                    });
                }
                #endif
                int id = list[i];
                auto modelName = all[id].ModelName;
                auto model = Resources::GetOutrageModel(modelName);
                obj.IsGeneric = true;
                obj.ID = list[i];
                obj.Type = all[list[i]].Type;
                obj.Radius = model->Radius;
            }
            ImGui::PopID();
            ImRect bb(btnPos.x, btnPos.y, btnPos.x + tileSize.x, btnPos.y + tileSize.y);
            if (bb.Overlaps(ImGui::GetCurrentWindow()->ClipRect)) {
                //if (i == 2)
                {
                    int id = list[i];
                    auto modelName = all[id].ModelName;
                    auto model = Resources::GetOutrageModel(modelName);
                    Render::_meshBuffer->LoadOutrageModel(*model, id);
                    Render::Materials->LoadOutrageTextures(model->TextureHandles);
                    //if (!Render::_meshBuffer->GetHandle(list[i].Model).Loaded)
                    //    Render::LoadModelDynamic(list[i].Model);
                    ImGui::GetCurrentWindow()->DrawList->AddCallback(ObjCallback, nullptr);
                    auto drawList = ImGui::GetCurrentWindow()->DrawList;
                    auto& data = drawList->CmdBuffer.Data[drawList->CmdBuffer.Size - 2];
                    data.ClipRect.x = btnPos.x;
                    data.ClipRect.y = btnPos.y;
                    data.ClipRect.z = tileSize.x;
                    data.ClipRect.w = tileSize.y;
                    data.IdxOffset = id;
                    //data.TextureId = (ImTextureID)list[i].Model;
                }

                ImGui::GetCurrentWindow()->DrawList->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
                #if 0
                {
                    ImGui::GetCurrentWindow()->DrawList->AddCallback(ObjCallback2, nullptr);
                    auto drawList = ImGui::GetCurrentWindow()->DrawList;
                    auto& data = drawList->CmdBuffer.Data[drawList->CmdBuffer.Size - 2];
                    data.ClipRect.x = btnPos.x;
                    data.ClipRect.y = btnPos.y;
                    data.ClipRect.z = tileSize.x;
                    data.ClipRect.w = tileSize.y;
                }
                #endif
            }

#if 0
            if (ImGui::IsItemHovered()) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    tmap1 = ltid;
                    Events::SelectTexture(tmap1, LevelTexID::None);
                    Events::TextureInfo(tmap1);
                    Render::LoadTextureDynamic(tmap1);
                }
                else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {}
                else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {}
            }
#endif

            ImGui::PopStyleColor();
            //ImGui::PopStyleVar();

#if 0
            if (id >= Render::OUTRAGE_TEXID_START) {
                if (auto entry = Seq::tryItem(Resources::GameTable.Textures, (int)id - Render::OUTRAGE_TEX_INDEX)) {
                    const auto& color = entry->Color;
                    if (color.x > 0 || color.y > 0 || color.z > 0) {
                        auto scaledColor = GetScaledColor(color);

                        ImVec2 bmin = { btnPos.x + 1, btnPos.y + 1 };
                        ImVec2 bmax = { bmin.x + 22 * Shell::DpiScale, bmin.y + 22 * Shell::DpiScale };
                        ImVec2 borderMax = { btnPos.x + 24, btnPos.y + 24 };

                        ImGui::GetCurrentWindow()->DrawList->AddRect(btnPos, borderMax, ImGui::GetColorU32({ 0, 0, 0, 1 }));
                        ImGui::GetCurrentWindow()->DrawList->AddRectFilled(bmin, bmax, ImGui::GetColorU32(scaledColor));
                    }
                }
            }
#endif

            float spacing = style.ItemSpacing.x / 2.0f;
            float xLast = ImGui::GetItemRectMax().x;
            float xNext = xLast + spacing + tileSize.x; // Expected position if next button was on same line
            if (i + 1 < n/*_textureIdsD3.size()*/ && xNext < availableWidth)
                ImGui::SameLine(0, spacing);

            i++;
            //break;
        };

        ImGui::PopStyleVar();
        ImGui::EndChild();
    }

    void TextureBrowserUI::Descent3Browser() {
        float contentWidth = ImGui::GetWindowContentRegionMax().x;
        float availableWidth = ImGui::GetWindowPos().x + contentWidth;

        //constexpr int ColumnWidth = 170;
        //bool twoColumn = availableWidth >= ColumnWidth * 2 - 20; // + padding

        if (ImGui::Checkbox("Show in use textures", &_showInUse))
            UpdateTextureList(_filter, true);

        if (ImGui::Checkbox("Show everything", &_showEverything))
            UpdateTextureList(_filter, true);

        ImGui::Text("Filter:");

        if (ImGui::Checkbox("Mine", &_filterD3.Mine))
            UpdateTextureList(_filter, true);

        if (ImGui::Checkbox("Lights", &_filterD3.Light))
            UpdateTextureList(_filter, true);

        if (ImGui::Checkbox("Procedural", &_filterD3.Procedural))
            UpdateTextureList(_filter, true);

        if (ImGui::Checkbox("Animated", &_filterD3.Animated))
            UpdateTextureList(_filter, true);

        ImGui::Separator();

        if (auto info = GetSelectionTextureInfoD3()) {
            ImGui::Text(info->Name.c_str());
            const auto& color = info->Color;
            auto scaledColor = GetScaledColor(color);

            ImGui::ColorButton("light", scaledColor, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);
            ImGui::SameLine();
            ImGui::Text("%.2f, %.2f, %.2f", color.x, color.y, color.z);
        }
        else {
            ImGui::Text("No texture selected");
        }

        ImGui::Separator();

        ImGui::BeginChild("textures");

        ImGuiStyle& style = ImGui::GetStyle();

        uint i = 0;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 2, 2 });

        ImVec2 tileSize{};
        switch (Settings::Editor.TexturePreviewSize) {
            case TexturePreviewSize::Small: tileSize = { 48, 48 };
                                          break;
            case TexturePreviewSize::Large: tileSize = { 96, 96 };
                                          break;
            default: tileSize = { 64, 64 };
        }

        tileSize.x *= Shell::DpiScale;
        tileSize.y *= Shell::DpiScale;

        constexpr ImVec4 bg = { 0.1f, 0.1f, 0.1f, 1.0f };
        constexpr int borderThickess = 2;

        auto tmap1 = LevelTexID::None, tmap2 = LevelTexID::Unset;
        if (auto seg = Game::Level.TryGetSegment(Editor::Selection.Segment)) {
            std::tie(tmap1, tmap2) = seg->GetTexturesForSide(Editor::Selection.Side);
        }

        for (auto& id : _textureIdsD3) {
            auto ltid = LevelTexID(id);
            auto& material = Render::Materials->Get(id);
            if (!material || material.State != TextureState::Resident)
                continue; // don't show invalid textures (usually TID 910)


            //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2, 2 });
            //ImGui::PushStyleColor(ImGuiCol_BorderShadow, { 1, 0, 0, 1 });

            ImVec4 borderColor = ltid == tmap1 ? ImVec4(1, 1, 1, 0.8f) : ImVec4(1, 1, 1, 0);

            ImGui::PushStyleColor(ImGuiCol_Button, borderColor);

            //auto btnPos = ImGui::GetCursorPos();
            auto btnPos = ImGui::GetCursorScreenPos();
            ImGui::ImageButton((ImTextureID)material.Pointer(), tileSize, { 0, 0 }, { 1, 1 }, borderThickess, bg);

            if (ImGui::IsItemHovered()) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    tmap1 = ltid;
                    Events::SelectTexture(tmap1, LevelTexID::None);
                    Events::TextureInfo(tmap1);
                    Render::LoadTextureDynamic(tmap1);
                }
                else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {}
                else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {}
            }

            ImGui::PopStyleColor();
            //ImGui::PopStyleVar();

            if (id >= Render::OUTRAGE_TEXID_START) {
                if (auto entry = Seq::tryItem(Resources::GameTable.Textures, (int)id - Render::OUTRAGE_TEX_INDEX)) {
                    const auto& color = entry->Color;
                    if (color.x > 0 || color.y > 0 || color.z > 0) {
                        auto scaledColor = GetScaledColor(color);

                        ImVec2 bmin = { btnPos.x + 1, btnPos.y + 1 };
                        ImVec2 bmax = { bmin.x + 22 * Shell::DpiScale, bmin.y + 22 * Shell::DpiScale };
                        ImVec2 borderMax = { btnPos.x + 24, btnPos.y + 24 };

                        ImGui::GetCurrentWindow()->DrawList->AddRect(btnPos, borderMax, ImGui::GetColorU32({ 0, 0, 0, 1 }));
                        ImGui::GetCurrentWindow()->DrawList->AddRectFilled(bmin, bmax, ImGui::GetColorU32(scaledColor));
                    }
                }
            }

            float spacing = style.ItemSpacing.x / 2.0f;
            float xLast = ImGui::GetItemRectMax().x;
            float xNext = xLast + spacing + tileSize.x; // Expected position if next button was on same line
            if (i + 1 < _textureIdsD3.size() && xNext < availableWidth)
                ImGui::SameLine(0, spacing);

            i++;
        };

        ImGui::PopStyleVar();
        ImGui::EndChild();
    }

    void TextureBrowserUI::OnUpdate() {
        if (Settings::Editor.Descent3Mode) {
            if (Settings::Editor.SelectionMode == SelectionMode::Object)
                Descent3ObjectBrowser();
            else
                Descent3Browser();
            return;
        }

        float contentWidth = ImGui::GetWindowContentRegionMax().x;
        float availableWidth = ImGui::GetWindowPos().x + contentWidth;

        DrawFilter();

        auto tmap1 = LevelTexID::None, tmap2 = LevelTexID::Unset;
        if (auto seg = Game::Level.TryGetSegment(Editor::Selection.Segment)) {
            std::tie(tmap1, tmap2) = seg->GetTexturesForSide(Editor::Selection.Side);
        }

        {
            auto overlayText = tmap2 <= LevelTexID(0) ? "None" : std::to_string((int)tmap2);
            ImGui::Text("Base: %i Overlay: %s", tmap1, overlayText.c_str());
            if (tmap2 > LevelTexID(0)) {
                ImGui::SameLine();
                if (ImGui::SmallButton("Clear"))
                    Events::SelectTexture(LevelTexID::None, LevelTexID::Unset);
            }
            ImGui::Separator();
        }

        // Don't draw any textures when a new level is loading
        if (Game::IsLoading) return;

        ImGui::BeginChild("textures");

        ImGuiStyle& style = ImGui::GetStyle();
        auto count = (uint)_textureIds.size();
        uint i = 0;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 2, 2 });

        ImVec2 tileSize{};
        switch (Settings::Editor.TexturePreviewSize) {
            case TexturePreviewSize::Small: tileSize = { 48, 48 };
                break;
            case TexturePreviewSize::Large: tileSize = { 96, 96 };
                break;
            default: tileSize = { 64, 64 };
        }

        tileSize.x *= Shell::DpiScale;
        tileSize.y *= Shell::DpiScale;

        constexpr ImVec4 bg = { 0.1f, 0.1f, 0.1f, 1.0f };
        constexpr int borderThickess = 2;

        for (auto& id : _textureIds) {
            auto& material = Render::Materials->Get(id);
            if (!material || material.State != TextureState::Resident)
                continue; // don't show invalid textures (usually TID 910)


            //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2, 2 });
            //ImGui::PushStyleColor(ImGuiCol_BorderShadow, { 1, 0, 0, 1 });

            ImVec4 borderColor =
                id == tmap1 ? ImVec4(1, 1, 1, 0.8f) : (id == tmap2 && tmap2 > LevelTexID(0) ? ImVec4(0, 1, 1, 0.8f) : ImVec4(1, 1, 1, 0));

            ImGui::PushStyleColor(ImGuiCol_Button, borderColor);

            ImGui::ImageButton((ImTextureID)material.Pointer(), tileSize, { 0, 0 }, { 1, 1 }, borderThickess, bg);

            if (ImGui::IsItemHovered()) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    tmap1 = id;
                    Events::SelectTexture(tmap1, LevelTexID::None);
                    Events::TextureInfo(tmap1);
                    Render::LoadTextureDynamic(tmap1);
                }
                else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    tmap2 = id;
                    Events::SelectTexture(LevelTexID::None, tmap2);
                    Events::TextureInfo(tmap2);
                    Render::LoadTextureDynamic(tmap2);
                }
                else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
                    Events::TextureInfo(id);
                }
            }

            ImGui::PopStyleColor();
            //ImGui::PopStyleVar();

            float spacing = style.ItemSpacing.x / 2.0f;
            float xLast = ImGui::GetItemRectMax().x;
            float xNext = xLast + spacing + tileSize.x; // Expected position if next button was on same line
            if (i + 1 < count && xNext < availableWidth)
                ImGui::SameLine(0, spacing);

            i++;
        };

        ImGui::PopStyleVar();
        ImGui::EndChild();
    }
}
