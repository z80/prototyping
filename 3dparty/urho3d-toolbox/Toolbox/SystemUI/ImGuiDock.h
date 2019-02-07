//
// Copyright (c) 2013-2017 Mikulas Florek
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

#pragma once


#include <imgui/imgui.h>
#include <Urho3D/Resource/XMLElement.h>

namespace ImGui
{

enum DockSlot_
{
    Slot_Left,
    Slot_Right,
    Slot_Top,
    Slot_Bottom,
    Slot_Tab,

    Slot_Float,
    Slot_None
};

void ShutdownDock();
void RootDock(const ImVec2& pos, const ImVec2& size);
bool BeginDock(const char* label, bool* opened = nullptr, ImGuiWindowFlags extra_flags = 0, const ImVec2& default_size = ImVec2(-1, -1));
void EndDock();
void SetDockActive();
void SaveDock(Urho3D::XMLElement element);
void LoadDock(Urho3D::XMLElement element);
void SetNextDockPos(const char* targetDockLabel, DockSlot_ pos, ImGuiCond_ condition);
bool IsDockDocked();
bool IsDockActive();
bool IsDockTabHovered();

}
