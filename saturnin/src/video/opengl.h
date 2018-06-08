// 
// opengl.h
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
//

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	opengl.h
///
/// \brief	Declares the OpenGl class and related functions. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <vector>

namespace saturnin {
namespace video {

	class Opengl {

	public:
        //@{
        // Constructors / Destructors
        Opengl()                           = default;
        Opengl(const Opengl&)              = delete;
        Opengl(Opengl&&)                   = delete;
        Opengl& operator=(const Opengl&) & = delete;
        Opengl& operator=(Opengl&&) &      = delete;
        ~Opengl()                          = default;
        //@}

		uint32_t create_framebuffer();

        uint32_t create_vertex_shader();
        uint32_t create_fragment_shader();
        uint32_t create_vertex_buffer_object(const float vertices[]);
        uint32_t create_program_shader(const uint32_t vertex_shader, const uint32_t fragment_shader);
        uint32_t create_vertex_array_object(const uint32_t vertex_buffer_object, const float vertices[]);
        void delete_shaders(std::vector<uint32_t> shaders);

        void setup_triangle();
        void draw_triangle();

        bool load_png_image(const std::vector<uint8_t>& source_data, std::vector<uint8_t>& image);
        bool load_icons(std::vector<uint8_t>& image);

    private:
        uint32_t program_shader_;
        uint32_t vao_;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn bool OpenGl::is_modern_opengl_capable();
    ///
    /// \brief  Query if the current video card is capable of rendering modern opengl (ie version
    ///         3.3+).
    ///
    /// \author Runik
    /// \date   24/12/2017
    ///
    /// \return True if the video car can render modern OpenGL .
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool is_modern_opengl_capable();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn int32_t run_legacy_opengl();
    ///
    /// \brief  Executes OpenGL code in legacy mode.
    ///
    /// \author Runik
    /// \date   03/04/2018
    ///
    /// \return An int32_t.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    int32_t run_legacy_opengl();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn int32_t run_modern_opengl();
    ///
    /// \brief  Executes OpenGL code in modern mode (3.3+).
    ///
    /// \author Runik
    /// \date   03/04/2018
    ///
    /// \return An int32_t.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    int32_t run_modern_opengl();
};
};