Saturnin is a Sega Saturn emulator

### How do I get set up? ###

* Summary of set up
* Configuration
	
	Before doing anything, remove template extensions from properties files:
	
	- props/\*.props.template -> props/\*.props  
	- saturnin/props/\*.props.template -> saturnin/props/\*.props    

    After that, you must update user macros from properties sheets inside VS2017 to set up your directories. To do so, open the Property Manager first 
    ( **View** | **Property Manager** ), then update each of the following sheets : "**dll**", "**solution.x86**", "**solution.x64**", "**saturnin.common**", "**saturnin.x86**", "**saturnin.x64**".
    
	You can then modify the files, adjusting the macros according to your setup.

* Dependencies
	* imgui 1.53 (gui)
  	* using vcpkg:
        * boost 1.67.0 (filesystem, locale)
        * epoxy 1.4.3 (opengl functions management)
		* libconfig 1.7.1 (configuration)
		* spdlog 0.16.3 (logging + string formatting)
* Deployment instructions

### Contribution guidelines ###

* [Saturnin Style Guide](https://bitbucket.org/Runik/saturnin-vs2017/wiki/Saturnin%20Style%20Guide)
* Code review
* Other guidelines

### Who do I talk to? ###

* Runik (Renaud Toumazet)
