/**
 * @defgroup Rltos_task_list Task list
 * @ingroup Rltos_task_prv
 * The group defining the RLTOS task list - containing the scheduler.
 * @addtogroup Rltos_task_list_api api
 * The API (interface) to the RLTOS task list module.
 * This module is ONLY used by the modules of RLTOS and should NEVER be accessed directly by an application.
 * The only reason the struct definitions for task and task list types are defined here is so the structs created with other modules
 * can be unit tested thoroughly - that is the tasks and the task lists can all be seperately probed
 * @ingroup Rltos_task_list
 * @{
 */

#ifndef RLTOS_TASK_LIST_H_
#define RLTOS_TASK_LIST_H_

#ifndef RLTOS_TASK_H_
#error "Must include rltos_task.h before including this file in any source file"
#endif

/** @brief pointer to task control structure*/
typedef struct task_ctl_t * p_task_ctl_t;

/** @brief pointer to task list control structure*/
typedef struct task_list_t * p_task_list_t;

/** @struct task_ctl_t
 * @brief Definition of task control structure
 * 
 * Contains the definition of a tasks control structure within RLTOS.
 */
struct task_ctl_t
{
	/* Task specific data*/
	stack_ptr_type stored_sp;	/**< Stored value of the stack pointer*/
	p_task_func_t p_task_func;	/**< Function pointer for entry of point of task*/
	rltos_uint idle_ready_time; /**< Value representing the time this task will be ready*/
	rltos_uint idle_time;		/**< Value representing the max time this task should remain idled*/
	rltos_uint idle_wrap_count; /**< Value to detect when wrap around is required*/
	rltos_uint priority;		/**< Value representing the tasks priority*/

	/* List specific data*/
	struct task_ctl_t *p_next_tctl[2]; /**< Pointer to the next item - can exist in two lists at once*/
	struct task_ctl_t *p_prev_tctl[2]; /**< Pointer to the previous item - can exist in two lists at once*/
	p_task_list_t p_owners[2];		   /**< Pointer to the list who owns this task - can exist in two lists at once*/
	rltos_uint sorting_values[2];		/**< Array to store the lists sorting values*/
};

/** @struct task_list_t
 * @brief Definition of task list control structure
 * 
 * Contains the definition of a task list control structure within RLTOS.
 */
struct task_list_t
{
	p_task_ctl_t p_head;  /**< Head of list*/
	p_task_ctl_t p_index; /**< current index of list*/
	rltos_uint size;	  /**< Size of list*/
};

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

/* Variables defined intask list we want to access during unit testing of modules integrating with kernel*/
extern struct task_list_t running_task_list;
extern struct task_list_t idle_task_list;
extern p_task_ctl_t p_current_task_ctl;
extern volatile rltos_uint rltos_system_tick;
extern volatile rltos_uint rltos_wrap_count;
extern rltos_uint rltos_next_idle_ready_tick;
extern rltos_uint rltos_next_idle_ready_wrap_count;
extern bool should_switch_task;

/** @brief Function used to check if a task is contained within the task list or not.
 * @param[in] lst - pointer to a task list to check for tasks existence.
 * @param[in] tsk - task to check whether contained in (owned by) list.
 * @param[in] list_index - index of the list to check.
 */
static inline bool Task_is_in_list(p_task_list_t const lst, p_task_ctl_t const tsk, const list_index_t ind)
{
	return (lst != NULL) && (tsk->p_owners[ind] == lst);
}
/* END OF FUNCTION*/

/** @brief Function used to set initial stack pointer, task entry function & clear pointers to next and prev nodes.
 * @param[inout] task_to_init - pointer to a task for initialisation.
 * @param[in] init_sp - initial stack pointer value (after stack initialisation).
 * @param[in] init_task_func - function pointer to tasks entry function.
 * @param[in] priority - priority of task, lower is better.
 * @param[in] task_is_running - if true - task placed in running list - otherwise placed in idle list.
 */
void Task_init(p_task_ctl_t const task_to_init, const stack_ptr_type init_sp, p_task_func_t const init_task_func, rltos_uint const priority, bool const task_is_running);

/** @brief Function used to set deinit a task and removes from both running and idle lists.
 * @param[inout] task_to_deinit - pointer to a task for deinitialisation.
 */
void Task_deinit(p_task_ctl_t const task_to_deinit);

/** @brief Function used to initialise a task list - 0's size and sets HEAD & INDEX to NULL.
 * @param[inout] list_to_init - pointer to a list for initialisation.
 */
void Task_list_init(p_task_list_t const list_to_init);

/** @brief Function used to append task to the running list
 * @param[inout] task_to_run - task to set running.
 */
void Task_set_running(p_task_ctl_t const task_to_run);

/** @brief Function used to append task to the idle list
 * @param[in] time_to_idle - number of ticks to idle task.
 */
void Task_set_current_idle(const rltos_uint time_to_idle);

/** @brief Function used to set the current task to wait on an object (uses auxillary list)
 * @param[inout] owner - object to register waiting on.
 * @param[in] time_to_wait - number of ticks to wait.
 */
void Task_set_current_wait_on_object(p_task_list_t const owner, const rltos_uint time_to_wait);

#endif /* RLTOS_TASK_LIST_H_ */

/*! @} */
