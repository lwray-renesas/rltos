/**
 * @defgroup Rltos_task_scheduler Task Scheduler
 * The group defining the RLTOS task scheduler - containing the tasks and task lists.
 * @ingroup Rltos_core
 * @addtogroup Rltos_task_scheduler_api Task Scheduler API
 * The API (interface) to the RLTOS task scheduler module.
 * This module is ONLY used by the modules of RLTOS and should NEVER be accessed directly by an application.
 * @ingroup Rltos_task_scheduler
 * @ingroup Rltos_task_prv
 * @ingroup Rltos_semaphore_prv
 * @ingroup Rltos_queue_prv
 * @ingroup Rltos_mutex_prv
 * @ingroup Rltos_event_flags_prv
 * @{
 */

#ifndef RLTOS_TASK_SCHEDULER_H_
#define RLTOS_TASK_SCHEDULER_H_

#ifndef RLTOS_TASK_H_
#error "Must include rltos_task.h before including this file in any source file"
#endif

/** @brief pointer to task control structure*/
typedef struct task_ctl_t * p_task_ctl_t;

/** @brief pointer to task list control structure*/
typedef struct task_list_t * p_task_list_t;

/** @enum list_index_t
 * @brief Provides enumerated type to index task item lists,
 * This is used to index between a tasks state list (running or idle) and an 
 * auxilliary list for an object which the task is associated with waiting on.
 */
typedef enum
{
    state_list = 0U,    /**< Index of list where the item exists in the running or idle list*/
    aux_list            /**< Index of list where the item exists waiting on an object*/
}list_index_t;

/** @brief Function to initialise the current task on entry to the scheduler & create + add the idle task to the running list.*/
void Task_scheduler_init(void);

/** @brief Function to deinitialise the current task and remove the idle task from the running list.*/
void Task_scheduler_deinit(void);

/** @brief Function used to set initial stack pointer, task entry function & clear pointers to next and prev nodes.
 * @param[inout] task_to_init - pointer to a task for initialisation.
 * @param[in] init_sp - initial stack pointer value (after stack initialisation).
 * @param[in] init_task_func - function pointer to tasks entry function.
 * @param[in] priority - priority of task, lower is better.
 * @param[in] task_is_running - if true - task placed in running list - otherwise placed in idle list.
 */
void Task_init(p_task_ctl_t const task_to_init, const stack_ptr_type init_sp, p_task_func_t const init_task_func, rltos_uint const priority, bool const task_is_running);

/** @brief Function used to set deinit a task and removes from both state and aux lists.
 * @param[inout] task_to_deinit - pointer to a task for deinitialisation.
 */
void Task_deinit(p_task_ctl_t const task_to_deinit);

/** @brief Function used to initialise a task list - 0's size and sets HEAD & INDEX to NULL.
 * @param[inout] list_to_init - pointer to a list for initialisation.
 */
void Task_list_init(p_task_list_t const list_to_init);

/** @brief Function used to insert the task to the running list
 * @param[inout] task_to_run - task to set running.
 */
void Task_set_running(p_task_ctl_t const task_to_run);

/** @brief Function used to set a task running - only if it is stopped (not waiting on an object or idled)
 * @param[inout] task_to_run - task to resume.
 */
void Task_resume(p_task_ctl_t const task_to_resume);

/** @brief Function used to append the task to the stopped list
 * @param[in] task_to_stop - task to stop.
 */
void Task_set_stopped(p_task_ctl_t const task_to_stop);

/** @brief Function used to append task to the idle list
 * @param[in] time_to_idle - number of ticks to idle task.
 */
void Task_set_current_idle(const rltos_uint time_to_idle);

/** @brief Function used to set the current task to wait on an object (uses auxillary list)
 * @param[inout] owner - object to register waiting on.
 * @param[in] time_to_wait - number of ticks to wait.
 */
void Task_set_current_wait_on_object(p_task_list_t const owner, const rltos_uint time_to_wait);

/** @brief if task_to_check is the current task - calls yield.
 * @param[in] task_to_check - task to check if its the current task or not.
 */
void Task_yield_if_current_task(p_task_ctl_t const task_to_check);

#endif /* RLTOS_TASK_SCHEDULER_H_ */

/*! @} */
