/**
* all_tests.cpp
* 
* main file for running all unit tests.
*/

#include "CppUTest/CommandLineTestRunner.h"

int main(int ac, char** av)
{
	return CommandLineTestRunner::RunAllTests(ac, av);
}

