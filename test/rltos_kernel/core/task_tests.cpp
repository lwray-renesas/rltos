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
TEST_GROUP(Task)
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

TEST(Task, Test_kernel_enter)
{
   CHECK_TEXT(p_current_task_ctl != NULL, "Current task is NULL");
   CHECK_TEXT(rltos_system_tick == 0U, "System tick count is not 0");
   CHECK_TEXT(rltos_wrap_count == 0U, "System wrap count is not 0");
   CHECK_TEXT(running_task_list.size != 0U, "Running task list size is zero");
   CHECK_TEXT(running_task_list.p_head != NULL, "Running task list head pointer is NULL");
   CHECK_TEXT(running_task_list.p_index != NULL, "Running task list index pointer is NULL");
}
/* END OF TEST*/

TEST(Task, Test_kernel_kill)
{
   Rltos_kernel_kill();
   CHECK_TEXT(p_current_task_ctl == NULL, "Current task is not NULL");
}
/* END OF TEST*/

TEST(Task, Test_task_create)
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

TEST(Task, Test_task_destroy)
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

TEST(Task, Test_task_stop_pending_task)
{
   p_task_ctl_t l_task0_under_test = p_current_task_ctl->p_prev_tctl[state_list];
   Rltos_task_stop((p_dummy_task_t)l_task0_under_test);

   CHECK_TEXT(!Task_is_in_list(&running_task_list, l_task0_under_test, state_list), "Task incorrectly in running list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task0_under_test, state_list), "Task incorrectly in idle list");
   CHECK_TEXT(Task_is_in_list(&stopped_task_list, l_task0_under_test, state_list), "Task not in stopped list");
}
/* END OF TEST*/

TEST(Task, Test_task_stop_idled_task)
{
   Rltos_task_sleep(0x1234U); /* Put a task in the idle task list*/

   p_task_ctl_t l_task0_under_test = idle_task_list.p_head;
   Rltos_task_stop((p_dummy_task_t)l_task0_under_test);

   CHECK_TEXT(!Task_is_in_list(&running_task_list, l_task0_under_test, state_list), "Task incorrectly in running list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task0_under_test, state_list), "Task incorrectly in idle list");
   CHECK_TEXT(Task_is_in_list(&stopped_task_list, l_task0_under_test, state_list), "Task not in stopped list");
}
/* END OF TEST*/

TEST(Task, Test_task_stop_current_task)
{
   p_task_ctl_t l_prev_running_index = running_task_list.p_index;
   p_task_ctl_t l_task0_under_test = p_current_task_ctl;

   CHECK_TEXT(l_prev_running_index == p_current_task_ctl, "Initial running list index is not the current task");

   Rltos_task_stop((p_dummy_task_t)l_task0_under_test);

   CHECK_TEXT(!Task_is_in_list(&running_task_list, l_task0_under_test, state_list), "Task incorrectly in running list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task0_under_test, state_list), "Task incorrectly in idle list");
   CHECK_TEXT(Task_is_in_list(&stopped_task_list, l_task0_under_test, state_list), "Task not in stopped list");
   CHECK_TEXT(l_prev_running_index != running_task_list.p_index, "Running lists index has not been updated");
}
/* END OF TEST*/

TEST(Task, Test_task_resume_stopped_task)
{
   p_task_ctl_t l_task0_under_test = stopped_task_list.p_head;

   Rltos_task_resume((p_dummy_task_t)l_task0_under_test);

   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task0_under_test, state_list), "Task not in running list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task0_under_test, state_list), "Task incorrectly in idle list");
   CHECK_TEXT(!Task_is_in_list(&stopped_task_list, l_task0_under_test, state_list), "Task incorrectly in stopped list");
}
/* END OF TEST*/

TEST(Task, Test_task_resume_stopped_aux_owned_task)
{
   p_task_ctl_t l_prev_running_index = running_task_list.p_index;
   p_task_ctl_t l_task0_under_test = p_current_task_ctl;

   CHECK_TEXT(l_prev_running_index == p_current_task_ctl, "Initial running list index is not the current task");

   /* stop the current task*/
   Task_set_current_wait_on_object(&l_aux_list_under_test, RLTOS_UINT_MAX);

   CHECK_TEXT(!Task_is_in_list(&running_task_list, l_task0_under_test, state_list), "Task incorrectly in running list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task0_under_test, state_list), "Task incorrectly in idle list");
   CHECK_TEXT(Task_is_in_list(&stopped_task_list, l_task0_under_test, state_list), "Task not in stopped list");
   CHECK_TEXT(l_prev_running_index != running_task_list.p_index, "Running lists index has not been updated");

   p_task_ctl_t l_intermediate_running_index = running_task_list.p_index;

   /* Resume should have no effect on this task*/
   Rltos_task_resume((p_dummy_task_t) p_current_task_ctl);

   CHECK_TEXT(!Task_is_in_list(&running_task_list, l_task0_under_test, state_list), "Task incorrectly in running list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task0_under_test, state_list), "Task incorrectly in idle list");
   CHECK_TEXT(Task_is_in_list(&stopped_task_list, l_task0_under_test, state_list), "Task not in stopped list");
   CHECK_TEXT(l_intermediate_running_index == running_task_list.p_index, "Running lists index has been updated");
}
/* END OF TEST*/

TEST(Task, Test_task_resume_idled_task)
{
   Rltos_task_sleep(0x1234U); /* Put a task in the idle task list*/

   p_task_ctl_t l_task0_under_test = idle_task_list.p_head;
   Rltos_task_resume((p_dummy_task_t)l_task0_under_test);

   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task0_under_test, state_list), "Task not in running list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task0_under_test, state_list), "Task incorrectly in idle list");
   CHECK_TEXT(!Task_is_in_list(&stopped_task_list, l_task0_under_test, state_list), "Task incorrectly in stopped list");
}
/* END OF TEST*/

TEST(Task, Test_task_sleep_zero_ticks)
{
   p_task_ctl_t l_prev_cur_task = p_current_task_ctl;

   /* Check 0 value has no effect*/
   Rltos_task_sleep(0U);
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_prev_cur_task, state_list), "Task not in running list when it should be");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_prev_cur_task, state_list), "Task placed in idle list when it shouldn't be");
   CHECK_TEXT(!Task_is_in_list(&stopped_task_list, l_prev_cur_task, state_list), "Task placed in stopped list when it shouldn't be");
}
/* END OF TEST*/

TEST(Task, Test_task_sleep_non_zero_ticks)
{
   p_task_ctl_t l_prev_cur_task = p_current_task_ctl;

   /* Check non-0 value changes task state list correctly*/
   Rltos_task_sleep(1U);
   CHECK_TEXT(!Task_is_in_list(&running_task_list, l_prev_cur_task, state_list), "Task placed in running list when it shouldn't be");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_prev_cur_task, state_list), "Task not placed in idle list when it should be");
   CHECK_TEXT(!Task_is_in_list(&stopped_task_list, l_prev_cur_task, state_list), "Task placed in stopped list when it shouldn't be");
}
/* END OF TEST*/

TEST(Task, Test_task_sleep_zero_maximum_ticks)
{
   p_task_ctl_t l_prev_cur_task = p_current_task_ctl;

   /* Check maximum u_integer value changes task state list correctly*/
   Rltos_task_sleep(RLTOS_UINT_MAX);
   CHECK_TEXT(!Task_is_in_list(&running_task_list, l_prev_cur_task, state_list), "Task placed in running list when it shouldn't be");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_prev_cur_task, state_list), "Task not placed in idle list when it should be");
   CHECK_TEXT(!Task_is_in_list(&stopped_task_list, l_prev_cur_task, state_list), "Task placed in stopped list when it shouldn't be");
}
/* END OF TEST*/





/** Test group for dummy struct sizes*/
TEST_GROUP(Task_sizes){
    void setup(void){
        /* Do Nothing*/
    }
    /* END OF FUNCTION*/

    void teardown(void){
        /* Do Nothing*/
    }
    /* END OF FUNCTION*/
};

TEST(Task_sizes, Test_dummy_task_sizes_equal_to_real)
{
   CHECK_TEXT((sizeof(struct task_ctl_t) == sizeof(dummy_task_t)), "Dummy task struct is not the same size as the real task control struct");
   CHECK_TEXT((sizeof(struct task_list_t) == sizeof(dummy_task_list_t)), "Dummy task list struct is not the same size as the real task list struct");
}
/* END OF TEST*/
