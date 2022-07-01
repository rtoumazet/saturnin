Saturnin is a Sega Saturn emulator

### How do I get set up? ###

* Configuration
	
	Before doing anything after cloning the repository, remove template extensions from properties files in the **./props** directory.

    Next, you must update user macros from properties sheets inside VS2019 to set up your directories. To do so, open the Property Manager first 
    ( **View** | **Property Manager** ), then update the **solution** sheet macros according to your setup.

* Dependencies 

    They are handled using [vcpkg](https://github.com/Microsoft/vcpkg). You'll find in **./saturnin/lib/vcpkg** everything necessary to build the libraries used by Saturnin.
    
    Follow these steps:
    
    1. remove **template** extension from **vcpkg.cfg.template**
    1. update **vcpkg_full_path** in **vcpkg.cfg** according to your vcpkg install.
    1. run **install_libraries.bat dynamic** for building and installing dynamic libraries or **install_libraries.bat static** for building and installing static ones.
    
    List of used libraries for reference:
    
    * date (rtc handling)
    * glbinding (opengl functions management)
    * glfw3 (opengl context)
    * glm (opengl mathematics)
    * dear imgui (gui)
    * libconfig (configuration)
    * libzip (zip handling)
    * libzippp (zip wrapper)
    * lodepng (png format handling)
    * spdlog (logging + string formatting)
    * thread-pool : Shoshany, B. (2021). A C++17 Thread Pool for High-Performance Scientific Computing. arXiv. https://doi.org/10.5281/zenodo.4742687

### Contribution guidelines ###

* [Saturnin Style Guide](wiki/saturnin%20style%20guide.md)
* Code review
* Other guidelines

### Who do I talk to? ###

* Runik (Renaud Toumazet)
