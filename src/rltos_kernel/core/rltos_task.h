/**
 * @defgroup Rltos_core Core
 * The group containing the core workings of the RLTOS system - namely tasks and the task scheduler.
 * @defgroup Rltos_task Task
 * The group defining the RLTOS raw kernel tasks.
 * @ingroup Rltos_core
 * @addtogroup Rltos_task_api Task API
 * The public API (interface) to the RLTOS kernela tasks.
 * @ingroup Rltos_task
 * @{
 */

#ifndef RLTOS_TASK_H_
#define RLTOS_TASK_H_

#include "rltos_port.h"

/** @brief Macro definition for the maximum number of lists a task can be in.
 * @details Currently set to 2; state and auxillary. This is directly proprtional to the
 * number of values in the typdef enum list_index_t.
*/
#define MAX_TASK_LISTS    (2U)

/** @brief pointer to task control structure*/
typedef struct rltos_task * p_rltos_task_t;

/** @brief pointer to task list control structure*/
typedef struct rltos_task_list * p_rltos_task_list_t;

/** @struct rltos_task_t
 * @brief Definition of task control structure
 * 
 * Contains the definition of a tasks control structure within RLTOS.
 */
typedef struct rltos_task
{
	/* Task specific data*/
	stack_ptr_type stored_sp;	/**< Stored value of the stack pointer*/
	p_task_func_t p_task_func;	/**< Function pointer for entry point of task*/
	rltos_uint idle_ready_time; /**< Value representing the time this task will be ready*/
	rltos_uint idle_time;		/**< Value representing the max time this task should remain idled*/
	rltos_uint idle_ready_wrap_count; /**< Value to detect when wrap around is required*/

	/* List specific data*/
	struct rltos_task *p_next_tctl[MAX_TASK_LISTS]; /**< Pointer to the next item - can exist in two lists at once*/
	struct rltos_task *p_prev_tctl[MAX_TASK_LISTS]; /**< Pointer to the previous item - can exist in two lists at once*/
	p_rltos_task_list_t p_owners[MAX_TASK_LISTS];		   /**< Pointer to the list who owns this task - can exist in two lists at once*/
}rltos_task_t;

/** @struct rltos_task_list_t
 * @brief Definition of task list control structure
 * 
 * Contains the definition of a task list control structure within RLTOS.
 */
typedef struct rltos_task_list
{
	p_rltos_task_t p_head;  /**< Head of list*/
	p_rltos_task_t p_index; /**< current index of list*/
	rltos_uint size;	  /**< Size of list*/
}rltos_task_list_t;

/** @enum rltos_err_t
 * @brief Enumerated type to store RLTOS error code
 */
typedef enum
{
	RLTOS_SUCCESS = 0U, /**< Success*/
	RLTOS_TIMEOUT, /**< Indicates an RTOS operation has timed out*/
	RLTOS_LOCKED, /**< Indicates a resource is currently locked*/
	RLTOS_RELEASED, /**< Indicates a resource is currently released*/
	RLTOS_INVALID_CONTEXT, /**< Indicates a function has been called in an invalid context*/
	RLTOS_ALREADY_SET, /**< Indicates a value is already set*/
	RLTOS_MAXIMUM, /**< Indicates a value is at the maximum threshold*/
}rltos_err_t;

/** @brief Enters RLTOS kernel and starts scheduler timer. 
 * Should not return from this function, if we do the only possible reason for it is:
 * 1. The dummy types sizes do not match the real types internally.
*/
void Rltos_kernel_enter(void);

/** @brief Cleanup of RLTOS kernel and stops scheduler timer. */
void Rltos_kernel_kill(void);

/** @brief Initialises task control structure, stack & appends to task list.
 * @param[inout] task_to_create - pointer to task structure from which to create the task.
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_f - function pointer to the task entry function.
 * @param[in] run_task - flag indicating whether to initialise the task to running or let it idle.*/
void Rltos_task_create(p_rltos_task_t const task_to_create, stack_ptr_type const p_stack_top, p_task_func_t const p_task_func, bool const run_task);

/** @brief Deinitialises task control structure, and removes from any lists.
 * @param[inout] task_to_destroy - pointer to task structure which represents the task to destroy.
 * @note Never call this function on the currently running task - the behaviour is undefined.*/
void Rltos_task_destroy(p_rltos_task_t const task_to_destroy);

/** @brief Stops the targetted task indefinitely.
 * @param[in] task_to_stop - Pointer to the task to stop.*/
void Rltos_task_stop(p_rltos_task_t const task_to_stop);

/** @brief Resumes the targetted task in the stop state.
 * @param[in] task_to_resume - Pointer to the task to resume.*/
void Rltos_task_resume(p_rltos_task_t const task_to_resume);

/** @brief Puts the current thread to SLEEP for a minimum of the given number of ticks
 * @param[in] tick_count - The number of RLTOS ticks which the task will be placed in the idle list. If this value is 0 - this function call resolves to a yield.*/
void Rltos_task_sleep(const rltos_uint tick_count);

#endif /* RLTOS_TASK_H_ */

/*! @} */
