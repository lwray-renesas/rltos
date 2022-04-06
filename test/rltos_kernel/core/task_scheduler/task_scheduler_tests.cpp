/*
 * task_list_tests.c
 *
 *  Created on: 14 Oct 2021
 *      Author: Louis Wray
 */

#include "CppUTest/TestHarness.h"
#include <memory>

extern "C"
{
/* Including the source file allows us to test the internal workings of the module*/
#include "task_scheduler/rltos_task_scheduler.c"
}
#include <array>
#include <random>
#include <iostream>
#include <functional>

#define NO_HOOK_ARG \
   std::function<void()> {}

/** Prototypes*/
/** @brief The dummy task function used for initialisation of tasks*/
void Dummy_task_func(void);

/** @brief Helper function used to simulate the scheduler running
 * @param[in] num_ticks - Number of ticks to run the scheduler simulation for.
 * @param[in] start_hook - Optional hook called at the beginning of the simulation.
 * @param[in] ts_hook - Optional hook called between the tick increment and the context switch functions.
 * @param[in] end_hook - Optional hook called at the end of the scheduler loop.
 * @param[in] finish_hook - Optional hook called at the end of the simulation.
 * @param[in] should_inc_tick - Inidcate whether or not the simulation should run the increment tick function.
 */
void Simulate_scheduler(const rltos_uint num_ticks,
                        std::function<void()> const &start_hook,
                        std::function<void()> const &ts_hook,
                        std::function<void()> const &end_hook,
                        std::function<void()> const &finish_hook,
                        const bool should_inc_tick);

/** Test group for task list initialser functions
 * All tests have acccess to a kernel with 100 tasks each with 32 units of stack - pointing to the dummy task function.
*/
TEST_GROUP(TaskSchedulerTestGroup)
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

void Dummy_task_func(void)
{
   /* Do nothing*/
}
/* END OF FUNCTION*/

void Simulate_scheduler(const rltos_uint num_ticks,
                        std::function<void()> const &start_hook,
                        std::function<void()> const &ts_hook,
                        std::function<void()> const &end_hook,
                        std::function<void()> const &finish_hook,
                        const bool should_inc_tick)
{
   rltos_uint l_num_ticks = num_ticks;

   while (l_num_ticks > 0)
   {
      if (start_hook)
      {
         start_hook();
      }
      if (should_inc_tick)
      {
         Rltos_scheduler_tick_inc();
      }

      if (ts_hook)
      {
         ts_hook();
      }

      Rltos_scheduler_switch_context();

      if (end_hook)
      {
         end_hook();
      }

      --l_num_ticks;
   }

   if (finish_hook)
   {
      finish_hook();
   }
}
/* END OF FUNCTION*/

TEST(TaskSchedulerTestGroup, Test_SchedulerInit)
{
   CHECK_TEXT(p_current_task_ctl != NULL, "Scheduler incorrectly initialised");
   CHECK_TEXT(running_task_list.p_head != NULL, "Scheduler incorrectly initialised");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_IdleTaskInit_Ok)
{
   CHECK_TEXT((idle_task_ctl.stored_sp == idle_task_stack), "Idle task stack pointer is not initialised as expected");
   CHECK_TEXT((idle_task_ctl.p_task_func == Rltos_idle_thread), "Idle task function pointer is not initialised as expected");
   CHECK_TEXT((idle_task_ctl.p_next_tctl[aux_list] == NULL), "Idle task should not be owned by an AUX list");
   CHECK_TEXT((idle_task_ctl.p_prev_tctl[aux_list] == NULL), "Idle task should not be owned by an AUX list");
   CHECK_TEXT((idle_task_ctl.p_owners[aux_list] == NULL), "Idle task should not be owned by an AUX list");
   CHECK_TEXT((idle_task_ctl.p_owners[state_list] == &running_task_list), "Idle task should be owned by the running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, &idle_task_ctl, state_list), "Idle task should be owned by the running list");
}

TEST(TaskSchedulerTestGroup, Test_TaskListInit_ValuesOk)
{
   CHECK_TEXT((l_aux_list_under_test.p_head == NULL), "Task_list_init failed to initialise head pointer");
   CHECK_TEXT((l_aux_list_under_test.p_index == NULL), "Task_list_init failed to initialise index pointer");
   CHECK_TEXT((l_aux_list_under_test.size == 0U), "Task_list_init failed to initialise size");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskAppend)
{
   p_task_ctl_t l_task0_under_test = (p_task_ctl_t)&(group_tasks_under_test[0]);
   p_task_ctl_t l_task1_under_test = (p_task_ctl_t)&(group_tasks_under_test[1]);
   p_task_ctl_t l_task2_under_test = (p_task_ctl_t)&(group_tasks_under_test[2]);
   p_task_ctl_t l_task3_under_test = (p_task_ctl_t)&(group_tasks_under_test[3]);

   Task_append_to_list(&l_aux_list_under_test, l_task0_under_test, aux_list);
   CHECK_TEXT(l_aux_list_under_test.p_head == l_task0_under_test, "List append failed, p_head not set correctly");
   CHECK_TEXT(l_aux_list_under_test.p_index == l_task0_under_test, "List append failed, p_index not set correctly");
   CHECK_TEXT(l_aux_list_under_test.size == 1U, "List append failed, size not set correctly");

   Task_append_to_list(&l_aux_list_under_test, l_task1_under_test, aux_list);
   CHECK_TEXT(l_aux_list_under_test.p_head == l_task0_under_test, "List append failed, p_head not set correctly");
   CHECK_TEXT(l_aux_list_under_test.p_index == l_task0_under_test, "List append failed, p_index not set correctly");
   CHECK_TEXT(l_aux_list_under_test.size == 2U, "List append failed, size not set correctly");
   CHECK_TEXT(l_aux_list_under_test.p_head->p_prev_tctl[state_list] == l_task1_under_test, "List append failed, p_head previous task not set correctly");
   CHECK_TEXT(l_task1_under_test->p_next_tctl[state_list] == l_aux_list_under_test.p_head, "List append failed, task appended next task not set correctly");

   Task_append_to_list(&l_aux_list_under_test, l_task2_under_test, aux_list);
   CHECK_TEXT(l_aux_list_under_test.p_head == l_task0_under_test, "List append failed, p_head not set correctly");
   CHECK_TEXT(l_aux_list_under_test.p_index == l_task0_under_test, "List append failed, p_index not set correctly");
   CHECK_TEXT(l_aux_list_under_test.size == 3U, "List append failed, size not set correctly");
   CHECK_TEXT(l_aux_list_under_test.p_head->p_prev_tctl[state_list] == l_task2_under_test, "List append failed, p_head previous task not set correctly");
   CHECK_TEXT(l_task2_under_test->p_next_tctl[state_list] == l_aux_list_under_test.p_head, "List append failed, task appended next task not set correctly");
   CHECK_TEXT(l_task2_under_test->p_prev_tctl[state_list] == l_task1_under_test, "List append failed, task appended prev task not set correctly");
   CHECK_TEXT(l_task1_under_test->p_next_tctl[state_list] == l_task2_under_test, "List append failed, task previously appended next task not set correctly");

   Task_append_to_list(&l_aux_list_under_test, l_task3_under_test, aux_list);
   CHECK_TEXT(l_aux_list_under_test.p_head == l_task0_under_test, "List append failed, p_head not set correctly");
   CHECK_TEXT(l_aux_list_under_test.p_index == l_task0_under_test, "List append failed, p_index not set correctly");
   CHECK_TEXT(l_aux_list_under_test.size == 4U, "List append failed, size not set correctly");
   CHECK_TEXT(l_aux_list_under_test.p_head->p_prev_tctl[state_list] == l_task3_under_test, "List append failed, p_head previous task not set correctly");
   CHECK_TEXT(l_task3_under_test->p_next_tctl[state_list] == l_aux_list_under_test.p_head, "List append failed, task appended next task not set correctly");
   CHECK_TEXT(l_task3_under_test->p_prev_tctl[state_list] == l_task2_under_test, "List append failed, task appended prev task not set correctly");
   CHECK_TEXT(l_task2_under_test->p_next_tctl[state_list] == l_task3_under_test, "List append failed, task previously appended next task not set correctly");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskRemove)
{
   p_task_ctl_t l_task0_under_test = (p_task_ctl_t)&(group_tasks_under_test[0]);

   Task_append_to_list(&l_aux_list_under_test, l_task0_under_test, aux_list);

   Task_remove_from_list(&l_aux_list_under_test, l_task0_under_test, aux_list);

   CHECK_TEXT((l_aux_list_under_test.p_head == NULL), "Task_remove_from_list did not reset p_head");
   CHECK_TEXT((l_aux_list_under_test.p_index == NULL), "Task_remove_from_list did not reset p_index");
   CHECK_TEXT((l_aux_list_under_test.size == 0U), "Task_remove_from_list did not reset size");
   CHECK_TEXT((l_task0_under_test->p_next_tctl[aux_list] == NULL), "Task_remove_from_list did not reset tasks next task pointer");
   CHECK_TEXT((l_task0_under_test->p_prev_tctl[aux_list] == NULL), "Task_remove_from_list did not reset tasks previous task pointer");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskInsert)
{
   /* Verify the running list is ordered in priority from highest (smallest number) to lowest (largest number)*/
   p_task_ctl_t tsk_indexer = running_task_list.p_head->p_next_tctl[state_list];
   while (tsk_indexer != running_task_list.p_head)
   {
      /* Checks if the current tasks sorting value is larger than or equal to the one before it*/
      CHECK_TEXT(tsk_indexer->sorting_values[state_list] >= tsk_indexer->p_prev_tctl[state_list]->sorting_values[state_list], "Running list in wrong order!");
      tsk_indexer = tsk_indexer->p_next_tctl[state_list];
   }

   /* Verify the idle list is ordered in priority from highest (smallest number) to lowest (largest number)*/
   tsk_indexer = idle_task_list.p_head->p_next_tctl[state_list];
   while (tsk_indexer != idle_task_list.p_head)
   {
      /* Checks if the current tasks sorting value is larger than or equal to the one before it*/
      CHECK_TEXT(tsk_indexer->sorting_values[state_list] >= tsk_indexer->p_prev_tctl[state_list]->sorting_values[state_list], "Idle list in wrong order!");
      tsk_indexer = tsk_indexer->p_next_tctl[state_list];
   }
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetRunning_ListsOK_IdleOnly)
{
   p_task_ctl_t l_task0_under_test = idle_task_list.p_head;

   Task_set_running(l_task0_under_test);

   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task0_under_test, state_list), "Task failed to be removed from idle list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task0_under_test, state_list), "Task failed to be added to running list");
   CHECK_TEXT(RLTOS_UINT_MAX == rltos_next_idle_ready_tick, "Failed to update next idle ready tick count");
   CHECK_TEXT(RLTOS_UINT_MAX == rltos_next_idle_ready_wrap_count, "Failed to update next idle ready wrap count");
}
/* END OF TEST*/
