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

        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

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
    void OpenGl::setupTriangle()
    {
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
        };

        //uint32_t vbo = createVertexBufferObject(vertices);
        uint32_t vertexShader = createVertexShader();
        uint32_t fragmentShader = createFragmentShader();
        programShader_ = createProgramShader(vertexShader, fragmentShader);
        vector<uint32_t> shadersToDelete = {vertexShader, fragmentShader};
        deleteShaders(shadersToDelete);

        unsigned int VBO;
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

    }
    void OpenGl::drawTriangle()
    {
        glUseProgram(programShader_);
        glBindVertexArray(vao_); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    uint32_t OpenGl::createProgramShader(const uint32_t vertexShader, const uint32_t fragmentShader)
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

        return shaderProgram;
    }
    uint32_t OpenGl::createVertexArrayObject(const uint32_t vertexBufferObject, const float vertices[])
    {
        uint32_t VAO;
        glGenVertexArrays(1, &VAO);

        // 1. bind Vertex Array Object
        glBindVertexArray(VAO);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 3. then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        return VAO;
    }
};
};