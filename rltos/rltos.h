/*
 * rltos.h
 *
 *  Created on: 22 Apr 2021
 *      Author: Louis Wray
 */

#ifndef RLTOS_RLTOS_H_
#define RLTOS_RLTOS_H_

/** The following definitions MUST be provided:
 * - uint8_t
 * - int8_t
 * - uint16_t
 * - int16_t
 * - uint32_t
 * - int32_t
 * - bool
 * - true
 * - false
 * - NULL
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "rltos_port.h"

/** @brief Utility macro for declaring a task.*/
#define RLTOS_DECLARE_TASK(task_name, stack_size_bytes, task_func_name)						\
	MEM_TYPE stack_type task_name##_stack[stack_size_bytes/2U];								\
	MEM_TYPE dummy_task_t task_name##_str;													\
	extern void task_func_name(void);

/** @brief Utility macro for creating a task.
 * @note This can only be used if the RLTOS_DECLARE_TASK utility macro has been used.
 */
#define RLTOS_CREATE_TASK(task_name, stack_size_bytes, task_func_name)						\
	Rltos_task_create(&task_name##_str, &task_name##_stack[stack_size_bytes/2U], &task_func_name)

/** @brief Dummy task structure guaranteed to be the same size as a task control structure.
 * This is used to hide the implementation of a real task control structure - but also allow static allocation of tasks.
 */
typedef struct
{
	stack_type dummy0;
	void * dummy1[3];
}dummy_task_t;

/** @brief Dummy task list structure guaranteed to be the same size as a task list structure.
 * This is used to hide the implementation of a real task list structure - but also allow static allocation of tasks.
 */
typedef struct
{
	stack_type dummy0;
	void * dummy1[2];
}dummy_task_list_t;

/** @brief typedef pointer to dummy task structure*/
typedef dummy_task_t MEM_TYPE * p_dummy_task_t;

/** @brief typedef pointer to dummy task list structure*/
typedef dummy_task_list_t MEM_TYPE * p_dummy_task_list_t;

/** @brief Enters RLTOS kernel and starts scheduler timer. */
void Rltos_kernel_enter(void);

/** @brief Initialises task control structure, stack & appends to task list.
 * @param[in] task_to_add - pointer to dummy task structure from which to create the task.
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_f - function pointer to the task entry function.*/
void Rltos_task_create(p_dummy_task_t const task_to_add, p_stack_type p_stack_top, void(* const p_task_func)(void));

/** @brief Puts the current thread to sleep for a minimum of the given number of ticks*/
void Rltos_task_sleep(const rltos_uint tick_count);

#endif /* RLTOS_RLTOS_H_ */
