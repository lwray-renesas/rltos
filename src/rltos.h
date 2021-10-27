/**
 * @defgroup Rltos_task Task
 * The group defining the RLTOS raw kernel tasks.
 * @addtogroup Rltos_task_api api
 * The public API (interface) to the RLTOS kernela tasks.
 * @ingroup Rltos_task
 * @{
 */

#ifndef RLTOS_RLTOS_H_
#define RLTOS_RLTOS_H_

#include "rltos_port.h"

/** @brief Utility macro for unused arguments*/
#define RLTOS_UNUSED(e)	((void)(e))

/** @struct dummy_task_t
 * @brief Dummy task structure guaranteed to be the same size as a task control structure.
 * 
 * This is used to hide the implementation of a real task control structure - and allow static allocation of tasks.
 */
typedef struct
{
	stack_ptr_type dummy0;
	void * dummy1;
	rltos_uint dummy2[4];
	void * dummy3[6];
	rltos_uint dummy4[2];
}dummy_task_t;

/** @struct dummy_task_list_t
 * @brief Dummy task list structure guaranteed to be the same size as a task list structure.
 * 
 * This is used to hide the implementation of a real task list structure - and allow static allocation of tasks.
 */
typedef struct DummyTaskLsit
{
	void * dummy1[2];
	rltos_uint dummy0;
}dummy_task_list_t;

/** @brief pointer to dummy task structure*/
typedef dummy_task_t * p_dummy_task_t;

/** @brief  pointer to dummy task list structure*/
typedef dummy_task_list_t * p_dummy_task_list_t;

/** @brief pointer type for task functions*/
typedef void* p_task_func_t;

/** @brief Enters RLTOS kernel and starts scheduler timer. */
void Rltos_kernel_enter(void);

/** @brief Initialises task control structure, stack & appends to task list.
 * @param[inout] task_to_add - pointer to dummy task structure from which to create the task.
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_f - function pointer to the task entry function.
 * @param[in] task_priority - function pointer to the task entry function.
 * @param[in] run_task - flag indicating whether to initialise the task to running or let it idle.*/
void Rltos_task_create(p_dummy_task_t const task_to_add, stack_ptr_type const p_stack_top, p_task_func_t const p_task_func, rltos_uint const task_priority, bool const run_task);

/** @brief Puts the current thread to sleep for a minimum of the given number of ticks*/
void Rltos_task_sleep(const rltos_uint tick_count);

#endif /* RLTOS_RLTOS_H_ */

/*! @} */
