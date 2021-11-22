# Minimum cmake version
cmake_minimum_required(VERSION 3.20)

# Create project
project(rltos_test)

# Setup RLTOS base directory
set(RLTOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/)

########################################## LISTING OF SOURCE - START ##########################################

##### List RLTOS source files - (port files & rltos_task.c to be tested within task_list) #####
set(RLTOS_SOURCE_FILES
${RLTOS_DIR}/port/CPPUTEST_UNIT_TESTING/rltos_scheduler_asm_dummy.c
${RLTOS_DIR}/port/CPPUTEST_UNIT_TESTING/rltos_port.c)

##### List RLTOS header files - (port files) #####
set(RLTOS_HEADER_FILES
${RLTOS_DIR}/port/CPPUTEST_UNIT_TESTING/rltos_port.h)

##### List TEST source files #####
set(TEST_SOURCE_FILES
all_tests.cpp
rltos_kernel/task/task_tests.cpp
rltos_kernel/task/task_scheduler/task_scheduler_tests.cpp
rltos_kernel/semaphore/semaphore_tests.cpp
rltos_kernel/queue/queue_tests.cpp
rltos_kernel/mutex/mutex_tests.cpp
rltos_kernel/event_flags/event_flags_tests.cpp)

##### List TEST header files #####
set(TEST_HEADER_FILES)

##### All source #####
set(ALL_SOURCE
${RLTOS_HEADER_FILES}
${RLTOS_SOURCE_FILES}
${TEST_HEADER_FILES}
${TEST_SOURCE_FILES})

########################################## LISTING OF SOURCE - STOP ##########################################

# Check for CPPUTEST_HOME environment variable
if(DEFINED ENV{CPPUTEST_HOME})
	# Get CPPUTEST Directory
	set(CPPUTEST_HOME_DIR "$ENV{CPPUTEST_HOME}" CACHE FILEPATH "Directory created after running CppUTest install build in windows")
else()
	set(CPPUTEST_HOME_DIR "Cannot find CPPUTEST_HOME" CACHE FILEPATH "Directory created after running CppUTest install build in windows")
endif()

# Creating an executable project
add_executable(rltos_test ${ALL_SOURCE})

# Add the following directories - only include directroies required by any project is a port directory containing rltos_port.h and the kernel task directory.
target_include_directories(rltos_test PUBLIC 
${CPPUTEST_HOME_DIR}/include
${RLTOS_DIR}/rltos_kernel
${RLTOS_DIR}/rltos_kernel/task
${RLTOS_DIR}/port/CPPUTEST_UNIT_TESTING)

# Add the following libraries
target_link_libraries(rltos_test LINK_PUBLIC 
${CPPUTEST_HOME_DIR}/lib/CppUTest.lib 
${CPPUTEST_HOME_DIR}/lib/CppUTestExt.lib)

if(WIN32)
	target_link_libraries(rltos_test LINK_PUBLIC winmm.lib)
endif()

# Group source
source_group(TREE ${RLTOS_DIR} PREFIX rltos FILES ${RLTOS_HEADER_FILES} ${RLTOS_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX test FILES ${TEST_HEADER_FILES} ${TEST_SOURCE_FILES})

