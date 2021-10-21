# rltos
Task scheduler for the Renesas RL78.

# Instructions to include minimally the task scheduler in the project
- Provide compiler with path to rltos.h header.
	- Found in rltos\
- Provide compiler with path to rltos_port.h
	- Found in rltos\port\<DEVICE>\
- Include AT LEAST rltos task list and rltos base code
	- Task list code found in rltos\rltos_kernel\task_list\
	- Base code found in rltos\rltos_kernel\rltos.c
- Include only 1 rltos_scheduler_asm.asm file which is correct for your target cpu/compiler.
	- Found in rltos\port\<DEVICE>\compiler\<TARGET_COMPILER>

The folder structure for a minimal project containing just the task scheduler for RL78 on CLANG will look like this:

<Project>
 |
 +-- main.c
 |    
 +-- <rltos>
	|
	+-- rltos.h
	|
	+-- <port>
	|	|
	|	+-- <RL78>
	|		|
	|		+-- rltos_port.h
	|		|
	|		+-- rltos_port.c
	|		|
	|		+-- <compiler>
	|			|
	|			+-- <CLANG>
	|				|
	|				+-- rltos_scheduler_asm.asm
	|
	+-- <rltos_kernel>
		|
		+-- rltos.c
		|
		+-- <task_list>
			|
			+-- rltos_task_list.c
			|
			+-- rltos_task_list.h

To include more modules, such as mutexes, include them in the file structure of the project from the kernel directory and ensure you provide the compiler with a path to the modules header file e.g. for mutex that is rltos_mutex.h.

## Note for unit testing
The port for unit testing contains a rltos_scheduler_asm_dummy c file which implements skeleton versions of what should be implemented in the rltos_scheduler_asm.asm file.
The unit testing port should work for most host based compilers - testing will only ever be performed on MSVC through visual studio.
The testing folder should contain a CMakeLists.txt file for CMAKE to generate the testing project.

## Note for vs code
The settings.JSON file contains two entries only to enable working from vs code and running the unit tests through cmake and the cmake plug in for vs code.
### "cmake.sourceDirectory": "${workspaceFolder}/test"
This entry sets the cmake extension to work from the test folder.
### "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
This entry ensures that the cmake extension provides all necessary configuration information to the C/C++ extension.
### Extenions required
- C/C++ by Microsoft
- CMake Tools by Microsoft

## Note on CLANG
Only supports near data model - awaiting compiler update to support.

## Requirements for unit testing (only currently supports MSVC and windows)
- [CppUtest](https://cpputest.github.io/)
- [CMake](https://cmake.org/)

## Requirements for code quality checks
- [Cppcheck](https://cppcheck.sourceforge.io/)

## Requirements for documentation production
- [Doxygen](https://www.doxygen.nl/index.html)
- [GraphViz](http://www.graphviz.org/)

## Notes on documentation
Documentation for current code can be found [here!](https://wraydev.github.io/rltos/)

## NOTES:
- Can't implement sleep function until insert list function sorts the list!

## TODO NEXT (in order - unit test as we go)
- On insert, sort the lists according to their sorting value (head = smallest number).
- Unit test code thoroughly - checkpoint.
- Integrate [Cppcheck](https://cppcheck.sourceforge.io/) in both unit tests and rltos main code (for each port).
- Setup doxyfile and provide doc folder.
- Tidy code banners & comments with license and proper descriptions +  author used for doxygen.
- Including behavioural tests observing switching between IDLE and RUNNING tasks - checkpoint.

## TODO
- Implement event_flags
- Implement mutex
- Implement queue
- Implement semaphore
- Create example project + documentation to setup two tasks.
- Implement co-operative scheduling
- Write install guide for RL78 port.
- Write instruction for getting up and running with unit testing this project.
