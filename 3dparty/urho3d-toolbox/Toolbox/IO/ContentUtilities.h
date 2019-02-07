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

#pragma once


#include <Urho3D/Container/Str.h>
#include <Urho3D/Precompiled.h>


namespace Urho3D
{

enum FileType
{
    FTYPE_FILE,
    FTYPE_ARCHIVE,
    FTYPE_WORD,
    FTYPE_CODE,
    FTYPE_IMAGE,
    FTYPE_PDF,
    FTYPE_VIDEO,
    FTYPE_POWERPOINT,
    FTYPE_TEXT,
    FTYPE_FILM,
    FTYPE_AUDIO,
    FTYPE_EXCEL,
};

enum ContentType
{
    CTYPE_UNKNOWN,
    CTYPE_SCENE,
    CTYPE_SCENEOBJECT,
    CTYPE_UILAYOUT,
    CTYPE_UISTYLE,
    CTYPE_MODEL,
    CTYPE_ANIMATION,
    CTYPE_MATERIAL,
    CTYPE_PARTICLE,
    CTYPE_RENDERPATH,
    CTYPE_SOUND,
    CTYPE_TEXTURE,
    CTYPE_TEXTUREXML,
};

/// Return file type based on extension of file name.
FileType GetFileType(const String& fileName);
/// Return icon from icon font based on extension of file name.
String GetFileIcon(const String& fileName);

/// Return content type by inspecting file contents.
ContentType GetContentType(const String& resourcePath);

}
