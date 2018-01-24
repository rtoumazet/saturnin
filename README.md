Saturnin is a Sega Saturn emulator

### How do I get set up? ###

* Summary of set up
* Configuration
	
	Before doing anything, remove template extension from project files:
	
	- saturnin_common.props.template -> saturnin_common.props  
	- saturnin_x86.props.template -> saturnin_x86.props  
	- saturnin_x64.props.template -> saturnin_x64.props  

After that, you must change saturnin property page from inside VS2017 to set up your directories. To do so, display the Propety Manager first 
( **View** | **Property Manager** ), then select **saturnin** | **Debug|Win32** | **saturnin_x86**, and **saturnin** | **Debug|Win64** | **saturnin_x64**.
You can then modify the files, adjusting the macros according to your setup.

* Dependencies
    * boost 1.65.1
	* imgui 1.52
	* libconfig 1.7.1 
	* epoxy 1.4.3
	* spdlog 0.16.3
* Deployment instructions

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Runik (Renaud Toumazet)
