/**
\mainpage

RLTOS is a real time operating system targetted at small embedded systemas with the aim to provide fully static allocation of all obects & tasks.
The name RLTOS comes from a play on the words RL78 and RTOS as at inception the systems first target was the Renesas RL78.

\section quickstart Quickstart
<hr>

\section project Project
<hr>
A project deep dive - going into configurations and algorithms and justifications of techniques etc.

\section coding_standards Coding Standards
<hr>

\subsection cppcheck CppCheck
- <a href="https://www.misra.org.uk/">MISRA C 2012</a> implemented through <a href="https://cppcheck.sourceforge.io/">Cppcheck</a>

\subsection doxygen Doxygen
- <a href="https://www.doxygen.nl/index.html">Doxygen</a>
- <a href="http://www.graphviz.org/">GraphViz</a>
- <a href="https://jothepro.github.io/doxygen-awesome-css/">doxygen-awesome-css</a>


\section unit_testing Unit Testing
<hr>
The port for unit testing contains a rltos_scheduler_asm_dummy c file which implements skeleton versions of what should be implemented in the rltos_scheduler_asm.asm file.
The unit testing port should work for most host based compilers - testing will only ever be performed on MSVC through visual studio.
The testing folder should contain a CMakeLists.txt file for CMAKE to generate the testing project.


\subsection vs_code VS Code
The settings.JSON file contains two entries only to enable working from vs code and running the unit tests through cmake and the cmake plug in for vs code.
- "cmake.sourceDirectory": "${workspaceFolder}/test"
This entry sets the cmake extension to work from the test folder.

- "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
This entry ensures that the cmake extension provides all necessary configuration information to the C/C++ extension.

Extenions required:
- C/C++ by Microsoft
- CMake Tools by Microsoft

\subsection cmake CMake
- <a href="https://cmake.org/">CMake</a>

\subsection cpputest CppUTest
- <a href="https://cpputest.github.io/">CppUTest</a>


\section todo TODO
<hr>
List what we need to do moving forward with the project.

\subsection next Next
- On insert, sort the lists according to their sorting value (head = smallest number).
- Unit test code thoroughly & MISRA clean - checkpoint.
- Review & tidy all comments paying attention to file code banners & comments with license and proper descriptions + author (doxygen).
- Including behavioural tests observing switching between IDLE and RUNNING tasks - checkpoint.

\subsection soon Soon
- Implement event_flags
- Implement mutex
- Implement queue
- Implement semaphore
- Create example project + documentation to setup two tasks.
- Implement co-operative scheduling
- Write install guide for RL78 port.
- Write instruction for getting up and running with unit testing this project.

\section ports Ports
<hr>
\subsection cpputest_unit_testing CPPUTEST_UNIT_TESTING
This port exists to satisfy the unit testing environment.

\subsection rl78 RL78
Port for the Renesas RL78 Microcontroller.

\subsubsection clang CLANG
Port using the CLANG compiler

\note CLANG is only currently supported in the near data model (default) 

\subsubsection iar IAR
Port using the IAR compiler

*/