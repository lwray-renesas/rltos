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
typedef struct Task_ctl_t MEM_TYPE * p_task_ctl_t;
typedef struct Task_list_t MEM_TYPE * p_task_list_t;

/** Pointer to running task list*/
extern struct Task_list_t running_task_list;
extern struct Task_list_t idle_task_list;

/** @brief Function used to initialise the current task to point at the running lists head */
void Scheduler_init(void);

/** @brief Function used to set initial stack pointer, task entry function & clear pointers to next and prev nodes.
 * @param[in] task_to_init - pointer to a task for initialisation.
 * @param[in] init_sp - initial stack pointer value (after stack initialisation).
 * @param[in] init_task_func - function pointer to tasks entry function.
 */
void Task_init(p_task_ctl_t const task_to_init, const stack_type init_sp, void(* const init_task_func)(void));

/** @brief Function used to append task to a task list
 * @param[in] list_to_append - pointer to a task list for which the task should be appended.
 * @param[in] task_to_append - task to append to list.
 */
void Task_append_to_list(p_task_list_t const list_to_append, p_task_ctl_t const task_to_append);

/** @brief Function used to remove task from a task list
 * @param[in] list_for_remove - pointer to a task list from which the task should be removed.
 * @param[in] task_to_remove - task to remove from list.
 */
void Task_remove_from_list(p_task_list_t const list_for_remove, p_task_ctl_t const task_to_remove);

#endif /* RLTOS_TASK_LIST_H_ */
