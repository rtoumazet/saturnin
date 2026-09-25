cmake_minimum_required(VERSION 4.4)

project(saturnin VERSION 0.1 LANGUAGES C CXX)

if(NOT WIN32)
	message(FATAL_ERROR "Saturnin requires Windows.")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glbinding CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(date CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(lodepng CONFIG REQUIRED)
find_package(libconfig CONFIG REQUIRED)
find_package(libzippp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

file(GLOB_RECURSE SATURNIN_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/lib/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/lib/*.c"
)

list(FILTER SATURNIN_SOURCES EXCLUDE REGEX "/interrupt_controller\\.cpp$")
list(FILTER SATURNIN_SOURCES EXCLUDE REGEX "/sample/")
list(FILTER SATURNIN_SOURCES EXCLUDE REGEX "/m68k_in\\.c$")
list(FILTER SATURNIN_SOURCES EXCLUDE REGEX "/m68kdasm\\.c$")
list(FILTER SATURNIN_SOURCES EXCLUDE REGEX "/m68kmake\\.c$")

file(GLOB_RECURSE SATURNIN_HEADERS CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/src/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/src/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/lib/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/lib/*.hpp"
)

add_executable(saturnin WIN32 ${SATURNIN_SOURCES} ${SATURNIN_HEADERS})

target_include_directories(saturnin PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/saturnin"
	"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include"
)

target_compile_definitions(saturnin PRIVATE
	NOMINMAX
	UNICODE
	_UNICODE
	_SCL_SECURE_NO_WARNINGS
	IMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/saturnin/src/video/imgui_wrapper.h"
	IMGUI_IMPL_OPENGL_LOADER_CUSTOM="${CMAKE_CURRENT_SOURCE_DIR}/saturnin/lib/imgui/imgui_loader.h"
)

target_compile_options(saturnin PRIVATE /utf-8 /W3 /MP /bigobj)
target_precompile_headers(saturnin PRIVATE
	"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/saturnin/src/pch.h>"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(saturnin PRIVATE /permissive-)
endif()

target_link_libraries(saturnin PRIVATE
	glfw
	glbinding::glbinding
	glbinding::glbinding-aux
	glm::glm
	imgui::imgui
	date::date
	fmt::fmt
	lodepng
	libzippp::libzippp
	libconfig::libconfig++
	libconfig::libconfig
	spdlog::spdlog
	OpenGL::GL
	Shlwapi.lib
)

target_link_options(saturnin PRIVATE /ENTRY:mainCRTStartup)

set_target_properties(saturnin PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>"
)

target_compile_definitions(saturnin PRIVATE $<$<CONFIG:Release>:IMGUI_IMPL_OPENGL_DEBUG>)

add_custom_command(TARGET saturnin POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/stv"
		"$<TARGET_FILE_DIR:saturnin>/stv"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/lang/exe"
		"$<TARGET_FILE_DIR:saturnin>/lang"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/saturnin/src/video/opengl/shaders"
		"$<TARGET_FILE_DIR:saturnin>/shaders"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/saturnin" FILES ${SATURNIN_SOURCES} ${SATURNIN_HEADERS})
