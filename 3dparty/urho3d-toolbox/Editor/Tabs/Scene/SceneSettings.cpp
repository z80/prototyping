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

#include "SceneSettings.h"
#include "SceneTab.h"
#include "Editor/EditorEvents.h"


namespace Urho3D
{

SceneSettings::SceneSettings(Context* context)
    : Serializable(context)
{
}

void SceneSettings::SaveProject(XMLElement scene)
{
    auto settings = scene.CreateChild("settings");
    settings.CreateChild("saveElapsedTime").SetVariant(saveElapsedTime_);
}

void SceneSettings::LoadProject(XMLElement scene)
{
    if (auto saveElapsedTime = scene.GetChild("saveElapsedTime"))
        saveElapsedTime_ = saveElapsedTime.GetVariant().GetBool();
}

void SceneSettings::RegisterObject(Context* context)
{
    context->RegisterFactory<SceneSettings>();
    URHO3D_ATTRIBUTE("Save Elapsed Time", bool, saveElapsedTime_, false, AM_EDIT);
}

SceneEffects::SceneEffects(SceneTab* tab)
    : Serializable(tab->GetContext())
    , tab_(tab)
{
    Prepare();
}

void SceneEffects::Prepare(bool force)
{
    if (!force && !rebuild_)
        return;

    // A "fake" context value which allows us to use Urho3D attribute definition macros while storing attributes
    // locally.
    auto context = this;
    attributes_.Clear();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // Update RenderPaths
    {
        renderPaths_.Clear();
        renderPathsEnumNames_.Clear();
        auto defaultRenderPathIndex = -1;
        for (const auto& dir: cache->GetResourceDirs())
        {
            Vector<String> renderPaths;
            String resourcePath = "RenderPaths/";
            String scanDir = AddTrailingSlash(dir) + resourcePath;
            fileSystem->ScanDir(renderPaths, scanDir, "*.xml", SCAN_FILES, false);
            renderPaths_.Push(renderPaths);
        }

        Sort(renderPaths_.Begin(), renderPaths_.End());
        auto index = 0;
        const String defaultRenderPath = "Forward.xml";
        for (const auto& pathName: renderPaths_)
        {
            // Select default renderpath. Engine does not store renderpath name so we have to cache selected renderpath
            // index here. In case index is not set - select default renderpath which is Forward.xml. Changing default
            // renderpath in engine will require patching editor.
            if (pathName == defaultRenderPath)
            {
                if (currentRenderPath_ < 0)
                    currentRenderPath_ = index;
                defaultRenderPathIndex = index;
            }
            renderPathsEnumNames_.Push(pathName.CString());
            index++;
        }
        renderPathsEnumNames_.Push(nullptr);
        if (defaultRenderPathIndex < 0)
        {
            URHO3D_LOGERRORF("Default RenderPath %s was not found.", defaultRenderPath.CString());
            defaultRenderPathIndex = 0;
        }

        auto getter = [this](const SceneEffects&, Variant& value) {
            value = currentRenderPath_;
        };
        auto setter = [this](SceneEffects&, const Variant& value) {
            // Without this check cache rebuild would re-set renderpath, and that resets all filter settings.
            if (value.GetInt() != currentRenderPath_)
            {
                currentRenderPath_ = value.GetInt();

                // Warning: this is a hack. If we set renderpath here directly then it would reset selected postprocess
                // effects. Instead we change current renderpath index and serialize scene state. Since index is already
                // changed new renderpath will be written to xml. Then we load the save, which sets new renderpath to a
                // viewport and restores postprocess effects. If this class expands you may have to split SaveProject()
                // and LoadProject() and use only relevant subset of those routines here.
                XMLFile file(context_);
                XMLElement root = file.CreateRoot("scene");
                SaveProject(root);
                LoadProject(root);

                rebuild_ = true;
            }
        };
        URHO3D_CUSTOM_ENUM_ATTRIBUTE("RenderPath", getter, setter, &renderPathsEnumNames_.Front(),
            defaultRenderPathIndex, AM_EDIT);
        SetAttribute("RenderPath", currentRenderPath_);
    }

    effects_.Clear();
    for (const auto& dir: cache->GetResourceDirs())
    {
        Vector<String> effects;
        String resourcePath = "PostProcess/";
        String scanDir = AddTrailingSlash(dir) + resourcePath;
        GetSubsystem<FileSystem>()->ScanDir(effects, scanDir, "*.xml", SCAN_FILES, false);

        for (const auto& effectFileName: effects)
        {
            auto effectPath = resourcePath + effectFileName;
            XMLFile* effect = cache->GetResource<XMLFile>(effectPath);

            auto root = effect->GetRoot();
            String tag;
            for (auto command = root.GetChild("command"); command.NotNull(); command = command.GetNext("command"))
            {
                tag = command.GetAttribute("tag");

                if (tag.Empty())
                {
                    URHO3D_LOGWARNING("Invalid PostProcess effect with empty tag");
                    continue;
                }

                bool isNew = effects_.Find(tag) == effects_.End();
                PostProcess* postprocess = &effects_[effectPath];
                if (!postprocess->tags_.Contains(tag))
                    postprocess->tags_.Push(tag);

                for (auto parameter = command.GetChild("parameter"); parameter.NotNull();
                    parameter = parameter.GetNext("parameter"))
                {
                    String name = parameter.GetAttribute("name");
                    String valueString = parameter.GetAttribute("value");

                    if (name.Empty() || valueString.Empty())
                    {
                        URHO3D_LOGWARNINGF("Invalid PostProcess effect tagged as %s", tag.CString());
                        continue;
                    }

                    if (!postprocess->variables_.Contains(name))
                        postprocess->variables_[name] = valueString.Split(' ').Size();
                }
            }
        }
    }

    // Register postprocess effect attributes
    for (auto it = effects_.Begin(); it != effects_.End(); it++)
    {
        Sort(it->second_.tags_.Begin(), it->second_.tags_.End());

        String fullPath = it->first_;
        String title = GetFileName(fullPath);

        bool effectEnabled = false;
        if (it->second_.tags_.Size() == 1)
        {
            String tag = it->second_.tags_.Front();
            auto getter = [this, tag](const SceneEffects&, Variant& value) {
                auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                value = path->IsEnabled(tag);
            };
            auto setter = [this, tag, fullPath, cache](SceneEffects&, const Variant& value) {
                auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                if (!path->IsAdded(tag))
                    path->Append(cache->GetResource<XMLFile>(fullPath));
                path->SetEnabled(tag, value.GetBool());
                rebuild_ = true;
                using namespace EditorSceneEffectsChanged;
                SendEvent(E_EDITORSCENEEFFECTSCHANGED, P_SCENETAB, tab_.Get());
            };
            effectEnabled = tab_->GetSceneView()->GetViewport()->GetRenderPath()->IsEnabled(tag);
            URHO3D_CUSTOM_ATTRIBUTE(title.CString(), getter, setter, bool, false, AM_EDIT);
            SetAttribute(title, effectEnabled);
        }
        else if (it->second_.tags_.Size() > 1)
        {
            auto getter = [this, fullPath](const SceneEffects&, Variant& value) {
                auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                const StringVector& allTags = effects_[fullPath].tags_;
                auto index = 0;
                for (const auto& tag: allTags)
                {
                    index++;
                    if (path->IsEnabled(tag))
                    {
                        value = index;
                        return;
                    }
                }
                value = 0;
            };
            auto setter = [this, fullPath, cache](SceneEffects&, const Variant& value) {
                auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                const StringVector& allTags = effects_[fullPath].tags_;

                for (const auto& tag: allTags)
                    path->SetEnabled(tag, false);

                if (value.GetInt() > 0)
                {
                    String tag = allTags[value.GetInt() - 1];    // Dropdown has extra argument at the start
                    if (!path->IsAdded(tag))
                        path->Append(cache->GetResource<XMLFile>(fullPath));
                    path->SetEnabled(tag, true);
                }
                rebuild_ = true;
                using namespace EditorSceneEffectsChanged;
                SendEvent(E_EDITORSCENEEFFECTSCHANGED, P_SCENETAB, tab_.Get());
            };

            // When one effect has multiple tags we assume that only one of those tags is supposed to be active during
            // runtime. In this case a dummy enum array is constructed and enum attribute is registered. It results in
            // a drop-down list for selecting a tag.
            it->second_.tagEnumNames_.Push("None");
            for (const auto& tagVal: it->second_.tags_)
                it->second_.tagEnumNames_.Push(tagVal.CString());
            it->second_.tagEnumNames_.Push(nullptr);

            int index = 0;
            for (const auto& tag: it->second_.tags_)
            {
                index++;
                if (tab_->GetSceneView()->GetViewport()->GetRenderPath()->IsEnabled(tag))
                {
                    effectEnabled = true;
                    break;
                }
            }
            if (!effectEnabled)            // Select "None"
                index = 0;
            URHO3D_CUSTOM_ENUM_ATTRIBUTE(title.CString(), getter, setter, &it->second_.tagEnumNames_.Front(), 0, AM_EDIT);
            SetAttribute(title, index);
        }

        // Do not show variables for disabled effects
        if (!effectEnabled)
            continue;

        StringVector variableKeys = it->second_.variables_.Keys();
        Sort(variableKeys.Begin(), variableKeys.End());
        for (const auto& name: variableKeys)
        {
            int elementCount = it->second_.variables_[name];
            switch (elementCount)
            {
            case 1:
            {
                auto getter = [this, name](const SceneEffects&, Variant& value) {
                    auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                    value = path->GetShaderParameter(name).GetFloat();
                };
                auto setter = [this, name](const SceneEffects&, const Variant& value) {
                    auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                    path->SetShaderParameter(name, value.GetFloat());
                    using namespace EditorSceneEffectsChanged;
                    SendEvent(E_EDITORSCENEEFFECTSCHANGED, P_SCENETAB, tab_.Get());
                };
                URHO3D_CUSTOM_ATTRIBUTE(name.CString(), getter, setter, float, 0.f, AM_EDIT);
                SetAttribute(name, tab_->GetSceneView()->GetViewport()->GetRenderPath()->GetShaderParameter(name).GetFloat());
                break;
            }
            case 2:
            {
                auto getter = [this, name](const SceneEffects&, Variant& value) {
                    auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                    value = path->GetShaderParameter(name).GetVector2();
                };
                auto setter = [this, name](const SceneEffects&, const Variant& value) {
                    auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                    path->SetShaderParameter(name, value.GetVector2());
                    using namespace EditorSceneEffectsChanged;
                    SendEvent(E_EDITORSCENEEFFECTSCHANGED, P_SCENETAB, tab_.Get());
                };
                URHO3D_CUSTOM_ATTRIBUTE(name.CString(), getter, setter, Vector2, Vector2::ZERO, AM_EDIT);
                SetAttribute(name, tab_->GetSceneView()->GetViewport()->GetRenderPath()->GetShaderParameter(name).GetVector2());
                break;
            }
            case 3:
            {
                auto getter = [this, name](const SceneEffects&, Variant& value) {
                    auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                    value = path->GetShaderParameter(name).GetVector3();
                };
                auto setter = [this, name](const SceneEffects&, const Variant& value) {
                    auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                    path->SetShaderParameter(name, value.GetVector3());
                    using namespace EditorSceneEffectsChanged;
                    SendEvent(E_EDITORSCENEEFFECTSCHANGED, P_SCENETAB, tab_.Get());
                };
                URHO3D_CUSTOM_ATTRIBUTE(name.CString(), getter, setter, Vector3, Vector3::ZERO, AM_EDIT);
                SetAttribute(name, tab_->GetSceneView()->GetViewport()->GetRenderPath()->GetShaderParameter(name).GetVector3());
                break;
            }
            case 4:
            {
                auto getter = [this, name](const SceneEffects&, Variant& value) {
                    auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                    value = path->GetShaderParameter(name).GetVector4();
                };
                auto setter = [this, name](const SceneEffects&, const Variant& value) {
                    auto path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
                    path->SetShaderParameter(name, value.GetVector4());
                    using namespace EditorSceneEffectsChanged;
                    SendEvent(E_EDITORSCENEEFFECTSCHANGED, P_SCENETAB, tab_.Get());
                };
                URHO3D_CUSTOM_ATTRIBUTE(name.CString(), getter, setter, Vector4, Vector4::ZERO, AM_EDIT);
                SetAttribute(name, tab_->GetSceneView()->GetViewport()->GetRenderPath()->GetShaderParameter(name).GetVector4());
                break;
            }
            default:
                URHO3D_LOGERRORF("Unknown postprocess shader parameter kind (%d elements)", elementCount);
                break;
            }
        }
    }
    rebuild_ = false;
}

void SceneEffects::SaveProject(XMLElement scene)
{
    scene.CreateChild("renderpath").SetAttribute("path", "RenderPaths/" + renderPaths_[currentRenderPath_]);

    RenderPath* path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
    for (auto it = effects_.Begin(); it != effects_.End(); it++)
    {
        auto fullPath = it->first_;
        for (const auto& tag: it->second_.tags_)
        {
            if (!path->IsEnabled(tag))
                continue;

            auto postprocess = scene.CreateChild("postprocess");
            postprocess.SetAttribute("tag", tag);
            postprocess.SetAttribute("path", fullPath);

            for (const auto& variable: it->second_.variables_)
            {
                auto var = postprocess.CreateChild(variable.first_);
                var.SetVariant(path->GetShaderParameter(variable.first_));
            }
        }
    }

}

void SceneEffects::LoadProject(XMLElement scene)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    if (auto renderpath = scene.GetChild("renderpath"))
    {
        String path = renderpath.GetAttribute("path");
        String fileName = GetFileNameAndExtension(path);
        currentRenderPath_ = 0;
        for (const auto& name: renderPaths_)
        {
            if (name == fileName)
                break;
            currentRenderPath_++;
        }
        if (currentRenderPath_ >= renderPaths_.Size())
        {
            currentRenderPath_ = -1;
            URHO3D_LOGERRORF("RenderPath %s was not found.", path.CString());
        }
        else
            tab_->GetSceneView()->GetViewport()->SetRenderPath(cache->GetResource<XMLFile>(path));
    }

    RenderPath* path = tab_->GetSceneView()->GetViewport()->GetRenderPath();
    for (auto postprocess = scene.GetChild("postprocess"); postprocess.NotNull();
        postprocess = postprocess.GetNext("postprocess"))
    {
        auto effectPath = postprocess.GetAttribute("path");
        auto tagName = postprocess.GetAttribute("tag");

        if (!path->IsAdded(tagName))
        {
            path->Append(cache->GetResource<XMLFile>(effectPath));
            if (effects_.Contains(effectPath))
            {
                // Some render paths have multiple tags and appending enables them all. Disable all tags
                // in added path, later on only selected tag will be enabled.
                for (const auto& tag: effects_[effectPath].tags_)
                    path->SetEnabled(tag, false);
            }
        }

        path->SetEnabled(tagName, true);

        for (auto child = postprocess.GetChild(); child.NotNull(); child = child.GetNext())
            path->SetShaderParameter(child.GetName(), child.GetVariant());
    }

    using namespace EditorSceneEffectsChanged;
    SendEvent(E_EDITORSCENEEFFECTSCHANGED, P_SCENETAB, tab_.Get());

    rebuild_ = true;
}

const Vector<AttributeInfo>* SceneEffects::GetAttributes() const
{
    return &attributes_;
}

template <class T>
AttributeInfo& SceneEffects::RegisterAttribute(const AttributeInfo& attr)
{
    attributes_.Push(attr);
    return attributes_.Back();
}

}
