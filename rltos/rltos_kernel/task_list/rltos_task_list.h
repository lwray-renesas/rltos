/**
 * @addtogroup Rltos_task_list rltos_task_list
 * @{
 */

#ifndef RLTOS_TASK_LIST_H_
#define RLTOS_TASK_LIST_H_

#ifndef RLTOS_RLTOS_H_
#error "Must include rltos.h before including this file in any source file"
#endif

/** Declare pointers to opaque data types*/
typedef struct task_ctl_t * p_task_ctl_t;
typedef struct task_list_t * p_task_list_t;

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
 * @param[in] priority - priority of task, lower is better.
 * @param[in] task_is_running - if true - task placed in running list - otherwise placed in idle list.
 */
void Task_init(p_task_ctl_t const task_to_init, const stack_ptr_type init_sp, void(* const init_task_func)(void), rltos_uint priority, bool task_is_running);

/** @brief Function used to set deinit a task and removes from both running and idle lists.
 * @param[in] task_to_deinit - pointer to a task for deinitialisation.
 */
void Task_deinit(p_task_ctl_t const task_to_deinit);

/** @brief Function used to initialise a task list - 0's size and sets HEAD & INDEX to NULL.
 * @param[in] list_to_init - pointer to a list for initialisation.
 */
void Task_list_init(p_task_list_t const list_to_init);

/** @brief Function used to append task to the running list
 * @param[in] task_to_run - task to set running.
 */
void Task_set_running(p_task_ctl_t const task_to_run);

/** @brief Function used to append task to the idle list
 * @param[in] time_to_idle - number of ticks to idle task.
 */
void Task_set_current_idle(const rltos_uint time_to_idle);

/** @brief Function used to set the current task to wait on an object (uses auxillary list)
 * @param[in] owner - object to register waiting on.
 * @param[in] time_to_wait - number of ticks to wait.
 */
void Task_set_current_wait_on_object(p_task_list_t const owner, const rltos_uint time_to_wait);

#endif /* RLTOS_TASK_LIST_H_ */

/*! @} */
