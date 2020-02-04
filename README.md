Saturnin is a Sega Saturn emulator

### How do I get set up? ###

* Configuration
	
	Before doing anything after cloning the repository, remove template extensions from properties files in the **./props** directory.

    After that, you must update user macros from properties sheets inside VS2017 to set up your directories. To do so, open the Property Manager first 
    ( **View** | **Property Manager** ), then update the following sheets macros according to your setup : **dll**, **solution**.

* Dependencies 

    They are handled using vcpkg. You'll find in **./saturnin/lib/vcpkg** everything necessary to build the libraries used by Saturnin.
    
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
    * spirit-po (translations)

### Contribution guidelines ###

* [Saturnin Style Guide](https://bitbucket.org/Runik/saturnin-vs2017/wiki/Saturnin%20Style%20Guide)
* Code review
* Other guidelines

### Who do I talk to? ###

* Runik (Renaud Toumazet)
