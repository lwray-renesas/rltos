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

/** @brief Utility macro for declaring a task.
 * @note This should really only be used with the RLTOS_CREATE_TASK(task_name, stack_size_bytes, task_func_name, tsk_priority, start_running).
 */
/* cppcheck-suppress misra-c2012-20.10 - This macro is a utility macro - not functional code, only way to implement is with # and ## */
/* cppcheck-suppress misra-c2012-20.7 - parenthesis will break this macro*/
#define RLTOS_DECLARE_TASK(task_name, stack_size_bytes, task_func_name)						\
	stack_type task_name##_stack[stack_size_bytes/2U];								\
	dummy_task_t task_name##_str;													\
	extern void task_func_name(void);

/** @brief Utility macro for creating a task.
 * @note This can only be used if the RLTOS_DECLARE_TASK(task_name, stack_size_bytes, task_func_name) utility macro has been used.
 */
/* cppcheck-suppress misra-c2012-20.10 - This macro is a utility macro - not functional code, only way to implement is with # and ## */
/* cppcheck-suppress misra-c2012-20.7 - parenthesis will break this macro*/
#define RLTOS_CREATE_TASK(task_name, stack_size_bytes, task_func_name, tsk_priority, start_running)	 \
	Rltos_task_create(&task_name##_str, &task_name##_stack[stack_size_bytes/2U], &task_func_name, tsk_priority, start_running)

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
typedef struct
{
	void * dummy1[2];
	rltos_uint dummy0;
}dummy_task_list_t;

/** @brief pointer to dummy task structure*/
typedef dummy_task_t * p_dummy_task_t;

/** @brief  pointer to dummy task list structure*/
typedef dummy_task_list_t * p_dummy_task_list_t;

/** @brief Enters RLTOS kernel and starts scheduler timer. */
void Rltos_kernel_enter(void);

/** @brief Initialises task control structure, stack & appends to task list.
 * @param[inout] task_to_add - pointer to dummy task structure from which to create the task.
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_f - function pointer to the task entry function.
 * @param[in] task_priority - function pointer to the task entry function.
 * @param[in] run_task - flag indicating whether to initialise the task to running or let it idle.*/
void Rltos_task_create(p_dummy_task_t const task_to_add, stack_ptr_type const p_stack_top, void(* const p_task_func)(void), rltos_uint task_priority, bool run_task);

/** @brief Puts the current thread to sleep for a minimum of the given number of ticks*/
void Rltos_task_sleep(const rltos_uint tick_count);

#endif /* RLTOS_RLTOS_H_ */

/*! @} */
