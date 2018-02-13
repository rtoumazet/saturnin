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

#pragma once

#include <cstdint>
#include <vector>

namespace saturnin {
namespace video {

	class OpenGl {

	public:
        //@{
        // Constructors / Destructors
        OpenGl()                           = default;
        OpenGl(const OpenGl&)              = delete;
        OpenGl(OpenGl&&)                   = delete;
        OpenGl& operator=(const OpenGl&) & = delete;
        OpenGl& operator=(OpenGl&&) &      = delete;
        ~OpenGl()                          = default;
        //@}

		uint32_t OpenGl::createFramebuffer();

        uint32_t OpenGl::createVertexShader();
        uint32_t OpenGl::createFragmentShader();
        uint32_t OpenGl::createVertexBufferObject(const float vertices[]);
        uint32_t OpenGl::createProgramShader(const uint32_t vertexShader, const uint32_t fragmentShader);
        uint32_t OpenGl::createVertexArrayObject(const uint32_t vertexBufferObject, const float vertices[]);
        void OpenGl::deleteShaders(std::vector<uint32_t> shaders);

        void OpenGl::setupTriangle();
        void OpenGl::drawTriangle();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn static bool OpenGl::isModernOpenGlCapable();
        ///
        /// \brief  Query if the current video card is capable of rendering modern opengl (ie version
        ///         3.3+).
        ///
        /// \author Runik
        /// \date   24/12/2017
        ///
        /// \return True if the video car can render modern OpenGL .
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static bool OpenGl::isModernOpenGlCapable();

        bool loadPngImage(const std::string& imageFullPath);

    private:
        uint32_t programShader_;
        uint32_t vao_;
    };

};
};