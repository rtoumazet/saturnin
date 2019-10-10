// 
// imgui_loader.h
// Saturnin
//
// Copyright (c) 2018 Renaud Toumazet
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	imgui_loader.h
///
/// \brief	Custom loader for OpenGL functions as used by imgui
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define GLFW_INCLUDE_NONE
//#include <glbinding/gl/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/glbinding.h>

using namespace gl;