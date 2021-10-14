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

## TODO NEXT
- Write sleep function and make use of idle task list in scheduler.

## TODO
- Implement event_flags
- Implement mutex
- Implement queue
- Implement semaphore
- Setup unit testing on CPPUTEST and include files in test repo.
- Setup doxyfile and provide doc folder.
- Create example project + documentation to setup two tasks.
- Implement co-operative scheduling
- Break RLTOS into seperate repo from ut_rl78
- Write install guide for RL78 port.
- Write instruction for getting up and running with unit testing this project.
- Explain the setings.JSON
	- How the cmake.sourceDirectory sets the testing CMakeLists.
	- How C_Cpp.default.configurationProvider - takes the config from cmake tools and provides it to the c/c++ information for intellisense.
	- Testing only supported currently in on MSVC.
