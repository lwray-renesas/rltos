/**
 * @addtogroup Rltos_task_scheduler_prv Task Scheduler Private
 * @ingroup Rltos_task_scheduler_api
 * @{
 */

#ifndef RLTOS_TASK_SCHEDULER_PRV_H_
#define RLTOS_TASK_SCHEDULER_PRV_H_

#ifndef RLTOS_TASK_H_
#error "Must include rltos_task.h before including this file in any source file"
#endif

#include "rltos_task_scheduler.h"

/** @brief Macro definition for the maximum number of lists a task can be in.
 * @details Currently set to 2; state and auxillary. This is directly proprtional to the
 * number of values in the typdef enum list_index_t.
*/
#define MAX_TASK_LISTS    (2U)

/** @struct task_ctl_t
 * @brief Definition of task control structure
 * 
 * Contains the definition of a tasks control structure within RLTOS.
 */
struct task_ctl_t
{
	/* Task specific data*/
	stack_ptr_type stored_sp;	/**< Stored value of the stack pointer*/
	p_task_func_t p_task_func;	/**< Function pointer for entry point of task*/
	rltos_uint idle_ready_time; /**< Value representing the time this task will be ready*/
	rltos_uint idle_time;		/**< Value representing the max time this task should remain idled*/
	rltos_uint idle_ready_wrap_count; /**< Value to detect when wrap around is required*/

	/* List specific data*/
	struct task_ctl_t *p_next_tctl[MAX_TASK_LISTS]; /**< Pointer to the next item - can exist in two lists at once*/
	struct task_ctl_t *p_prev_tctl[MAX_TASK_LISTS]; /**< Pointer to the previous item - can exist in two lists at once*/
	p_task_list_t p_owners[MAX_TASK_LISTS];		   /**< Pointer to the list who owns this task - can exist in two lists at once*/
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

/*! @} */

/* Variables defined intask list we want to access during unit testing of modules integrating with kernel*/
extern struct task_list_t running_task_list;
extern struct task_list_t idle_task_list;
extern struct task_list_t stopped_task_list;
extern p_task_ctl_t p_current_task_ctl;
extern volatile rltos_uint rltos_system_tick;
extern volatile rltos_uint rltos_wrap_count;
extern bool should_switch_task;

/** @brief implementation of rltos tick increment - called from rltos_scheduler_asm.asm */
void Rltos_scheduler_tick_inc(void);

/** @brief implementation of rltos context switch - called from rltos_scheduler_asm.asm */
void Rltos_scheduler_switch_context(void);

#endif /* RLTOS_TASK_SCHEDULER_PRV_H_ */
