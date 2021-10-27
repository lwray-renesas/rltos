/*
 * task_tests.c
 *
 *  Created on: 14 Oct 2021
 *      Author: Louis Wray
 */

#include "CppUTest/TestHarness.h"
#include <memory>

/* Including the source file allows us to test the internal workings of the rltos_task_list module*/
#include "rltos_kernel/task/rltos_task.c"

/** Test group for task list initialser functions*/
TEST_GROUP(TaskTestGroup)
{

   void setup(void)
   {
      
   }
   /* END OF FUNCTION*/

   void teardown(void)
   {
      
   }
   /* END OF FUNCTION*/
};

TEST(TaskTestGroup, FirstFailingTest)
{
   CHECK_TEXT(false, "TODO: Write rltos_task tests");
}
/* END OF TEST*/
