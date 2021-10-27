# Minimum cmake version
cmake_minimum_required(VERSION 3.20)

# Create project
project(rltos_test)

# Setup RLTOS base directory
set(RLTOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/)

########################################## LISTING OF SOURCE - START ##########################################

##### List RLTOS source files #####
set(RLTOS_SOURCE_FILES
${RLTOS_DIR}/port/CPPUTEST_UNIT_TESTING/rltos_scheduler_asm_dummy.c
${RLTOS_DIR}/port/CPPUTEST_UNIT_TESTING/rltos_port.c
${RLTOS_DIR}/rltos_kernel/rltos.c
${RLTOS_DIR}/rltos_kernel/task_list/rltos_task_list.c
${RLTOS_DIR}/rltos_kernel/event_flags/rltos_event_flags.c
${RLTOS_DIR}/rltos_kernel/mutex/rltos_mutex.c
${RLTOS_DIR}/rltos_kernel/queue/rltos_queue.c
${RLTOS_DIR}/rltos_kernel/semaphore/rltos_semaphore.c)

##### List RLTOS header files #####
set(RLTOS_HEADER_FILES
${RLTOS_DIR}/port/CPPUTEST_UNIT_TESTING/rltos_port.h
${RLTOS_DIR}/rltos.h
${RLTOS_DIR}/rltos_kernel/task_list/rltos_task_list.h
${RLTOS_DIR}/rltos_kernel/event_flags/rltos_event_flags.h
${RLTOS_DIR}/rltos_kernel/mutex/rltos_mutex.h
${RLTOS_DIR}/rltos_kernel/queue/rltos_queue.h
${RLTOS_DIR}/rltos_kernel/semaphore/rltos_semaphore.h)

##### List TEST source files #####
set(TEST_SOURCE_FILES
all_tests.cpp
rltos_kernel/task_list/task_list_tests.cpp)

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

# Add the following directories
target_include_directories(rltos_test PUBLIC 
${CPPUTEST_HOME_DIR}/include
${RLTOS_DIR}
${RLTOS_DIR}/port/CPPUTEST_UNIT_TESTING)

# Add the following libraries
target_link_libraries(rltos_test LINK_PUBLIC 
${CPPUTEST_HOME_DIR}/lib/CppUTest.lib 
${CPPUTEST_HOME_DIR}/lib/CppUTestExt.lib
winmm.lib)

# Group source
source_group(TREE ${RLTOS_DIR} PREFIX rltos FILES ${RLTOS_HEADER_FILES} ${RLTOS_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX test FILES ${TEST_HEADER_FILES} ${TEST_SOURCE_FILES})

# To compensate for C++'s lack of designated initializers, we must use msvc flag /std:c++latest
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
