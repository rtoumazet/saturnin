// 
// opengl.h
// Saturnin
//
// Copyright (c) 2003-2017 Renaud Toumazet
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
#include <cstdint>
#include <vector>

namespace saturnin {
namespace video {

	class OpenGl {

	public:
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	OpenGl::OpenGl()
		///
		/// \brief	Default constructor.
		///
		/// \author	Runik
		/// \date	20/11/2017
		////////////////////////////////////////////////////////////////////////////////////////////////////

		OpenGl::OpenGl() {};

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	~OpenGl::OpenGl()
		///
		/// \brief	Default constructor.
		///
		/// \author	Runik
		/// \date	20/11/2017
		////////////////////////////////////////////////////////////////////////////////////////////////////

		OpenGl::~OpenGl() {};

		void OpenGl::createFramebuffer();

        uint32_t OpenGl::generateTexture();
        uint32_t OpenGl::generateFramebuffer(const uint32_t texture);
        uint32_t OpenGl::createVertexShader();
        uint32_t OpenGl::createFragmentShader();
        uint32_t OpenGl::createVertexBufferObject(const float vertices[]);
        uint32_t OpenGl::createProgramShader(const uint32_t vertexShader, const uint32_t fragmentShader);
        uint32_t OpenGl::createVertexArrayObject(const uint32_t vertexBufferObject, const float vertices[]);
        void OpenGl::deleteShaders(std::vector<uint32_t> shaders);

        void OpenGl::setupTriangle();
        void OpenGl::drawTriangle();

    private:
        uint32_t programShader_;
        uint32_t vao_;
    };

};
};