Saturnin is a Sega Saturn emulator

[![MSBuild](https://github.com/rtoumazet/saturnin/actions/workflows/msbuild.yml/badge.svg)](https://github.com/rtoumazet/saturnin/actions/workflows/msbuild.yml)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=rtoumazet_saturnin&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=rtoumazet_saturnin)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=rtoumazet_saturnin&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=rtoumazet_saturnin)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=rtoumazet_saturnin&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=rtoumazet_saturnin)

### How do I get set up? ###

  #### Configuration ####
	
  Before doing anything after cloning the repository, remove template extensions from properties files in the **./props** directory.

  Next, you must update user macros from properties sheets inside VS2022 to set up your directories. To do so, open the Property Manager first ( **View** | **Property Manager** ), then update the **solution** sheet macros according to your setup.

  #### Dependencies ####

  They are handled using [vcpkg](https://github.com/Microsoft/vcpkg). You'll find in **./saturnin/lib/vcpkg** everything necessary to build the libraries used by Saturnin.
    
  Follow these steps:
    
1. remove **template** extension from **vcpkg.cfg.template**
1. update **vcpkg_full_path** in **vcpkg.cfg** according to your vcpkg install.
1. run **install_libraries.bat dynamic** for building and installing dynamic libraries or **install_libraries.bat static** for building and installing static ones.
    
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
