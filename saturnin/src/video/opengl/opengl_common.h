//
// opengl_common.h
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

#include <unordered_map>
#include <saturnin/src/emulator_defs.h> // u8, u16, u32

namespace saturnin::video {

enum class ProgramShader : u8 { main };

struct string_hash {
    using is_transparent = void;
    [[nodiscard]] size_t operator()(const char* txt) const { return std::hash<std::string_view>{}(txt); }
    [[nodiscard]] size_t operator()(std::string_view txt) const { return std::hash<std::string_view>{}(txt); }
    [[nodiscard]] size_t operator()(const std::string& txt) const { return std::hash<std::string>{}(txt); }
};

using GraphicShaders
    = std::unordered_map<std::string, std::string, string_hash, std::equal_to<>>; ///< Shader name + shader content
using ProgramShaders = std::unordered_map<ProgramShader, u32>;                    ///< Program shader name + its OpenGL id.

// Holds the various shaders used in the program
struct Shaders {
    GraphicShaders graphics;
    ProgramShaders programs;
};

} // namespace saturnin::video
