//
// Copyright (c) 2008-2017 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/IO/Log.h>
#include "SystemUI/SystemUI.h"
#include <Urho3D/Graphics/Light.h>
#include <imgui/imgui_internal.h>
#include <ImGuizmo/ImGuizmo.h>
#include "Gizmo.h"

namespace Urho3D
{

Gizmo::Gizmo(Context* context) : Object(context)
{
    SubscribeToEvent(E_POSTRENDERUPDATE, [&](StringHash, VariantMap&) { RenderDebugInfo(); });
}

Gizmo::~Gizmo()
{
    UnsubscribeFromAllEvents();
}

bool Gizmo::Manipulate(const Camera* camera, Node* node)
{
    Vector<WeakPtr<Node>> nodes;
    nodes.Push(WeakPtr<Node>(node));
    return Manipulate(camera, nodes);
}

bool Gizmo::IsActive() const
{
    return ImGuizmo::IsUsing();
}

bool Gizmo::Manipulate(const Camera* camera, const Vector<WeakPtr<Node>>& nodes)
{
    if (nodes.Empty())
        return false;

    if (!IsActive())
    {
        // Find center point of all nodes
        if (nodes.Size() == 1)
        {
            // Scene itself may not be manipulated as it does nothing
            if (nodes.Front()->GetType() == Scene::GetTypeStatic())
                return false;

            currentOrigin_ = nodes.Front()->GetTransform().ToMatrix4();     // Makes gizmo work in local space too.
        }
        else
        {
            // It is not clear what should be rotation and scale of center point for multiselection, therefore we limit
            // multiselection operations to world space (see above).
            Vector3 center = Vector3::ZERO;
            auto count = 0;
            for (const auto& node: nodes)
            {
                if (node->GetType() == Scene::GetTypeStatic())
                    continue;
                center += node->GetWorldPosition();
                count++;
            }
            center /= count;
            currentOrigin_.SetTranslation(center);
        }
    }

    // Enums are compatible.
    ImGuizmo::OPERATION operation = static_cast<ImGuizmo::OPERATION>(operation_);
    ImGuizmo::MODE mode = ImGuizmo::WORLD;
    // Scaling only works in local space. Multiselections only work in world space.
    if (transformSpace_ == TS_LOCAL)
        mode = ImGuizmo::LOCAL;
    else if (transformSpace_ == TS_WORLD)
        mode = ImGuizmo::WORLD;

    // Scaling is always done in local space even for multiselections.
    if (operation_ == GIZMOOP_SCALE)
        mode = ImGuizmo::LOCAL;
        // Any other operations on multiselections are done in world space.
    else if (nodes.Size() > 1)
        mode = ImGuizmo::WORLD;

    Matrix4 view = camera->GetView().ToMatrix4().Transpose();
    Matrix4 proj = camera->GetProjection().Transpose();
    Matrix4 tran = currentOrigin_.Transpose();
    Matrix4 delta;

    ImGuiIO& io = ImGui::GetIO();

    auto pos = displayPos_;
    auto size = displaySize_;
    if (size.x == 0 && size.y == 0)
        size = io.DisplaySize;
    ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
    ImGuizmo::Manipulate(&view.m00_, &proj.m00_, operation, mode, &tran.m00_, &delta.m00_, nullptr);

    if (IsActive())
    {
        if (!wasActive_)
        {
            // Just started modifying nodes.
            for (const auto& node: nodes)
                initialTransforms_[node] = node->GetTransform();
        }

        wasActive_ = true;
        tran = tran.Transpose();
        delta = delta.Transpose();

        currentOrigin_ = Matrix4(tran);

        for (const auto& node: nodes)
        {
            if (node == nullptr)
            {
                URHO3D_LOGERROR("Gizmo received null pointer of node.");
                continue;
            }

            if (operation_ == GIZMOOP_SCALE)
            {
                // A workaround for ImGuizmo bug where delta matrix returns absolute scale value.
                if (!nodeScaleStart_.Contains(node))
                    nodeScaleStart_[node] = node->GetScale();
                node->SetScale(nodeScaleStart_[node] * delta.Scale());
            }
            else
            {
                // Delta matrix is always in world-space.
                if (operation_ == GIZMOOP_ROTATE)
                    node->RotateAround(currentOrigin_.Translation(), -delta.Rotation(), TS_WORLD);
                else
                    node->Translate(delta.Translation(), TS_WORLD);
            }
        }

        return true;
    }
    else
    {
        if (wasActive_)
        {
            // Just finished modifying nodes.
            using namespace GizmoNodeModified;
            for (const auto& node: nodes)
            {
                if (node.Expired())
                {
                    URHO3D_LOGWARNINGF("Node expired while manipulating it with gizmo.");
                    continue;
                }

                auto it = initialTransforms_.Find(node.Get());
                if (it == initialTransforms_.End())
                {
                    URHO3D_LOGWARNINGF("Gizmo has no record of initial node transform. List of transformed nodes "
                        "changed mid-manipulation?");
                    continue;
                }

                SendEvent(E_GIZMONODEMODIFIED, P_NODE, node.Get(), P_OLDTRANSFORM, it->second_,
                    P_NEWTRANSFORM, node->GetTransform());
            }
        }
        wasActive_ = false;
        initialTransforms_.Clear();
        if (operation_ == GIZMOOP_SCALE && !nodeScaleStart_.Empty())
            nodeScaleStart_.Clear();
    }
    return false;
}

bool Gizmo::ManipulateSelection(const Camera* camera)
{
    return Manipulate(camera, nodeSelection_);
}

void Gizmo::RenderUI()
{
    ui::TextUnformatted("Op:");
    ui::SameLine(60);

    if (ui::RadioButton("Tr", GetOperation() == GIZMOOP_TRANSLATE))
        SetOperation(GIZMOOP_TRANSLATE);
    ui::SameLine();
    if (ui::RadioButton("Rot", GetOperation() == GIZMOOP_ROTATE))
        SetOperation(GIZMOOP_ROTATE);
    ui::SameLine();
    if (ui::RadioButton("Scl", GetOperation() == GIZMOOP_SCALE))
        SetOperation(GIZMOOP_SCALE);

    ui::TextUnformatted("Space:");
    ui::SameLine(60);
    if (ui::RadioButton("World", GetTransformSpace() == TS_WORLD))
        SetTransformSpace(TS_WORLD);
    ui::SameLine();
    if (ui::RadioButton("Local", GetTransformSpace() == TS_LOCAL))
        SetTransformSpace(TS_LOCAL);
}

bool Gizmo::Select(Node* node)
{
    WeakPtr<Node> weakNode(node);
    if (nodeSelection_.Contains(weakNode))
        return false;
    nodeSelection_.Push(weakNode);
    return true;
}

bool Gizmo::Unselect(Node* node)
{
    WeakPtr<Node> weakNode(node);
    if (!nodeSelection_.Contains(weakNode))
        return false;
    nodeSelection_.Remove(weakNode);
    return true;
}

void Gizmo::RenderDebugInfo()
{
    DebugRenderer* debug = nullptr;
    for (auto it = nodeSelection_.Begin(); it != nodeSelection_.End();)
    {
        WeakPtr<Node> node = *it;
        if (node.Expired())
            it = nodeSelection_.Erase(it);
        else
        {
            if (debug == nullptr)
            {
                if (auto scene = node->GetScene())
                    debug = scene->GetComponent<DebugRenderer>();
            }
            if (debug != nullptr)
            {
                for (auto& component: node->GetComponents())
                {
                    if (auto light = dynamic_cast<Light*>(component.Get()))
                        light->DrawDebugGeometry(debug, true);
                    else if (auto drawable = dynamic_cast<Drawable*>(component.Get()))
                        debug->AddBoundingBox(drawable->GetWorldBoundingBox(), Color::WHITE);
                    else
                        component->DrawDebugGeometry(debug, true);
                }
            }
            ++it;
        }
    }
}

void Gizmo::HandleAutoSelection()
{
    if (autoModeCamera_.Null())
        return;

    ManipulateSelection(autoModeCamera_);

    // Discard clicks when interacting with UI
    if (GetSubsystem<UI>()->GetFocusElement() != nullptr)
        return;

    // Discard clicks when interacting with SystemUI
    if (GetSubsystem<SystemUI>()->IsAnyItemActive() || GetSubsystem<SystemUI>()->IsAnyItemHovered())
        return;

    // Discard clicks when gizmo is being manipulated
    if (IsActive())
        return;

    Input* input = GetSubsystem<Input>();
    if (input->GetMouseButtonPress(MOUSEB_LEFT))
    {
        UI* ui = GetSubsystem<UI>();
        IntVector2 pos = ui->GetCursorPosition();
        // Check the cursor is visible and there is no UI element in front of the cursor
        if (!input->IsMouseVisible() || ui->GetElementAt(pos, true))
            return;

        Graphics* graphics = GetSubsystem<Graphics>();
        Scene* cameraScene = autoModeCamera_->GetScene();
        Ray cameraRay = autoModeCamera_->GetScreenRay((float)pos.x_ / graphics->GetWidth(), (float)pos.y_ / graphics->GetHeight());
        // Pick only geometry objects, not eg. zones or lights, only get the first (closest) hit
        PODVector<RayQueryResult> results;
        RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, M_INFINITY, DRAWABLE_GEOMETRY);
        cameraScene->GetComponent<Octree>()->RaycastSingle(query);
        if (results.Size())
        {
            WeakPtr<Node> clickNode(results[0].drawable_->GetNode());
            if (!input->GetKeyDown(KEY_CTRL))
                nodeSelection_.Clear();

            ToggleSelection(clickNode);
        }
    }

    if (input->GetKeyDown(KEY_SHIFT) && input->GetKeyPress(KEY_TAB))
        operation_ = static_cast<GizmoOperation>(((size_t)operation_ + 1) % GIZMOOP_MAX);

    if (input->GetKeyDown(KEY_CTRL) && input->GetKeyPress(KEY_TAB))
    {
        if (transformSpace_ == TS_WORLD)
            transformSpace_ = TS_LOCAL;
        else if (transformSpace_ == TS_LOCAL)
            transformSpace_ = TS_WORLD;
    }
}

void Gizmo::EnableAutoMode(Camera* camera)
{
    if (autoModeCamera_ == camera)
        return;

    if (camera == nullptr)
        UnsubscribeFromEvent(E_UPDATE);
    else
    {
        Scene* scene = camera->GetScene();
        if (scene == nullptr)
        {
            URHO3D_LOGERROR("Camera which does not belong to scene can not be used for gizmo auto selection.");
            return;
        }

        autoModeCamera_ = camera;

        scene->GetOrCreateComponent<DebugRenderer>();
        SubscribeToEvent(E_UPDATE, [&](StringHash, VariantMap&) { HandleAutoSelection(); });
    }
}

void Gizmo::ToggleSelection(Node* node)
{
    if (IsSelected(node))
        Unselect(node);
    else
        Select(node);
}

bool Gizmo::UnselectAll()
{
    if (nodeSelection_.Empty())
        return false;
    nodeSelection_.Clear();
    return true;
}

bool Gizmo::IsSelected(Node* node) const
{
    WeakPtr<Node> pNode(node);
    return nodeSelection_.Contains(pNode);
}

void Gizmo::SetScreenRect(const IntVector2& pos, const IntVector2& size)
{
    displayPos_ = ToImGui(pos);
    displaySize_ = ToImGui(size);
}

void Gizmo::SetScreenRect(const IntRect& rect)
{
    displayPos_ = ToImGui(rect.Min());
    displaySize_.x = rect.Width();
    displaySize_.y = rect.Height();
}

}
