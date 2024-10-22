//
// opengl_shaders.h
// Saturnin
//
// Copyright (c) 2024 Renaud Toumazet
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
//

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/opengl/opengl.h>
#include <saturnin/src/video/vdp_common.h>

namespace saturnin::video {

auto initializeShaders() -> GraphicShaders;
void deleteShaders(const std::vector<u32>& shaders);

auto createVertexShader(const GraphicShaders& shaders_list, std::string_view shader_name) -> u32;
auto createFragmentShader(const GraphicShaders& shaders_list, std::string_view shader_name) -> u32;
auto createProgramShader(u32 vertex_shader_id, u32 fragment_shader_id) -> u32;
auto getShaderSource(const GraphicShaders& shaders_list, std::string_view name) -> const char*;

void checkShaderCompilation(u32 shader);
void checkProgramCompilation(u32 program);

}; // namespace saturnin::video
