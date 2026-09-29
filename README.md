Saturnin is a Sega Saturn emulator

[![MSBuild](https://github.com/rtoumazet/saturnin/actions/workflows/cmake.yml/badge.svg)](https://github.com/rtoumazet/saturnin/actions/workflows/cmake.yml)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=rtoumazet_saturnin&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=rtoumazet_saturnin)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=rtoumazet_saturnin&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=rtoumazet_saturnin)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=rtoumazet_saturnin&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=rtoumazet_saturnin)

### How do I get set up? ###

  #### Configuration and build ####

  Saturnin requires Windows, Visual Studio 2026 (18.10 or later), CMake 4.4 or later, and [vcpkg](https://github.com/Microsoft/vcpkg). Set `VCPKG_ROOT` to your vcpkg installation and make sure the Visual Studio C++ workload is installed.

  From the repository root, configure the x64 static build with CMake:

  ```powershell
  cmake -S . -B build -G "Visual Studio 18 2026" -A x64 `
    -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
    -DVCPKG_TARGET_TRIPLET=x64-windows-static
  ```

  The vcpkg manifest in `vcpkg.json` supplies the dependencies and they are installed during configuration. Build the desired configuration with:

  ```powershell
  cmake --build build --config Debug --parallel
  cmake --build build --config RelWithDebInfo --parallel
  cmake --build build --config Release --parallel
  ```

  `RelWithDebInfo` replaces the former `DebugFast` configuration. 32-bit configurations are no longer supported. The executable and runtime assets are written below `build/bin/<configuration>`.

  CMake presets are also available as `windows-x64-debug`, `windows-x64-relwithdebinfo`, and `windows-x64-release`, with matching build presets. They use the same x64 static vcpkg triplet. Set the `external-build-root.binaryDir` value in `CMakePresets.json` to a suitable local build directory before using them.
    
  List of used libraries for reference:
    
* [argagg](https://github.com/vietjtnguyen/argagg) : A simple C++11 command line argument parser.
* [date](https://github.com/HowardHinnant/date) :  A date and time library based on the C++11/14/17 <chrono> header.
* [embed](https://github.com/MKlimenko/embed) : std::embed implementation for the poor (C++17).
* [glbinding](https://github.com/cginternals/glbinding) : A C++ binding for the OpenGL API, generated using the gl.xml specification.
* [glfw3](https://github.com/glfw/glfw) : A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input.
* [glm](https://github.com/g-truc/glm) : OpenGL Mathematics (GLM).
* [dear imgui](https://github.com/ocornut/imgui) : Bloat-free Graphical User interface for C++ with minimal dependencies.
* [libconfig](https://github.com/hyperrealm/libconfig) : C/C++ library for processing configuration files.
* [libzip](https://github.com/nih-at/libzip) : A C library for reading, creating, and modifying zip archives.
* [libzippp](https://github.com/ctabin/libzippp) : C++ wrapper for libzip.
* [lodepng](https://github.com/lvandeve/lodepng) : PNG encoder and decoder in C and C++.
* [spdlog](https://github.com/gabime/spdlog) : Fast C++ logging library.
* [thread-pool](https://github.com/bshoshany/thread-pool) : A fast, lightweight, and easy-to-use C++17 thread pool library
### Contribution guidelines ###

* [Saturnin Style Guide](https://github.com/rtoumazet/saturnin/wiki/Saturnin-style-guide)

### Who do I talk to? ###

  * Runik (Renaud Toumazet)

  [![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/S6S2122MKA)
