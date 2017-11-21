#include "opengl.h"
// 
// opengl.cpp
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

#include <iostream> // cout
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

using namespace std;

namespace saturnin {
namespace video {
    
    void OpenGl::createFramebuffer()
    {
        // Framebuffer
        //

    }
    
    uint32_t OpenGl::generateTexture()
    {
        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 320, 200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return texture;
    }
    
    uint32_t OpenGl::generateFramebuffer(const uint32_t texture)
    {
        uint32_t fbo;
        glGenFramebuffers(1, &fbo);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) cout << "Framebuffer error";

        return fbo;
    }

    uint32_t OpenGl::createVertexShader()
    {
        uint32_t vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        const char* vertexShaderSource = R"(
		    #version 330 core
		    layout(location = 0) in vec3 aPos;

		    void main()
		    {
			    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
		    }
        )";

        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        int32_t  success;
        
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        return vertexShader;
    }
    uint32_t OpenGl::createFragmentShader()
    {
        const char* fragmentShaderSource = R"(
        	#version 330 core
        	out vec4 FragColor;

        	void main()
        	{
        	    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        	} 
        )";

        uint32_t fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        return fragmentShader;
    }
    uint32_t OpenGl::createVertexBufferObject(const float vertices[])
    {
        uint32_t vbo;
        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        return vbo;
    }
    
    void OpenGl::deleteShaders(std::vector<uint32_t> shaders)
    {
        for (auto shader : shaders) {
            glDeleteShader(shader);
        }
    }
    bool OpenGl::creatingRenderTarget()
    {
        // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
        GLuint FramebufferName = 0;
        glGenFramebuffers(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        // Now we need to create the texture which will contain the RGB output of our shader.This code is very classic :

        // The texture we're going to render to
        GLuint renderedTexture;
        glGenTextures(1, &renderedTexture);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        // Give an empty image to OpenGL ( the last "0" )
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 320, 200, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        // Poor filtering. Needed !
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Finally, we configure our framebuffer

            // Set "renderedTexture" as our colour attachement #0
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        // Something may have gone wrong during the process, depending on the capabilities of the GPU.This is how you check it :

        // Always check that our framebuffer is ok
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return false;

        return true;

    }
    uint32_t OpenGl::createAndUseProgram(const uint32_t vertexShader, const uint32_t fragmentShader)
    {
        uint32_t shaderProgram;
        shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        GLint success = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[256];
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
        }
        glUseProgram(shaderProgram);

        return shaderProgram;
    }
    uint32_t OpenGl::createVertexArrayObject(const uint32_t vertexBufferObject, const float vertices[])
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        unsigned int VAO;
        glGenVertexArrays(1, &VAO);

        // 1. bind Vertex Array Object
        glBindVertexArray(VAO);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 3. then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        return uint32_t();
    }
};
};