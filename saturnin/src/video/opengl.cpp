// 
// opengl.cpp
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

#include <functional> // function
#include <iostream> // cout
#include <lodepng.h>
#include "opengl.h"
#include "gui.h"
#include <imgui.h>
#include "../../lib/imgui/imgui_impl_glfw.h"
#include "../../lib/imgui/imgui_impl_opengl2.h"
#include "../../lib/imgui/imgui_impl_opengl3.h"


#include "../../res/icons.png.inc"

using namespace gl;

namespace saturnin {
namespace video {
    
Opengl::Opengl(core::Config* config) {
    config_ = config;
    bool is_legacy = config->readValue(core::Access_keys::config_legacy_opengl);
    if (is_legacy)  calculateRendering = std::bind(&Opengl::calculateLegacyRendering, this);
    else            calculateRendering = std::bind(&Opengl::calculateModernRendering, this);
    
}

uint32_t Opengl::createFramebuffer()
{
    uint32_t framebuffer{};
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    uint32_t textureColorbuffer;
    gl::glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 320, 200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLenum::GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLenum::GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureColorbuffer, 0);
    gl::GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return framebuffer;
}
    
uint32_t Opengl::createVertexShader()
{
    uint32_t vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const char* vertex_shader_source = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;

		void main()
		{
			gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
		}
    )";

    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    int32_t  success;
        
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char info_log[512];
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
    }

    return vertex_shader;
}
uint32_t Opengl::createFragmentShader()
{
    const char* fragment_shader_source = R"(
        #version 330 core
        out vec4 FragColor;

        void main()
        {
        	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        } 
    )";

    uint32_t fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    return fragment_shader;
}
//uint32_t Opengl::create_vertex_buffer_object(const float vertices[])
//{
//    uint32_t vbo;
//    glGenBuffers(1, &vbo);
//
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    return vbo;
//}
    
void Opengl::deleteShaders(std::vector<uint32_t> shaders)
{
    for (auto shader : shaders) {
        glDeleteShader(shader);
    }
}
void Opengl::setupTriangle()
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    //uint32_t vbo                          = createVertexBufferObject(vertices);
    uint32_t vertex_shader                  = createVertexShader();
    uint32_t fragment_shader                = createFragmentShader();
    program_shader_                         = createProgramShader(vertex_shader, fragment_shader);
    std::vector<uint32_t> shaders_to_delete = {vertex_shader, fragment_shader};
    deleteShaders(shaders_to_delete);

    unsigned int vbo;
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GLenum::GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

}
void Opengl::drawTriangle()
{
    glUseProgram(program_shader_);
    glBindVertexArray(vao_); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

uint32_t Opengl::createProgramShader(const uint32_t vertex_shader, const uint32_t fragment_shader)
{
    uint32_t shader_program;
    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    GLint success = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[256];
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << info_log << std::endl;
    }

    return shader_program;
}
//uint32_t Opengl::createVertexArrayObject(const uint32_t vertex_buffer_object, const float vertices[])
//{
//    uint32_t VAO;
//    glGenVertexArrays(1, &VAO);
//
//    // 1. bind Vertex Array Object
//    glBindVertexArray(VAO);
//    // 2. copy our vertices array in a buffer for Opengl to use
//    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    // 3. then set our vertex attributes pointers
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    return VAO;
//}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void Opengl::preRendering(uint32_t& fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER,  fbo);
    gl::glViewport(0, 0, 320, 200);
    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

int32_t Opengl::bindTextureToFramebuffer() {
    GLint ret{};
    int32_t texture{};
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &ret);
    if (ret != GL_NONE) {
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &texture);
    }
    return texture;
}

int32_t Opengl::calculateLegacyRendering() {
    static float i = 0;
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(i, 0.0f, 0.0f, 1.0f);
    
    glBegin(GL_TRIANGLES);
    glColor4f(1.0f, 0.5f, 0.2f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glEnd();

    glPopMatrix();
    ++i;

    return this->bindTextureToFramebuffer();
}

int32_t Opengl::calculateModernRendering() {
    this->setupTriangle();
    this->drawTriangle();

    return this->bindTextureToFramebuffer();
}

void Opengl::postRendering() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Opengl::loadPngImage(const std::vector<uint8_t>& source_data, std::vector<uint8_t>& image) {
    // Load file and decode image.
    uint32_t width{};
    uint32_t height{};
    uint32_t error = lodepng::decode(image, width, height, source_data, LCT_RGBA);

    // If there's an error, display it.
    if (error != 0) {
        std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    //// "F" texture
    //uint8_t texture[] = {
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 
    //};
    //glEnable(GL_TEXTURE_2D);
    //
    //glGenTextures(1, &iconsTextureId);
    //glBindTexture(GL_TEXTURE_2D, iconsTextureId);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, 0x8, 0x8, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, source_data.data());
    //glObjectLabel(GL_TEXTURE, iconsTextureId, -1, "Example Texture");

    return true;
}

uint32_t Opengl::loadIcons(std::vector<uint8_t>& image) {
    //Opengl.loadPngImage("D:/Dev/Sources/VS2017/saturnin-vs2017/saturnin/res/icons.png");
    //std::vector<uint8_t> icons_vector(icons_png, icons_png + sizeof(icons_png));
    //return loadPngImage(icons_vector, image);
    
        // "F" texture
    uint8_t texture[] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x00,0x00,0x00,
        0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    };
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &iconsTextureId);
    glBindTexture(GL_TEXTURE_2D, iconsTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 0x8, 0x8, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

    return iconsTextureId;
}

void Opengl::generateIconsTexture() {

}

bool isModernOpenglCapable()
{
    if (!glfwInit()) return false;
    else {
        GLFWwindow* window = glfwCreateWindow(1280, 720, "Test", NULL, NULL);
        if (window == nullptr) return false;
        else {
            uint32_t major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
            uint32_t minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);

            if (major == 3) {
                if (minor < 3) return false;
            }
            else if (major < 3) return false;

            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }
    return true;
}

int32_t runLegacyOpengl(core::Emulator_context& state) {
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return EXIT_FAILURE;

    std::string window_title = fmt::format(core::tr("Saturnin {0} - Legacy rendering"), core::saturnin_version);
    auto window = glfwCreateWindow(1280, 720, window_title.c_str(), NULL, NULL);
    if (window == NULL)
        return EXIT_FAILURE;

    glfwSetWindowCloseCallback(window, windowCloseCallback);
    glfwSetWindowUserPointer(window, (void*)&state);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

                            //cout << epoxy_gl_version() << endl;

                            //epoxy_handle_external_wglMakeCurrent();
                            //if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_pbuffer"))
                            //    cout << "WGL_ARB_pbuffer not found !" << endl;

                            //if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_pixel_format"))
                            //    cout << "WGL_ARB_pixel_format not found !" << endl;

                            //if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_render_texture"))
                            //    cout << "WGL_ARB_render_texture not found !" << endl;
                            //
                            //if (!epoxy_has_gl_extension("GL_ARB_framebuffer_object"))
                            //    cout << "GL_ARB_framebuffer_object not found !" << endl;
                            //
                            //if (!epoxy_has_gl_extension("GL_EXT_framebuffer_object"))
                            //    cout << "GL_EXT_framebuffer_object not found !" << endl;
                            
	glbinding::initialize(glfwGetProcAddress);

    Opengl opengl(state.config());
    uint32_t fbo = opengl.createFramebuffer();
    //Opengl.setupTriangle();


    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    // 
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    bool show_test_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(0); // You may want this if using this code in an Opengl 3+ context where shaders may be bound

        gui::buildGui(state, opengl, fbo, display_w, display_h);

        if (state.renderingStatus_ == core::RenderingStatus::reset) glfwSetWindowShouldClose(window, true);

        gui::show_test_window(show_test_window);


        ImGui::Render();
            
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

int32_t runModernOpengl(core::Emulator_context& state) {
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return EXIT_FAILURE;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    std::string window_title = fmt::format(core::tr("Saturnin {0} - Modern rendering"), core::saturnin_version);
    auto window = glfwCreateWindow(1280, 720, window_title.c_str(), NULL, NULL);
    if (window == nullptr) {
        return EXIT_FAILURE;
    }

    glfwSetWindowCloseCallback(window, windowCloseCallback);
    glfwSetWindowUserPointer(window, (void*)&state);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

	glbinding::initialize(glfwGetProcAddress);

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    // 
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    bool show_test_window = true;
    bool show_another_window = false;
    bool show_video = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    Opengl opengl(state.config());
    uint32_t fbo = opengl.createFramebuffer();
    //opengl.setup_triangle();

    //std::vector<uint8_t> image;
    //opengl.load_icons(image);
    //GLuint tex;
    //glEnable(GL_TEXTURE_2D);
    //glGenTextures(1, &tex);
    //glBindTexture(GL_TEXTURE_2D, tex);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

            
        gui::show_test_window(show_test_window);

        // Rendering
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        gui::buildGui(state, opengl, fbo, display_w, display_h);

        if (state.renderingStatus_ == core::RenderingStatus::reset) glfwSetWindowShouldClose(window, true);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void windowCloseCallback(GLFWwindow* window) {
    core::Emulator_context *state = reinterpret_cast<core::Emulator_context*>(glfwGetWindowUserPointer(window));
    state->renderingStatus_ = core::RenderingStatus::stopped;
}



};
};