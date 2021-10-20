/*
 * rltos_task_list.h
 *
 *  Created on: 22 Apr 2021
 *      Author: Louis Wray
 */

#ifndef RLTOS_TASK_LIST_H_
#define RLTOS_TASK_LIST_H_

#ifndef RLTOS_RLTOS_H_
#error "Must include rltos.h before including this file in any source file"
#endif

/** Declare pointers to opaque data types*/
typedef struct Task_ctl_t * p_task_ctl_t;
typedef struct Task_list_t * p_task_list_t;

/** Provide enumerated type to index task item lists*/
typedef enum
{
    state_list = 0U,    /* Index of list where the item exists in the running or idle list*/
    aux_list            /* Index of list where the item exists waiting on an object*/
}list_index_t;

/** @brief Function used to initialise the current task to point at the running lists head */
void Scheduler_init(void);

/** @brief Function used to set initial stack pointer, task entry function & clear pointers to next and prev nodes.
 * @param[in] task_to_init - pointer to a task for initialisation.
 * @param[in] init_sp - initial stack pointer value (after stack initialisation).
 * @param[in] init_task_func - function pointer to tasks entry function.
 * @param[in] task_is_running - if true - task placed in running list - otherwise placed in idle list.
 */
void Task_init(p_task_ctl_t const task_to_init, const stack_ptr_type init_sp, void(* const init_task_func)(void), bool task_is_running);

/** @brief Function used to set deinit a task and removes from both running and idle lists.
 * @param[in] task_to_deinit - pointer to a task for deinitialisation.
 */
void Task_deinit(p_task_ctl_t const task_to_deinit);

/** @brief Function used to initialise a task list.
 * @param[in] list_to_init - pointer to a list for initialisation.
 * @param[in] first_task - optional first task to add - if NULL, no task is added.
 * @param[in] list_index - index of the list to in which to add the first task (ignored in the case first_task is NULL).
 */
void Task_list_init(p_task_list_t const list_to_init, p_task_ctl_t const first_task, const list_index_t list_index);

/** @brief Function used to append task to the running list
 * @param[in] task_to_run - task to set running.
 */
void Task_set_running(p_task_ctl_t const task_to_insert);

/** @brief Function used to append task to the idle list
 * @param[in] task_to_idle - task to set idle.
 */
void Task_set_idle(p_task_ctl_t const task_to_insert);

/** @brief Function used to append task to a task list
 * @param[in] list_for_insert - pointer to a task list for which the task should be inserted.
 * @param[in] task_to_insert - task to insert in list.
 * @param[in] list_index - index of the list to in which to insert the task.
 */
void Task_insert_in_list(p_task_list_t const list_for_insert, p_task_ctl_t const task_to_insert, const list_index_t list_index);

/** @brief Function used to remove task from a task list
 * @param[in] list_for_remove - pointer to a task list from which the task should be removed.
 * @param[in] task_to_remove - task to remove from list.
 * @param[in] list_index - index of the list to in which to remove the task.
 */
void Task_remove_from_list(p_task_list_t const list_for_remove, p_task_ctl_t const task_to_remove, const list_index_t list_index);

#endif /* RLTOS_TASK_LIST_H_ */
