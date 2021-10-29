/*
 * task_list_tests.c
 *
 *  Created on: 14 Oct 2021
 *      Author: Louis Wray
 */

#include "CppUTest/TestHarness.h"
#include <memory>

extern "C" {
/* Including the source file allows us to test the internal workings of the module*/
#include "task_scheduler/rltos_task_scheduler.c"
}
#include <array>
#include <random>
#include <iostream>





/** Prototypes*/
void Dummy_task_func(void);

/** Test group for task list initialser functions*/
TEST_GROUP(TaskSchedulerTestGroup)
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

void Dummy_task_func(void)
{
   /* Do nothing*/
}
/* END OF FUNCTION*/

TEST(TaskSchedulerTestGroup, Test_SchedulerInit_SetsCurrentTaskOk)
{
   /* Create testable task & append to list*/
   std::unique_ptr<struct task_ctl_t> l_task_under_test = std::make_unique<struct task_ctl_t>();
   Task_append_to_list(&running_task_list, l_task_under_test.get(), state_list);

   Task_scheduler_init();

   CHECK_TEXT(p_current_task_ctl == l_task_under_test.get(), "Scheduler incorrectly initialised");

   Task_remove_from_list(&running_task_list, l_task_under_test.get(), state_list);
   p_current_task_ctl = NULL;
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
   std::array<struct task_ctl_t,tasks_to_add> l_tasks_under_test;

   /* Create random number generation function*/
   std::random_device rd;
   std::mt19937 g(rd());
   std::uniform_int_distribution<> distrib(0);

   /* set the priorities of each task*/
   for(struct task_ctl_t &tsk : l_tasks_under_test)
   {
      tsk.priority = static_cast<rltos_uint>(distrib(g));
   }

   /* Shuffle the list*/
   std::shuffle(l_tasks_under_test.begin(), l_tasks_under_test.end(), g);

   /* Add the tasks (now in random order)*/
   for(struct task_ctl_t &tsk : l_tasks_under_test)
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
   std::array<struct task_ctl_t,tasks_to_add> l_tasks_under_test;

   /* Create random number generation function*/
   std::random_device rd;
   std::mt19937 g(rd());
   std::uniform_int_distribution<> distrib(0);

   const rltos_uint tsk_priority = static_cast<rltos_uint>(distrib(g));

   /* set the priorities of each task*/
   for(struct task_ctl_t &tsk : l_tasks_under_test)
   {
      tsk.priority = tsk_priority;
   }

   /* Shuffle the list*/
   std::shuffle(l_tasks_under_test.begin(), l_tasks_under_test.end(), g);

   /* Add the tasks (now in random order)*/
   for(struct task_ctl_t &tsk : l_tasks_under_test)
   {
      Task_insert_in_list(l_list_under_test.get(), &tsk, state_list, tsk.priority);
   }

   /* Verify the list is now ordered in the order the same priority tasks were added in*/
   rltos_uint index_arr = 0U;
   p_task_ctl_t tsk_indexer = l_list_under_test->p_head;
   while (tsk_indexer->p_next_tctl[state_list] != l_list_under_test->p_head)
   {
      /* Checks if the current task order in the list, is the same as the index of the task in the task array*/
      CHECK_TEXT( &l_tasks_under_test[index_arr] == tsk_indexer, "List in wrong order!");
      tsk_indexer = tsk_indexer->p_next_tctl[state_list];
      ++index_arr;
   }

   /* Check last value*/
   CHECK_TEXT( &l_tasks_under_test[index_arr] == tsk_indexer, "List in wrong order!");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetRunning)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskSetRunning");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetCurrentIdle)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskSetCurrentIdle");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_TaskSetCurrentWaitOnObject)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskSetCurrentWaitOnObject");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_SchedulerTickInc)
{
   CHECK_TEXT(false, "TODO: Write Test_SchedulerTickInc");
}
/* END OF TEST*/

TEST(TaskSchedulerTestGroup, Test_SchedulerSwitchContext)
{
   /* Create dummy task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task0 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack0 = std::make_unique<stack_type[]>(32);
   /* Create dummy task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task1 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack1 = std::make_unique<stack_type[]>(32);
   /* Create dummy task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task2 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack2 = std::make_unique<stack_type[]>(32);
   /* Create dummy task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task3 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack3 = std::make_unique<stack_type[]>(32);
   /* Create dummy task and stack*/
   std::unique_ptr<struct task_ctl_t> l_task4 = std::make_unique<struct task_ctl_t>();
   std::unique_ptr<stack_type[]> l_stack4 = std::make_unique<stack_type[]>(32);

   Task_init(l_task0.get(), l_stack0.get(), &Dummy_task_func, 0U, true);
   Task_init(l_task1.get(), l_stack1.get(), &Dummy_task_func, 0U, true);
   Task_init(l_task2.get(), l_stack2.get(), &Dummy_task_func, 0U, true);
   Task_init(l_task3.get(), l_stack3.get(), &Dummy_task_func, 0U, true);
   Task_init(l_task4.get(), l_stack4.get(), &Dummy_task_func, 0U, true);

   Task_scheduler_init(); /* Initialise running task list p_head (l_task0)*/
   CHECK_TEXT(p_current_task_ctl == l_task0.get(), "p_current_task failed to initialise as expected");

   should_switch_task = true;
   Rltos_scheduler_switch_context();
   CHECK_TEXT(p_current_task_ctl == l_task1.get(), "context switch didnt occur when expected");
   CHECK_TEXT(running_task_list.p_index == l_task1.get(), "context switch didnt occur when expected");

   should_switch_task = false;
   Rltos_scheduler_switch_context();
   CHECK_TEXT(p_current_task_ctl == l_task1.get(), "context switch occurred when expected");
   CHECK_TEXT(running_task_list.p_index == l_task1.get(), "context switch occurred when expected");

   should_switch_task = true;
   Rltos_scheduler_switch_context();
   CHECK_TEXT(p_current_task_ctl == l_task2.get(), "context switch didnt occur when expected");
   CHECK_TEXT(running_task_list.p_index == l_task2.get(), "context switch didnt occur when expected");

   should_switch_task = true;
   Rltos_scheduler_switch_context();
   CHECK_TEXT(p_current_task_ctl == l_task3.get(), "context switch didnt occur when expected");
   CHECK_TEXT(running_task_list.p_index == l_task3.get(), "context switch didnt occur when expected");

   should_switch_task = false;
   Rltos_scheduler_switch_context();
   CHECK_TEXT(p_current_task_ctl == l_task3.get(), "context switch occurred when expected");
   CHECK_TEXT(running_task_list.p_index == l_task3.get(), "context switch occurred when expected");

   should_switch_task = true;
   Rltos_scheduler_switch_context();
   CHECK_TEXT(p_current_task_ctl == l_task4.get(), "context switch didnt occur when expected");
   CHECK_TEXT(running_task_list.p_index == l_task4.get(), "context switch didnt occur when expected");

   should_switch_task = false;
   Rltos_scheduler_switch_context();
   CHECK_TEXT(p_current_task_ctl == l_task4.get(), "context switch occurred when expected");
   CHECK_TEXT(running_task_list.p_index == l_task4.get(), "context switch occurred when expected");

   should_switch_task = false;
   Rltos_scheduler_switch_context();
   CHECK_TEXT(p_current_task_ctl == l_task4.get(), "context switch occurred when expected");
   CHECK_TEXT(running_task_list.p_index == l_task4.get(), "context switch occurred when expected");

   should_switch_task = true;
   Rltos_scheduler_switch_context();
   CHECK_TEXT(p_current_task_ctl == l_task0.get(), "context switch didnt occur when expected");
   CHECK_TEXT(running_task_list.p_index == l_task0.get(), "context switch didnt occur when expected");

   /* Tear down*/
   Task_deinit(l_task0.get());
   Task_deinit(l_task1.get());
   Task_deinit(l_task2.get());
   Task_deinit(l_task3.get());
   Task_deinit(l_task4.get());

   p_current_task_ctl = NULL;
}
/* END OF TEST*/

