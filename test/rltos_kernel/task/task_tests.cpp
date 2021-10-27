/*
 * task_tests.c
 *
 *  Created on: 14 Oct 2021
 *      Author: Louis Wray
 */

#include "CppUTest/TestHarness.h"
#include <memory>

extern "C" {
/* Including the source file allows us to test the internal workings of the module*/
#include "rltos_task.c"
}





/***************************************************************************************************
 * Task                                                                                            *
 **************************************************************************************************/

/* Collection of dummy functions for task create tests*/
static void task0_func(void) { }
static void task1_func(void) { }
static void task2_func(void) { }
static void task3_func(void) { }
static void task4_func(void) { }

/** Test group for task list initialser functions*/
TEST_GROUP(TaskTestGroup)
{
   dummy_task_t task0;
   stack_type task0_stack[32];

   dummy_task_t task1;
   stack_type task1_stack[32];
   
   dummy_task_t task2;
   stack_type task2_stack[32];
   
   dummy_task_t task3;
   stack_type task3_stack[32];
   
   dummy_task_t task4;
   stack_type task4_stack[32];

   void setup(void)
   {
      Rltos_task_create(&task0, task0_stack, &task0_func, 0U, true);
      Rltos_task_create(&task1, task1_stack, &task1_func, 0U, true);
      Rltos_task_create(&task2, task2_stack, &task2_func, 0U, true);
      Rltos_task_create(&task3, task3_stack, &task3_func, 0U, true);
      Rltos_task_create(&task4, task4_stack, &task4_func, 0U, true);
   }
   /* END OF FUNCTION*/

   void teardown(void)
   {
      Rltos_task_destroy(&task0);
      Rltos_task_destroy(&task1);
      Rltos_task_destroy(&task2);
      Rltos_task_destroy(&task3);
      Rltos_task_destroy(&task4);
   }
   /* END OF FUNCTION*/
};

TEST(TaskTestGroup, RltosKernelEnter_DoesNotCrash)
{
   Rltos_kernel_enter();
   CHECK(true);
}
/* END OF TEST*/

TEST(TaskTestGroup, RltosTaskCreate_ValuesOk)
{
   CHECK_TEXT(((p_task_ctl_t)&task0)->p_task_func == &task0_func, "Task 0 of 4 function incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task1)->p_task_func == &task1_func, "Task 1 of 4 function incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task2)->p_task_func == &task2_func, "Task 2 of 4 function incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task3)->p_task_func == &task3_func, "Task 3 of 4 function incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task4)->p_task_func == &task4_func, "Task 4 of 4 function incorrectly initialised");

   CHECK_TEXT(((p_task_ctl_t)&task0)->stored_sp == task0_stack, "Task 0 of 4 stack incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task1)->stored_sp == task1_stack, "Task 1 of 4 stack incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task2)->stored_sp == task2_stack, "Task 2 of 4 stack incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task3)->stored_sp == task3_stack, "Task 3 of 4 stack incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task4)->stored_sp == task4_stack, "Task 4 of 4 stack incorrectly initialised");

   CHECK_TEXT(((p_task_ctl_t)&task0)->priority == 0U, "Task 0 of 4 priority incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task1)->priority == 0U, "Task 1 of 4 priority incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task2)->priority == 0U, "Task 2 of 4 priority incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task3)->priority == 0U, "Task 3 of 4 priority incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task4)->priority == 0U, "Task 4 of 4 priority incorrectly initialised");

   CHECK_TEXT(((p_task_ctl_t)&task0)->p_owners[aux_list] == NULL, "Task 0 of 4 aux list incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task1)->p_owners[aux_list] == NULL, "Task 1 of 4 aux list incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task2)->p_owners[aux_list] == NULL, "Task 2 of 4 aux list incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task3)->p_owners[aux_list] == NULL, "Task 3 of 4 aux list incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task4)->p_owners[aux_list] == NULL, "Task 4 of 4 aux list incorrectly initialised");

   CHECK_TEXT(Task_is_in_list(&running_task_list, (p_task_ctl_t)&task0, state_list), "Task 0 of 4 not placed in running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, (p_task_ctl_t)&task1, state_list), "Task 1 of 4 not placed in running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, (p_task_ctl_t)&task2, state_list), "Task 2 of 4 not placed in running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, (p_task_ctl_t)&task3, state_list), "Task 3 of 4 not placed in running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, (p_task_ctl_t)&task4, state_list), "Task 4 of 4 not placed in running list");

   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task0, state_list), "Task 0 of 4 erroneously placed in idle list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task1, state_list), "Task 1 of 4 erroneously placed in idle list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task2, state_list), "Task 2 of 4 erroneously placed in idle list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task3, state_list), "Task 3 of 4 erroneously placed in idle list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task4, state_list), "Task 4 of 4 erroneously placed in idle list");
}
/* END OF TEST*/

TEST(TaskTestGroup, RltosTaskDestroy_ValuesOk)
{
   Rltos_task_destroy(&task0);
   Rltos_task_destroy(&task1);
   Rltos_task_destroy(&task2);
   Rltos_task_destroy(&task3);
   Rltos_task_destroy(&task4);

   CHECK_TEXT(((p_task_ctl_t)&task0)->p_task_func == NULL, "Task 0 of 4 function not set to NULL");
   CHECK_TEXT(((p_task_ctl_t)&task1)->p_task_func == NULL, "Task 1 of 4 function not set to NULL");
   CHECK_TEXT(((p_task_ctl_t)&task2)->p_task_func == NULL, "Task 2 of 4 function not set to NULL");
   CHECK_TEXT(((p_task_ctl_t)&task3)->p_task_func == NULL, "Task 3 of 4 function not set to NULL");
   CHECK_TEXT(((p_task_ctl_t)&task4)->p_task_func == NULL, "Task 4 of 4 function not set to NULL");

   CHECK_TEXT(((p_task_ctl_t)&task0)->stored_sp == NULL, "Task 0 of 4 stack not set to NULL");
   CHECK_TEXT(((p_task_ctl_t)&task1)->stored_sp == NULL, "Task 1 of 4 stack not set to NULL");
   CHECK_TEXT(((p_task_ctl_t)&task2)->stored_sp == NULL, "Task 2 of 4 stack not set to NULL");
   CHECK_TEXT(((p_task_ctl_t)&task3)->stored_sp == NULL, "Task 3 of 4 stack not set to NULL");
   CHECK_TEXT(((p_task_ctl_t)&task4)->stored_sp == NULL, "Task 4 of 4 stack not set to NULL");

   CHECK_TEXT(((p_task_ctl_t)&task0)->p_owners[aux_list] == NULL, "Task 0 of 4 aux list incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task1)->p_owners[aux_list] == NULL, "Task 1 of 4 aux list incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task2)->p_owners[aux_list] == NULL, "Task 2 of 4 aux list incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task3)->p_owners[aux_list] == NULL, "Task 3 of 4 aux list incorrectly initialised");
   CHECK_TEXT(((p_task_ctl_t)&task4)->p_owners[aux_list] == NULL, "Task 4 of 4 aux list incorrectly initialised");

   CHECK_TEXT(!Task_is_in_list(&running_task_list, (p_task_ctl_t)&task0, state_list), "Task 0 of 4 incorrectly placed in running list");
   CHECK_TEXT(!Task_is_in_list(&running_task_list, (p_task_ctl_t)&task1, state_list), "Task 1 of 4 incorrectly placed in running list");
   CHECK_TEXT(!Task_is_in_list(&running_task_list, (p_task_ctl_t)&task2, state_list), "Task 2 of 4 incorrectly placed in running list");
   CHECK_TEXT(!Task_is_in_list(&running_task_list, (p_task_ctl_t)&task3, state_list), "Task 3 of 4 incorrectly placed in running list");
   CHECK_TEXT(!Task_is_in_list(&running_task_list, (p_task_ctl_t)&task4, state_list), "Task 4 of 4 incorrectly placed in running list");

   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task0, state_list), "Task 0 of 4 incorrectly placed in idle list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task1, state_list), "Task 1 of 4 incorrectly placed in idle list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task2, state_list), "Task 2 of 4 incorrectly placed in idle list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task3, state_list), "Task 3 of 4 incorrectly placed in idle list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task4, state_list), "Task 4 of 4 incorrectly placed in idle list");
}
/* END OF TEST*/

TEST(TaskTestGroup, RltosTaskSleep_InTheCorrectList)
{
   p_current_task_ctl = (p_task_ctl_t)&task0; /* Must set current task pointer manually as we are not "in context" automatically during unit testing*/
   Rltos_task_sleep(0U); /* Check 0 value has no effect*/
   CHECK_TEXT(Task_is_in_list(&running_task_list, (p_task_ctl_t)&task0, state_list), "Task 0 of 4 not in running list when it should be");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task0, state_list), "Task 0 of 4 incorrectly placed in idle list");
   
   Rltos_task_sleep(1U); /* Check non-0 value changes task state list*/
   CHECK_TEXT(!Task_is_in_list(&running_task_list, (p_task_ctl_t)&task0, state_list), "Task 0 of 4 incorrectly still in running list");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task0, state_list), "Task 0 of 4 not in idle list wen it should be");
   
   /* Repeat test for new task*/
   p_current_task_ctl = (p_task_ctl_t)&task1;
   Rltos_task_sleep(0U);
   CHECK_TEXT(Task_is_in_list(&running_task_list, (p_task_ctl_t)&task1, state_list), "Task 1 of 4 not in running list when it should be");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task1, state_list), "Task 1 of 4 incorrectly placed in idle list");

   Rltos_task_sleep(1U);
   CHECK_TEXT(!Task_is_in_list(&running_task_list, (p_task_ctl_t)&task1, state_list), "Task 1 of 4 incorrectly still in running list");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, (p_task_ctl_t)&task1, state_list), "Task 1 of 4 not in idle list wen it should be");
}
/* END OF TEST*/





/***************************************************************************************************
 * Misc                                                                                            *
 **************************************************************************************************/

/** Test group for dummy struct sizes*/
TEST_GROUP(RltosSizesTestGroup)
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

TEST(RltosSizesTestGroup, Test_RltosSizes_DummyEqualToReal)
{
   CHECK_TEXT((sizeof(struct task_ctl_t) == sizeof(dummy_task_t)), "Dummy task struct is not the same size as the real task control struct");
   CHECK_TEXT((sizeof(struct task_list_t) == sizeof(dummy_task_list_t)), "Dummy task list struct is not the same size as the real task list struct");
}
/* END OF TEST*/
