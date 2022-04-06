/*
 * task_tests.c
 *
 *  Created on: 14 Oct 2021
 *      Author: Louis Wray
 */

#include "CppUTest/TestHarness.h"
#include <memory>

extern "C"
{
#include "rltos_task.h"
#include "task_scheduler/rltos_task_scheduler_prv.h"
/* Including the source file allows us to test the internal workings of the module*/
#include "rltos_task.c"
}
#include <array>
#include <random>
#include <iostream>
#include <functional>



/** @brief The dummy task function used for initialisation of tasks*/
static void Dummy_task_func(void);

/** Test group for task list initialser functions*/
TEST_GROUP(TaskTestGroup)
{
   static const size_t tasks_to_add = 100;
   static const size_t stack_size_units = 32;

   /* Create testable tasks*/
   std::array<dummy_task_t, tasks_to_add> group_tasks_under_test;
   stack_type group_stacks_under_test[tasks_to_add][stack_size_units];
   struct task_list_t l_aux_list_under_test;

   void setup(void)
   {
      /* Create random number generation function*/
      std::random_device rd;
      std::mt19937 g(rd());
      std::uniform_int_distribution<> distrib(0);

      /* Initialise every task - half the tasks in the running state and half in the idle state - with random priorities*/
      for(uint32_t init_index = 0U; init_index < (tasks_to_add-1U); ++init_index)
      {
         const bool task_running = (init_index % 2) == 0;
         Rltos_task_create(&(group_tasks_under_test[init_index]), group_stacks_under_test[init_index], &Dummy_task_func, static_cast<rltos_uint>(distrib(g)), task_running);
      }
      
      /* Garuntee there is always two tasks with the same priority - satisfy testing of same priority task handling*/
      Rltos_task_create(&(group_tasks_under_test[tasks_to_add-1U]), group_stacks_under_test[tasks_to_add-1U], &Dummy_task_func, ((p_task_ctl_t)(&(group_tasks_under_test[0])))->priority, false);

      Rltos_kernel_enter();

      Task_list_init(&l_aux_list_under_test);
   }
   /* END OF FUNCTION*/

   void teardown(void)
   {
      /* Initialise every task*/
      for(uint32_t deinit_index = 0U; deinit_index < tasks_to_add; ++deinit_index)
      {
         Rltos_task_destroy(&(group_tasks_under_test[deinit_index]));
      }

      Rltos_kernel_kill();
   }
   /* END OF FUNCTION*/
};

static void Dummy_task_func(void)
{
   /* Do nothing*/
}
/* END OF FUNCTION*/

TEST(TaskTestGroup, RltosTaskCreate_ValuesOk)
{
   for(dummy_task_t const &tsk_to_tst : group_tasks_under_test)
   {
      p_task_ctl_t p_tsk_to_tst = (p_task_ctl_t)&tsk_to_tst;
      CHECK_TEXT(p_tsk_to_tst->p_task_func == &Dummy_task_func, "Task function incorrectly initialised");
      CHECK_TEXT(p_tsk_to_tst->p_owners[aux_list] == NULL, "Task aux list incorrectly initialised");
      CHECK_TEXT((p_tsk_to_tst->p_owners[state_list] == &running_task_list) || (p_tsk_to_tst->p_owners[state_list] == &stopped_task_list), "Task not in valid starting list");
   }
}
/* END OF TEST*/

TEST(TaskTestGroup, RltosTaskDestroy_ValuesOk)
{
   p_task_ctl_t p_tsk_to_tst = (p_task_ctl_t)&group_tasks_under_test[0];

   Rltos_task_destroy(&group_tasks_under_test[0]);

   CHECK_TEXT(p_tsk_to_tst->p_task_func == NULL, "Task function not set to NULL");
   CHECK_TEXT(p_tsk_to_tst->stored_sp == NULL, "Task stack pointer not set to NULL");
   CHECK_TEXT(p_tsk_to_tst->p_owners[aux_list] == NULL, "Task aux list owner incorrectly initialised");
   CHECK_TEXT(p_tsk_to_tst->p_owners[state_list] == NULL, "Task state list owner incorrectly initialised");
   CHECK_TEXT(!Task_is_in_list(&running_task_list, p_tsk_to_tst, state_list), "Task incorrectly still in running list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, p_tsk_to_tst, state_list), "Task incorrectly still in idle list");
   CHECK_TEXT(!Task_is_in_list(&stopped_task_list, p_tsk_to_tst, state_list), "Task incorrectly still in stopped list");
}
/* END OF TEST*/

TEST(TaskTestGroup, RltosTaskSleep_InTheCorrectList)
{
   p_task_ctl_t l_prev_cur_task = p_current_task_ctl;

   /* Check 0 value has no effect*/
   Rltos_task_sleep(0U);
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_prev_cur_task, state_list), "Task not in running list when it should be");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_prev_cur_task, state_list), "Task placed in idle list when it shouldn't be");
   CHECK_TEXT(!Task_is_in_list(&stopped_task_list, l_prev_cur_task, state_list), "Task placed in stopped list when it shouldn't be");

   l_prev_cur_task = p_current_task_ctl;

   /* Check non-0 value changes task state list*/
   Rltos_task_sleep(1U);
   CHECK_TEXT(!Task_is_in_list(&running_task_list, l_prev_cur_task, state_list), "Task placed in running list when it shouldn't be");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_prev_cur_task, state_list), "Task not placed in idle list when it should be");
   CHECK_TEXT(!Task_is_in_list(&stopped_task_list, l_prev_cur_task, state_list), "Task placed in stopped list when it shouldn't be");
}
/* END OF TEST*/





/** Test group for dummy struct sizes*/
TEST_GROUP(RltosSizesTestGroup){
    void setup(void){
        /* Do Nothing*/
    }
    /* END OF FUNCTION*/

    void teardown(void){
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
