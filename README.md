Saturnin is a Sega Saturn emulator

### How do I get set up? ###

* Summary of set up
* Configuration
	
	Before doing anything, remove template extensions from properties files in the **./props** directory.

    After that, you must update user macros from properties sheets inside VS2017 to set up your directories. To do so, open the Property Manager first 
    ( **View** | **Property Manager** ), then update the following sheets macros according to your setup : **dll**, **solution**.

* Dependencies 

    They are handled using vcpkg. You'll find in **./saturnin/lib/vcpkg** everything necessary to build the libraries used by Saturnin.
    
    Follow these steps:
    1. remove **template** extension from **vcpkg.cfg.template**
    2. update **vcpkg_full_path** in **vcpkg.cfg** according to your vcpkg install
    3. run **install_libraries.bat dynamic** for building and installing dynamic libraries or **install_libraries.bat static** for building and installing static ones.
    
    List of used libraries for reference:
    * boost-locale 1.67.0 (locale)
	* boost-filesystem 1.67.0 (filesystem)
	* glfw3 (opengl context)
	* imgui 1.62 (gui)
    * libepoxy 1.4.3 (opengl functions management)
	* libconfig 1.7.1 (configuration)
	* libzip rel-1-5-1 (zip handling)
	* spdlog 0.16.3 (logging + string formatting)

### Contribution guidelines ###

* [Saturnin Style Guide](https://bitbucket.org/Runik/saturnin-vs2017/wiki/Saturnin%20Style%20Guide)
* Code review
* Other guidelines

### Who do I talk to? ###

* Runik (Renaud Toumazet)
