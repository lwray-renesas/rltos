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

#define NO_HOOK_ARG std::function<void()>{}

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

/** Test group for task list initialser functions*/
TEST_GROUP(TaskSchedulerTestGroup){
    void setup(void){

    }
    /* END OF FUNCTION*/

    void teardown(void){

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

TEST(TaskSchedulerTestGroup, Test_SchedulerInitAndDeinit_ControlsIdleTaskOk)
{
   Task_scheduler_init();
   CHECK_TEXT(p_current_task_ctl == &idle_task_ctl, "Scheduler incorrectly initialised");
   CHECK_TEXT(running_task_list.p_head == &idle_task_ctl, "Scheduler incorrectly initialised");
   Task_scheduler_deinit();
   CHECK_TEXT(p_current_task_ctl == NULL, "Scheduler incorrectly deinitialised");
   CHECK_TEXT(running_task_list.p_head == NULL, "Scheduler incorrectly deinitialised");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskInit_ValuesOk)
{
   /* Create dummy task and stack*/
   std::unique_ptr<struct task_ctl_t> l_dummy_task = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_dummy_stack = std::make_unique<stack_type[]>(32);

   /* Create testable task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test = std::make_unique<stack_type[]>(32);

   Task_init(l_dummy_task.get(), l_dummy_stack.get(), &Dummy_task_func, 0U, true);
   Task_init(l_task_under_test.get(), l_stack_under_test.get(), &Dummy_task_func, 0U, true);

   CHECK_TEXT((l_task_under_test->stored_sp == l_stack_under_test.get()), "Task_init failed to initialise stack pointer");
   CHECK_TEXT((l_task_under_test->p_task_func == Dummy_task_func), "Task_init failed to initialise pointer to task function correctly");
   CHECK_TEXT((l_task_under_test->p_next_tctl[state_list] == l_dummy_task.get()), "Task_list_init failed to initialise tasks next task STATE list");
   CHECK_TEXT((l_task_under_test->p_prev_tctl[state_list] == l_dummy_task.get()), "Task_list_init failed to initialise tasks prev task STATE list");
   CHECK_TEXT((l_task_under_test->p_next_tctl[aux_list] == NULL), "Task_init failed to initialise tasks next task AUX list");
   CHECK_TEXT((l_task_under_test->p_prev_tctl[aux_list] == NULL), "Task_init failed to initialise tasks prev task AUX list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test.get(), state_list), "Task_init failed to add task to the running list");
   CHECK_TEXT((l_task_under_test->p_owners[aux_list] == NULL), "Task_init editted aux list owner without cause");

   Task_deinit(l_dummy_task.get());
   Task_deinit(l_task_under_test.get());

   /* Retest but add to idle lsit instead*/
   Task_init(l_task_under_test.get(), l_stack_under_test.get(), &Dummy_task_func, 0U, false);
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test.get(), state_list), "Task_init failed to add task to the idle list");

   Task_deinit(l_task_under_test.get());
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskDeinit_ValuesOk)
{
   /* Create testable task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test = std::make_unique<stack_type[]>(32);

   Task_init(l_task_under_test.get(), l_stack_under_test.get(), &Dummy_task_func, 0U, true);
   Task_deinit(l_task_under_test.get());

   CHECK_TEXT((l_task_under_test->stored_sp == NULL), "Task_deinit failed to deinitialise stack pointer");
   CHECK_TEXT((l_task_under_test->p_task_func == NULL), "Task_deinit failed to deinitialise pointer to task function");
   CHECK_TEXT((l_task_under_test->p_next_tctl[state_list] == NULL), "Task_list_init failed to initialise tasks next task STATE list");
   CHECK_TEXT((l_task_under_test->p_prev_tctl[state_list] == NULL), "Task_list_init failed to initialise tasks prev task STATE list");
   CHECK_TEXT((l_task_under_test->p_next_tctl[aux_list] == NULL), "Task_list_init failed to initialise tasks next task AUX list");
   CHECK_TEXT((l_task_under_test->p_prev_tctl[aux_list] == NULL), "Task_list_init failed to initialise tasks prev task AUX list");
   CHECK_TEXT((l_task_under_test->p_owners[state_list] == NULL), "Task_init failed to add task to the running list");
   CHECK_TEXT((l_task_under_test->p_owners[aux_list] == NULL), "Task_init editted aux list owner without cause");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskListInit_ValuesOk)
{
   /* Create testable list*/
   std::unique_ptr<struct task_list_t> l_list_under_test = std::make_unique<struct task_list_t>();

   Task_list_init(l_list_under_test.get());

   CHECK_TEXT((l_list_under_test->p_head == NULL), "Task_list_init failed to initialise head pointer");
   CHECK_TEXT((l_list_under_test->p_index == NULL), "Task_list_init failed to initialise index pointer");
   CHECK_TEXT((l_list_under_test->size == 0U), "Task_list_init failed to initialise size");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskAppend)
{
   /* Create testable list*/
   std::unique_ptr<struct task_list_t> l_list_under_test = std::make_unique<struct task_list_t>();

   /* Create testable task & append to list*/
   std::unique_ptr<struct task_ctl_t> l_task0_under_test = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<struct task_ctl_t> l_task1_under_test = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<struct task_ctl_t> l_task2_under_test = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<struct task_ctl_t> l_task3_under_test = std::make_unique<struct task_ctl_t>();

   Task_append_to_list(l_list_under_test.get(), l_task0_under_test.get(), state_list);
   CHECK_TEXT(l_list_under_test->p_head == l_task0_under_test.get(), "List append failed, p_head not set correctly");
   CHECK_TEXT(l_list_under_test->p_index == l_task0_under_test.get(), "List append failed, p_index not set correctly");
   CHECK_TEXT(l_list_under_test->size == 1U, "List append failed, size not set correctly");

   Task_append_to_list(l_list_under_test.get(), l_task1_under_test.get(), state_list);
   CHECK_TEXT(l_list_under_test->p_head == l_task0_under_test.get(), "List append failed, p_head not set correctly");
   CHECK_TEXT(l_list_under_test->p_index == l_task0_under_test.get(), "List append failed, p_index not set correctly");
   CHECK_TEXT(l_list_under_test->size == 2U, "List append failed, size not set correctly");
   CHECK_TEXT(l_list_under_test->p_head->p_prev_tctl[state_list] == l_task1_under_test.get(), "List append failed, p_head previous task not set correctly");
   CHECK_TEXT(l_task1_under_test->p_next_tctl[state_list] == l_list_under_test->p_head, "List append failed, task appended next task not set correctly");

   Task_append_to_list(l_list_under_test.get(), l_task2_under_test.get(), state_list);
   CHECK_TEXT(l_list_under_test->p_head == l_task0_under_test.get(), "List append failed, p_head not set correctly");
   CHECK_TEXT(l_list_under_test->p_index == l_task0_under_test.get(), "List append failed, p_index not set correctly");
   CHECK_TEXT(l_list_under_test->size == 3U, "List append failed, size not set correctly");
   CHECK_TEXT(l_list_under_test->p_head->p_prev_tctl[state_list] == l_task2_under_test.get(), "List append failed, p_head previous task not set correctly");
   CHECK_TEXT(l_task2_under_test->p_next_tctl[state_list] == l_list_under_test->p_head, "List append failed, task appended next task not set correctly");
   CHECK_TEXT(l_task2_under_test->p_prev_tctl[state_list] == l_task1_under_test.get(), "List append failed, task appended prev task not set correctly");
   CHECK_TEXT(l_task1_under_test->p_next_tctl[state_list] == l_task2_under_test.get(), "List append failed, task previously appended next task not set correctly");

   Task_append_to_list(l_list_under_test.get(), l_task3_under_test.get(), state_list);
   CHECK_TEXT(l_list_under_test->p_head == l_task0_under_test.get(), "List append failed, p_head not set correctly");
   CHECK_TEXT(l_list_under_test->p_index == l_task0_under_test.get(), "List append failed, p_index not set correctly");
   CHECK_TEXT(l_list_under_test->size == 4U, "List append failed, size not set correctly");
   CHECK_TEXT(l_list_under_test->p_head->p_prev_tctl[state_list] == l_task3_under_test.get(), "List append failed, p_head previous task not set correctly");
   CHECK_TEXT(l_task3_under_test->p_next_tctl[state_list] == l_list_under_test->p_head, "List append failed, task appended next task not set correctly");
   CHECK_TEXT(l_task3_under_test->p_prev_tctl[state_list] == l_task2_under_test.get(), "List append failed, task appended prev task not set correctly");
   CHECK_TEXT(l_task2_under_test->p_next_tctl[state_list] == l_task3_under_test.get(), "List append failed, task previously appended next task not set correctly");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskRemove)
{
   /* Create testable list*/
   std::unique_ptr<struct task_list_t> l_list_under_test = std::make_unique<struct task_list_t>();

   /* Create testable task & append to list*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test = std::make_unique<struct task_ctl_t>();

   Task_append_to_list(l_list_under_test.get(), l_task_under_test.get(), aux_list);

   Task_remove_from_list(l_list_under_test.get(), l_task_under_test.get(), aux_list);

   CHECK_TEXT((l_list_under_test->p_head == NULL), "Task_remove_from_list did not reset p_head");
   CHECK_TEXT((l_list_under_test->p_index == NULL), "Task_remove_from_list did not reset p_index");
   CHECK_TEXT((l_list_under_test->size == 0U), "Task_remove_from_list did not reset size");
   CHECK_TEXT((l_task_under_test->p_next_tctl[aux_list] == NULL), "Task_remove_from_list did not reset tasks next task pointer");
   CHECK_TEXT((l_task_under_test->p_prev_tctl[aux_list] == NULL), "Task_remove_from_list did not reset tasks previous task pointer");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskIsInList)
{
   /* Create testable list*/
   std::unique_ptr<struct task_list_t> l_list_under_test = std::make_unique<struct task_list_t>();

   /* Create testable task & append to list*/
   std::unique_ptr<struct task_ctl_t> l_task0_under_test = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<struct task_ctl_t> l_task1_under_test = std::make_unique<struct task_ctl_t>();

   Task_append_to_list(l_list_under_test.get(), l_task0_under_test.get(), aux_list);

   CHECK_TEXT(Task_is_in_list(l_list_under_test.get(), l_task0_under_test.get(), aux_list), "task not detected in aux task list");
   CHECK_TEXT(!Task_is_in_list(l_list_under_test.get(), l_task1_under_test.get(), aux_list), "task detected in aux task list");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskInsert)
{
   const size_t tasks_to_add = 100;

   /* Create testable list*/
   std::unique_ptr<struct task_list_t> l_list_under_test = std::make_unique<struct task_list_t>();

   /* Create testable tasks*/
   std::array<struct task_ctl_t, tasks_to_add> l_tasks_under_test;

   /* Create random number generation function*/
   std::random_device rd;
   std::mt19937 g(rd());
   std::uniform_int_distribution<> distrib(0);

   /* set the priorities of each task*/
   for (struct task_ctl_t &tsk : l_tasks_under_test)
   {
      tsk.priority = static_cast<rltos_uint>(distrib(g));
   }

   /* Shuffle the list*/
   std::shuffle(l_tasks_under_test.begin(), l_tasks_under_test.end(), g);

   /* Add the tasks (now in random order)*/
   for (struct task_ctl_t &tsk : l_tasks_under_test)
   {
      Task_insert_in_list(l_list_under_test.get(), &tsk, state_list, tsk.priority);
   }

   /* Verify the list is now ordered*/
   p_task_ctl_t tsk_indexer = l_list_under_test->p_head->p_next_tctl[state_list];
   while (tsk_indexer != l_list_under_test->p_head)
   {
      /* Checks if the current tasks sorting value is larger than or equal to the one before it*/
      CHECK_TEXT(tsk_indexer->sorting_values[state_list] >= tsk_indexer->p_prev_tctl[state_list]->sorting_values[state_list], "List in wrong order!");
      tsk_indexer = tsk_indexer->p_next_tctl[state_list];
   }
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskInsert_SameSortValues)
{
   const size_t tasks_to_add = 100;

   /* Create testable list*/
   std::unique_ptr<struct task_list_t> l_list_under_test = std::make_unique<struct task_list_t>();

   /* Create testable tasks*/
   std::array<struct task_ctl_t, tasks_to_add> l_tasks_under_test;

   /* Create random number generation function*/
   std::random_device random_dev;
   std::mt19937 random_gen(random_dev());
   std::uniform_int_distribution<> distrib(0);

   const rltos_uint tsk_priority = static_cast<rltos_uint>(distrib(random_gen));

   /* set the priorities of each task*/
   for (struct task_ctl_t &tsk : l_tasks_under_test)
   {
      tsk.priority = tsk_priority;
   }

   /* Shuffle the list*/
   std::shuffle(l_tasks_under_test.begin(), l_tasks_under_test.end(), random_gen);

   /* Add the tasks (now in random order)*/
   for (struct task_ctl_t &tsk : l_tasks_under_test)
   {
      Task_insert_in_list(l_list_under_test.get(), &tsk, state_list, tsk.priority);
   }

   /* Verify the list is now ordered in the order the same priority tasks were added in*/
   rltos_uint index_arr = 0U;
   p_task_ctl_t tsk_indexer = l_list_under_test->p_head;
   while (tsk_indexer->p_next_tctl[state_list] != l_list_under_test->p_head)
   {
      /* Checks if the current task order in the list, is the same as the index of the task in the task array*/
      CHECK_TEXT(&l_tasks_under_test[index_arr] == tsk_indexer, "List in wrong order!");
      tsk_indexer = tsk_indexer->p_next_tctl[state_list];
      ++index_arr;
   }

   /* Check last value*/
   CHECK_TEXT(&l_tasks_under_test[index_arr] == tsk_indexer, "List in wrong order!");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetRunning_ListsOK_IdleOnly)
{
   /* Create testable task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test = std::make_unique<stack_type[]>(32);

   Task_init(l_task_under_test.get(), l_stack_under_test.get(), &Dummy_task_func, 0U, false);

   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test.get(), state_list), "Task incorrectly intitialised - should be in idle list");

   Task_set_running(l_task_under_test.get());

   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task_under_test.get(), state_list), "Task failed to be removed from idle list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test.get(), state_list), "Task failed to be added to running list");
   CHECK_TEXT(RLTOS_UINT_MAX == rltos_next_idle_ready_tick, "Failed to update next idle ready tick count");
   CHECK_TEXT(RLTOS_UINT_MAX == rltos_next_idle_ready_wrap_count, "Failed to update next idle ready wrap count");

   /* Tear down*/
   Task_deinit(l_task_under_test.get());
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetRunning_ListsOK_IdleOnly_2tasks)
{
   /* Create testable task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test0 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test0 = std::make_unique<stack_type[]>(32);

   std::unique_ptr<struct task_ctl_t> l_task_under_test1 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test1 = std::make_unique<stack_type[]>(32);

   Task_init(l_task_under_test0.get(), l_stack_under_test0.get(), &Dummy_task_func, 0U, false);
   Task_init(l_task_under_test1.get(), l_stack_under_test1.get(), &Dummy_task_func, 0U, false);

   /* Random values to verify the idle counters are updated*/
   l_task_under_test1->idle_ready_time = 0x1234U;
   l_task_under_test1->idle_wrap_count = 0x5678U;

   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test0.get(), state_list), "Task incorrectly intitialised - should be in idle list");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test1.get(), state_list), "Task incorrectly intitialised - should be in idle list");

   Task_set_running(l_task_under_test0.get());

   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test0.get(), state_list), "Task failed to be added to running list");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test1.get(), state_list), "Task incorrectly intiialised - should be in idle list");
   /* Check the idle counters in the scheduler have been updated*/
   CHECK_TEXT(l_task_under_test1->idle_ready_time == rltos_next_idle_ready_tick, "Failed to update next idle ready tick count");
   CHECK_TEXT(l_task_under_test1->idle_wrap_count == rltos_next_idle_ready_wrap_count, "Failed to update next idle ready wrap count");

   /* Tear down*/
   Task_deinit(l_task_under_test0.get());
   Task_deinit(l_task_under_test1.get());
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetRunning_ListsOK_IdleAndAux)
{
   /* Create testable task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test = std::make_unique<stack_type[]>(32);

   /* Create testable auxiallary list*/
   std::unique_ptr<struct task_list_t> l_aux_list_under_test = std::make_unique<struct task_list_t>();

   Task_list_init(l_aux_list_under_test.get());

   Task_init(l_task_under_test.get(), l_stack_under_test.get(), &Dummy_task_func, 0U, false);
   Task_append_to_list(l_aux_list_under_test.get(), l_task_under_test.get(), aux_list);

   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test.get(), state_list), "Task incorrectly intitialised - should be in idle list");
   CHECK_TEXT(Task_is_in_list(l_aux_list_under_test.get(), l_task_under_test.get(), aux_list), "Task incorrectly intitialised - should be in aux list");

   Task_set_running(l_task_under_test.get());

   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task_under_test.get(), state_list), "Task failed to be removed from idle list");
   CHECK_TEXT(!Task_is_in_list(l_aux_list_under_test.get(), l_task_under_test.get(), aux_list), "Task failed to be removed from aux list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test.get(), state_list), "Task failed to be added to running list");

   /* Tear down*/
   Task_deinit(l_task_under_test.get());
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetCurrentIdle_ListsOK)
{
   /* Create testable task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test0 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test0 = std::make_unique<stack_type[]>(32);

   std::unique_ptr<struct task_ctl_t> l_task_under_test1 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test1 = std::make_unique<stack_type[]>(32);

   Task_init(l_task_under_test0.get(), l_stack_under_test0.get(), &Dummy_task_func, 0U, true);
   Task_init(l_task_under_test1.get(), l_stack_under_test1.get(), &Dummy_task_func, 1U, true);

   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test0.get(), state_list), "Task incorrectly intitialised - should be in running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test1.get(), state_list), "Task incorrectly intitialised - should be in running list");
   CHECK_TEXT(running_task_list.p_head == l_task_under_test0.get(), "head of running task list is not as expected");

   Task_scheduler_init();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test0.get(), "curent task is not as expected");

   /* This call will always be followed by a yield*/
   Task_set_current_idle(0x1234U);

   Rltos_scheduler_switch_context();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test1.get(), "curent task is not as expected");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test0.get(), state_list), "Task incorrectly set - should be in running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test1.get(), state_list), "Task erroneously changed - should be in running list");
   CHECK_TEXT(l_task_under_test0->idle_ready_time == rltos_next_idle_ready_tick, "Failed to update next idle ready tick count");
   CHECK_TEXT(l_task_under_test0->idle_wrap_count == rltos_next_idle_ready_wrap_count, "Failed to update next idle ready wrap count");

   Task_deinit(l_task_under_test0.get());
   Task_deinit(l_task_under_test1.get());
   Task_scheduler_deinit();
   rltos_next_idle_ready_wrap_count = 0U;
   rltos_next_idle_ready_tick = RLTOS_UINT_MAX;
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetCurrentIdle_ListsOK_WrapAround)
{
   /* Create testable task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test0 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test0 = std::make_unique<stack_type[]>(32);

   std::unique_ptr<struct task_ctl_t> l_task_under_test1 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test1 = std::make_unique<stack_type[]>(32);

   Task_init(l_task_under_test0.get(), l_stack_under_test0.get(), &Dummy_task_func, 0U, true);
   Task_init(l_task_under_test1.get(), l_stack_under_test1.get(), &Dummy_task_func, 1U, true);

   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test0.get(), state_list), "Task incorrectly intitialised - should be in running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test1.get(), state_list), "Task incorrectly intitialised - should be in running list");
   CHECK_TEXT(running_task_list.p_head == l_task_under_test0.get(), "head of running task list is not as expected");

   Task_scheduler_init();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test0.get(), "curent task is not as expected");

   rltos_system_tick = RLTOS_UINT_MAX-1U;
   /* This call will always be followed by a yield*/
   Task_set_current_idle(0x1234U);

   Rltos_scheduler_switch_context();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test1.get(), "curent task is not as expected");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test0.get(), state_list), "Task incorrectly set - should be in running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test1.get(), state_list), "Task erroneously changed - should be in running list");
   CHECK_TEXT(l_task_under_test0->idle_ready_time == rltos_next_idle_ready_tick, "Failed to update next idle ready tick count");
   CHECK_TEXT(l_task_under_test0->idle_wrap_count == rltos_next_idle_ready_wrap_count, "Failed to update next idle ready wrap count");
   CHECK_TEXT(((RLTOS_UINT_MAX-1U) + 0x1234U) == rltos_next_idle_ready_tick, "Failed to update next idle ready tick count");
   CHECK_TEXT(1U == rltos_next_idle_ready_wrap_count, "Failed to update next idle ready wrap count");

   Task_deinit(l_task_under_test0.get());
   Task_deinit(l_task_under_test1.get());
   Task_scheduler_deinit();
   rltos_next_idle_ready_wrap_count = 0U;
   rltos_next_idle_ready_tick = RLTOS_UINT_MAX;
   rltos_system_tick = 0U;
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetCurrentIdle_ticksUpdatedOK)
{
   /* Create testable task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test0 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test0 = std::make_unique<stack_type[]>(32);

   std::unique_ptr<struct task_ctl_t> l_task_under_test1 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test1 = std::make_unique<stack_type[]>(32);

   std::unique_ptr<struct task_ctl_t> l_task_under_test2 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test2 = std::make_unique<stack_type[]>(32);

   std::unique_ptr<struct task_ctl_t> l_task_under_test3 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test3 = std::make_unique<stack_type[]>(32);

   std::unique_ptr<struct task_ctl_t> l_task_under_test4 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test4 = std::make_unique<stack_type[]>(32);

   Task_init(l_task_under_test0.get(), l_stack_under_test0.get(), &Dummy_task_func, 0U, true);
   Task_init(l_task_under_test1.get(), l_stack_under_test1.get(), &Dummy_task_func, 1U, true);
   Task_init(l_task_under_test2.get(), l_stack_under_test2.get(), &Dummy_task_func, 2U, true);
   Task_init(l_task_under_test3.get(), l_stack_under_test3.get(), &Dummy_task_func, 3U, true);
   Task_init(l_task_under_test4.get(), l_stack_under_test4.get(), &Dummy_task_func, 4U, true);

   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test0.get(), state_list), "Task incorrectly intitialised - should be in running list");
   CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test1.get(), state_list), "Task incorrectly intitialised - should be in running list");
   CHECK_TEXT(running_task_list.p_head == l_task_under_test0.get(), "head of running task list is not as expected");

   Task_scheduler_init();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test0.get(), "curent task is not as expected");

   Task_set_current_idle(0x1235U);
   Rltos_scheduler_switch_context();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test1.get(), "curent task is not as expected");
   CHECK_TEXT(rltos_next_idle_ready_tick == 0x1235U, "tick count incorrect");
   CHECK_TEXT(rltos_next_idle_ready_wrap_count == 0U, "wrap count incorrect");

   Task_set_current_idle(0x1234U);
   Rltos_scheduler_switch_context();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test2.get(), "curent task is not as expected");
   CHECK_TEXT(rltos_next_idle_ready_tick == 0x1234U, "tick count incorrect");
   CHECK_TEXT(rltos_next_idle_ready_wrap_count == 0U, "wrap count incorrect");

   Task_set_current_idle(0x1235U);
   Rltos_scheduler_switch_context();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test3.get(), "curent task is not as expected");
   CHECK_TEXT(rltos_next_idle_ready_tick == 0x1234U, "tick count incorrect");
   CHECK_TEXT(rltos_next_idle_ready_wrap_count == 0U, "wrap count incorrect");

   /* Manually increase the ready wrapcount to test we overwrite this with the task being idles for less time (which does not force a tick wrap around)*/
   rltos_next_idle_ready_wrap_count += 1U;

   Task_set_current_idle(RLTOS_UINT_MAX);
   Rltos_scheduler_switch_context();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test4.get(), "curent task is not as expected");
   CHECK_TEXT(rltos_next_idle_ready_tick == RLTOS_UINT_MAX, "tick count incorrect");
   CHECK_TEXT(rltos_next_idle_ready_wrap_count == 0U, "wrap count incorrect");

   /* Now force a wrap around a verify the idle task has not been over written*/
   rltos_system_tick = RLTOS_UINT_MAX;

   Task_set_current_idle(10U);
   Rltos_scheduler_switch_context();

   CHECK_TEXT(p_current_task_ctl == &idle_task_ctl, "curent task is not as expected");
   CHECK_TEXT(rltos_next_idle_ready_tick == RLTOS_UINT_MAX, "tick count incorrect");
   CHECK_TEXT(rltos_next_idle_ready_wrap_count == 0U, "wrap count incorrect");

   Task_deinit(l_task_under_test0.get());
   Task_deinit(l_task_under_test1.get());
   Task_deinit(l_task_under_test2.get());
   Task_deinit(l_task_under_test3.get());
   Task_deinit(l_task_under_test4.get());

   Task_scheduler_deinit();
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetCurrentWaitOnObject_ListsOK)
{
   std::unique_ptr<struct task_ctl_t> l_task_under_test0 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test0 = std::make_unique<stack_type[]>(32);

   std::unique_ptr<struct task_list_t> l_task_list0 = std::make_unique<struct task_list_t>();

   Task_init(l_task_under_test0.get(), l_stack_under_test0.get(), &Dummy_task_func, 0U, true);

   Task_scheduler_init();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test0.get(), "curent task is not as expected");

   Task_set_current_wait_on_object(l_task_list0.get(), 0x1234U);

   CHECK_TEXT(Task_is_in_list(l_task_list0.get(), l_task_under_test0.get(), aux_list), "task is not in aux list as expected");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test0.get(), state_list), "task is not in idle list as expected");

   Task_set_running(l_task_under_test0.get());

   CHECK_TEXT(!Task_is_in_list(l_task_list0.get(), l_task_under_test0.get(), aux_list), "task is still in aux list");
   CHECK_TEXT(!Task_is_in_list(&idle_task_list, l_task_under_test0.get(), state_list), "task is still in idle list");

   Task_deinit(l_task_under_test0.get());
   Task_scheduler_deinit();
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_SchedulerTickInc)
{
   std::unique_ptr<struct task_ctl_t> l_task_under_test0 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack_under_test0 = std::make_unique<stack_type[]>(32);

   std::unique_ptr<struct task_list_t> l_task_list0 = std::make_unique<struct task_list_t>();

   Task_init(l_task_under_test0.get(), l_stack_under_test0.get(), &Dummy_task_func, 0U, true);

   Task_scheduler_init();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test0.get(), "curent task is not as expected");

   Task_set_current_wait_on_object(l_task_list0.get(), RLTOS_UINT_MAX);

   CHECK_TEXT(Task_is_in_list(l_task_list0.get(), l_task_under_test0.get(), aux_list), "task is not in aux list as expected");
   CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test0.get(), state_list), "task is not in idle list as expected");

   /* Set variable ready for wrap around (speeds up test)*/
   rltos_system_tick = RLTOS_UINT_MAX - 0xFFFFU;

   /* simluate up to wrap around*/
   Simulate_scheduler(0xFFFFU, [&](){ 
      CHECK_TEXT(Task_is_in_list(l_task_list0.get(), l_task_under_test0.get(), aux_list), "task is not in aux list as expected");
      CHECK_TEXT(Task_is_in_list(&idle_task_list, l_task_under_test0.get(), state_list), "task is not in idle list as expected");
      CHECK_TEXT(Task_is_in_list(&running_task_list, &idle_task_ctl, state_list), "idle task is not running as expected");
      CHECK_TEXT(running_task_list.p_index == &idle_task_ctl, "Idle task not running when expected");
   }, [&](){}, [&](){}, [&](){
      CHECK_TEXT(!Task_is_in_list(l_task_list0.get(), l_task_under_test0.get(), aux_list), "task is not in aux list as expected");
      CHECK_TEXT(Task_is_in_list(&running_task_list, l_task_under_test0.get(), state_list), "task is not in idle list as expected");
      CHECK_TEXT(Task_is_in_list(&running_task_list, &idle_task_ctl, state_list), "idle task is not running as expected");
      CHECK_TEXT((p_current_task_ctl == l_task_under_test0.get()) && (running_task_list.p_index == l_task_under_test0.get()), "Temp task not running when expected");
      }, true);

   /* Check wrap around*/
   Simulate_scheduler(1U, [=](){}, [=](){}, [&](){}, [=](){
      CHECK_TEXT(rltos_wrap_count == 1U,"rtlos wrap around did not occur");
   }, true);

    Task_deinit(l_task_under_test0.get());

    Task_scheduler_deinit();
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_SchedulerSwitchContext)
{
   const size_t tasks_to_add = 10U;
   size_t scheduler_iterations = 1000U;

   /* Create testable tasks*/
   std::array<struct task_ctl_t, tasks_to_add> l_tasks_under_test;

   /* Create testable tasks*/
   std::array<stack_type[32], tasks_to_add> l_stacks_under_test;

   for (size_t tsk_ind = 0U; tsk_ind < tasks_to_add; ++tsk_ind)
   {
      Task_init(&l_tasks_under_test[tsk_ind], (stack_ptr_type)&l_stacks_under_test[tsk_ind], &Dummy_task_func, 0U, true);
   }

   Task_scheduler_init(); /* Initialise running task list p_head (l_task0)*/
   CHECK_TEXT(p_current_task_ctl == &l_tasks_under_test[0U], "p_current_task failed to initialise as expected");

   /* Create random number generation function*/
   std::random_device random_dev;
   std::mt19937 random_gen(random_dev());
   std::uniform_int_distribution<> distrib(0, 1);

   while (scheduler_iterations > 0U)
   {
      p_task_ctl_t saved_task_before_sched = p_current_task_ctl;
      bool task_switch_expected = distrib(random_gen) == 1; /* If the random generator determines we should switch task - should_switch_task = true, otherwise its false*/

      should_switch_task = task_switch_expected;

      /* Run the scheduler - if should_switch_task is false on entry - the function sets it to true
      This is intended and is used when telling the scheduler tick handler that the index has already 
      been udpated (such as the case when calling Task_set_current_idle())*/
      Rltos_scheduler_switch_context();

      /* Check if we are supposed to switch the tasks*/
      if (task_switch_expected)
      {
         /* check if the next task was the idle task and ensure we tried to skip it*/
         if(saved_task_before_sched->p_next_tctl[state_list] == &idle_task_ctl)
         {
            CHECK_TEXT(p_current_task_ctl == saved_task_before_sched->p_next_tctl[state_list]->p_next_tctl[state_list], "context switch didnt occur when expected");
            CHECK_TEXT(running_task_list.p_index == saved_task_before_sched->p_next_tctl[state_list]->p_next_tctl[state_list], "context switch didnt occur when expected");
         }
         else
         {
            CHECK_TEXT(p_current_task_ctl == saved_task_before_sched->p_next_tctl[state_list], "context switch didnt occur when expected");
            CHECK_TEXT(running_task_list.p_index == saved_task_before_sched->p_next_tctl[state_list], "context switch didnt occur when expected");
         }
      }
      else
      {
         CHECK_TEXT(p_current_task_ctl == saved_task_before_sched, "context switch occurred when not expected or desrired");
         CHECK_TEXT(running_task_list.p_index == saved_task_before_sched, "context switch occurred when not expected or desrired");
      }

      --scheduler_iterations;
   }

   /* Tear down*/
   for (size_t tsk_ind = 0U; tsk_ind < tasks_to_add; ++tsk_ind)
   {
      Task_deinit(&l_tasks_under_test[tsk_ind]);
   }

   Task_scheduler_deinit();
}
/* END OF TEST*/
