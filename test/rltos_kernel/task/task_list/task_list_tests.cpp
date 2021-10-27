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
#include "task_list/rltos_task_list.c"
}





/** Prototypes*/
void Dummy_task_func(void);

/** Test group for task list initialser functions*/
TEST_GROUP(TaskListInitTestGroup)
{
   /* Global variables for use in tests - provide a list, task, stack and function for every test*/
   p_task_list_t p_aux_task_list;
   p_task_ctl_t p_dummy_task;
   stack_type dummy_stack[32];

   void setup(void)
   {
      /* Setup initialises a dummy task to the running list.
      * Also initialises a dummy tasklist to contain a dummy task in the aux list
      */
      p_dummy_task = new struct task_ctl_t;
      p_aux_task_list = new struct task_list_t;
      Task_init(p_dummy_task, dummy_stack, &Dummy_task_func, 0U, true);
      Task_list_init(p_aux_task_list);
   }
   /* END OF FUNCTION*/

   void teardown(void)
   {
      /* Deinitialises dummy task and destroys dummy task and list*/
      Task_deinit(p_dummy_task);
      delete p_dummy_task;
      delete p_aux_task_list;
   }
   /* END OF FUNCTION*/
};

void Dummy_task_func(void)
{
   /* Do nothing*/
}
/* END OF FUNCTION*/

TEST(TaskListInitTestGroup, Test_SchedulerInit)
{
   CHECK_TEXT(false, "TODO: Write Test_SchedulerInit");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskInit_ValuesOk)
{
   std::unique_ptr<struct task_ctl_t> l_task_under_test = std::make_unique<struct task_ctl_t>();
   Task_init(l_task_under_test.get(), dummy_stack, &Dummy_task_func, 0U, true);

   CHECK_TEXT((l_task_under_test.get()->stored_sp == dummy_stack), "Task_init failed to initialise stack pointer");
   CHECK_TEXT((l_task_under_test.get()->p_task_func == Dummy_task_func), "Task_init failed to initialise pointer to task function correctly");

   CHECK_TEXT((l_task_under_test->p_next_tctl[state_list] == p_dummy_task), "Task_list_init failed to initialise tasks next task");
   CHECK_TEXT((l_task_under_test->p_prev_tctl[state_list] == p_dummy_task), "Task_list_init failed to initialise tasks prev task");
   CHECK_TEXT((l_task_under_test.get()->p_owners[state_list] == &running_task_list), "Task_init failed to add task to the running list");
   CHECK_TEXT((l_task_under_test.get()->p_owners[aux_list] == NULL), "Task_init editted aux list owner without cause");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskDeinit_ValuesOk)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskDeinit_ValuesOk");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskListInit_ValuesOk)
{
   /* Task_init done in setup*/
   /* Task_list_init done in setup*/

   CHECK_TEXT((p_aux_task_list->p_head == p_dummy_task), "Task_list_init failed to initialise head pointer");
   CHECK_TEXT((p_aux_task_list->p_index == p_dummy_task), "Task_list_init failed to initialise index pointer");
   CHECK_TEXT((p_aux_task_list->size == 1U), "Task_list_init failed to initialise size correctly");
   CHECK_TEXT((p_aux_task_list->p_index->p_next_tctl[state_list] == p_dummy_task), "Task_list_init failed to initialise tasks next task");
   CHECK_TEXT((p_aux_task_list->p_index->p_prev_tctl[state_list] == p_dummy_task), "Task_list_init failed to initialise tasks prev task");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskAppend)
{
   std::unique_ptr<struct task_ctl_t> p_local_task = std::make_unique<struct task_ctl_t>();

   Task_insert_in_list(p_aux_task_list, p_local_task.get(), state_list, 0U);

   CHECK_TEXT((p_aux_task_list->p_head == p_aux_task_list->p_index), "Task_insert_in_list editted p_head or p_index without cause");
   CHECK_TEXT((p_aux_task_list->p_head == p_dummy_task), "Task_insert_in_list editted p_head without cause");
   CHECK_TEXT((p_aux_task_list->p_index == p_dummy_task), "Task_insert_in_list editted p_index without cause");
   CHECK_TEXT((p_aux_task_list->size == 2U), "Task_insert_in_list failed to update size correctly");
   CHECK_TEXT((p_local_task->p_next_tctl[state_list] == p_aux_task_list->p_head), "Task_insert_in_list failed to place task in right order");
   CHECK_TEXT((p_local_task->p_prev_tctl[state_list] == p_aux_task_list->p_head), "Task_insert_in_list failed to place task in right order");
   CHECK_TEXT((p_aux_task_list->p_head->p_prev_tctl[state_list] == p_local_task.get()), "Task_insert_in_list failed to place task in right order");
   CHECK_TEXT((p_aux_task_list->p_index->p_prev_tctl[state_list] == p_local_task.get()), "Task_insert_in_list failed to place task in right order");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskRemove)
{
   Task_remove_from_list(p_aux_task_list, p_dummy_task, aux_list);

   CHECK_TEXT((p_aux_task_list->p_head == NULL), "Task_remove_from_list did not reset p_head");
   CHECK_TEXT((p_aux_task_list->p_index == NULL), "Task_remove_from_list did not reset p_index");
   CHECK_TEXT((p_aux_task_list->size == 0U), "Task_remove_from_list did not reset size");
   CHECK_TEXT((p_dummy_task->p_next_tctl[aux_list] == NULL), "Task_remove_from_list did not reset tasks next task pointer");
   CHECK_TEXT((p_dummy_task->p_prev_tctl[aux_list] == NULL), "Task_remove_from_list did not reset tasks previous task pointer");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskInsert)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskInsert");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskSetRunning)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskSetRunning");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskSetCurrentIdle)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskSetCurrentIdle");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskSetCurrentWaitOnObject)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskSetCurrentWaitOnObject");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_SchedulerTickInc)
{
   CHECK_TEXT(false, "TODO: Write Test_SchedulerTickInc");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_SchedulerSwitchContext)
{
      Rltos_kernel_enter();
   CHECK_TEXT(false, "TODO: Write Test_SchedulerSwitchContext");
}
/* END OF TEST*/

