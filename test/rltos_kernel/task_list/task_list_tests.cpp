/*
 * task_list_tests.c
 *
 *  Created on: 14 Oct 2021
 *      Author: Louis Wray
 */

#include "CppUTest/TestHarness.h"
#include <memory>

/* Including the source file allows us to test the internal workings of the rltos_task_list module*/
#include "rltos_kernel/task_list/rltos_task_list.c"

/** Prototypes*/
void Dummy_task_func(void);

/** Test group for task list initialser functions*/
TEST_GROUP(TaskListInitTestGroup)
{
   /* Global variables for use in tests - provide a list, task, stack and function for every test*/
   p_task_list_t p_dummy_task_list;
   p_task_ctl_t p_dummy_task;
   stack_type dummy_stack[32];

   void setup(void)
   {
      p_dummy_task = new struct Task_ctl_t;
      p_dummy_task_list = new struct Task_list_t;
      Task_init(p_dummy_task, dummy_stack, &Dummy_task_func);
      Task_list_init(p_dummy_task_list, p_dummy_task, state_list);
   }
   /* END OF FUNCTION*/

   void teardown(void)
   {
      delete p_dummy_task;
      delete p_dummy_task_list;
   }
   /* END OF FUNCTION*/
};

void Dummy_task_func(void)
{
   /* Do nothing*/
}
/* END OF FUNCTION*/

TEST(TaskListInitTestGroup, Test_TaskInit_ValuesOk)
{
   std::unique_ptr<struct Task_ctl_t> l_task_under_test = std::make_unique<struct Task_ctl_t>();
   Task_init(l_task_under_test.get(), dummy_stack, &Dummy_task_func);

   CHECK_TEXT((l_task_under_test.get()->stored_sp == dummy_stack), "Task_init failed to initialise stack pointer");
   CHECK_TEXT((l_task_under_test.get()->p_next_tctl[state_list] == NULL), "Task_init failed to initialise pointer to next task to NULL");
   CHECK_TEXT((l_task_under_test.get()->p_prev_tctl[state_list] == NULL), "Task_init failed to initialise pointer to previous task to NULL");
   CHECK_TEXT((l_task_under_test.get()->p_task_func == Dummy_task_func), "Task_init failed to initialise pointer to task function correctly");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskListInit_ValuesOk)
{
   /* Task_init done in setup*/
   /* Task_list_init done in setup*/

   CHECK_TEXT((p_dummy_task_list->p_head == p_dummy_task), "Task_list_init failed to initialise head pointer");
   CHECK_TEXT((p_dummy_task_list->p_index == p_dummy_task), "Task_list_init failed to initialise index pointer");
   CHECK_TEXT((p_dummy_task_list->size == 1U), "Task_list_init failed to initialise size correctly");
   CHECK_TEXT((p_dummy_task_list->p_index->p_next_tctl[state_list] == p_dummy_task), "Task_list_init failed to initialise tasks next task");
   CHECK_TEXT((p_dummy_task_list->p_index->p_prev_tctl[state_list] == p_dummy_task), "Task_list_init failed to initialise tasks prev task");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskAppend)
{
   std::unique_ptr<struct Task_ctl_t> p_local_task = std::make_unique<struct Task_ctl_t>();

   Task_insert_in_list(p_dummy_task_list, p_local_task.get(), state_list);

   CHECK_TEXT((p_dummy_task_list->p_head == p_dummy_task_list->p_index), "Task_insert_in_list editted p_head or p_index without cause");
   CHECK_TEXT((p_dummy_task_list->p_head == p_dummy_task), "Task_insert_in_list editted p_head without cause");
   CHECK_TEXT((p_dummy_task_list->p_index == p_dummy_task), "Task_insert_in_list editted p_index without cause");
   CHECK_TEXT((p_dummy_task_list->size == 2U), "Task_insert_in_list failed to update size correctly");
   CHECK_TEXT((p_local_task->p_next_tctl[state_list] == p_dummy_task_list->p_head), "Task_insert_in_list failed to place task in right order");
   CHECK_TEXT((p_local_task->p_prev_tctl[state_list] == p_dummy_task_list->p_head), "Task_insert_in_list failed to place task in right order");
   CHECK_TEXT((p_dummy_task_list->p_head->p_prev_tctl[state_list] == p_local_task.get()), "Task_insert_in_list failed to place task in right order");
   CHECK_TEXT((p_dummy_task_list->p_index->p_prev_tctl[state_list] == p_local_task.get()), "Task_insert_in_list failed to place task in right order");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskRemove)
{
   Task_remove_from_list(p_dummy_task_list, p_dummy_task, state_list);

   CHECK_TEXT((p_dummy_task_list->p_head == NULL), "Task_remove_from_list did not reset p_head");
   CHECK_TEXT((p_dummy_task_list->p_index == NULL), "Task_remove_from_list did not reset p_index");
   CHECK_TEXT((p_dummy_task_list->size == 0U), "Task_remove_from_list did not reset size");
   CHECK_TEXT((p_dummy_task->p_next_tctl[state_list] == NULL), "Task_remove_from_list did not reset tasks next task pointer");
   CHECK_TEXT((p_dummy_task->p_prev_tctl[state_list] == NULL), "Task_remove_from_list did not reset tasks previous task pointer");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskListInit_NullTask)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskListInit_NullTask");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskAddThenRemoveSameTask)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskAddThenRemoveSameTask");
}
/* END OF TEST*/

TEST(TaskListInitTestGroup, Test_TaskAddThenRemoveDifferentTask)
{
   CHECK_TEXT(false, "TODO: Write Test_TaskAddThenRemoveDifferentTask");
}
/* END OF TEST*/

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
   CHECK_TEXT((sizeof(struct Task_ctl_t) == sizeof(dummy_task_t)), "Dummy task struct is not the same size as the real task control struct");
   CHECK_TEXT((sizeof(struct Task_list_t) == sizeof(dummy_task_list_t)), "Dummy task list struct is not the same size as the real task list struct");
}
/* END OF TEST*/
