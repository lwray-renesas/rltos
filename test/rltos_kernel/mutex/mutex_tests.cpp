/*
 * mutex_tests.c
 *
 *  Created on: 14 Oct 2021
 *      Author: Louis Wray
 */

#include "CppUTest/TestHarness.h"
#include <memory>

extern "C" {
#include "rltos_task.h"
#include "task_scheduler/rltos_task_scheduler_prv.h"
/* Including the source file allows us to test the internal workings of the module*/
#include "mutex/rltos_mutex.c"
}





/** Test group for dummy struct sizes*/
TEST_GROUP(Mutex)
{
   void setup(void)
   {
       /* Do Nothing*/
   }
   /* END OF FUNCTION*/

   void teardown(void)
   {
       /* Do Nothing*/
   }
   /* END OF FUNCTION*/
};

TEST(Mutex, Placeholder)
{
   CHECK_TEXT(true, "No Code To Test");
}
/* END OF TEST*/
