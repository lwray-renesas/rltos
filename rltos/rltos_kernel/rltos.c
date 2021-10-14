/*
 * rltos_kernel.c
 *
 *  Created on: 22 Apr 2021
 *      Author: Louis Wray
 */

#include "rltos.h"
#include "task_list/rltos_task_list.h"

/** @brief Function to enter the first rltos task.
 * @details Defined in rltos_kernel.asm - does not save context, only restores it and returns as though from interrupt (RETI).
 * This function MUST be called after initialising the current task pointer (p_current_task_ctl).
 */
extern void Rltos_enter_first_task(void);

void Rltos_kernel_enter(void)
{
	Init_current_task();
	Rltos_enter_first_task();
}
/* END OF FUNCTION*/

void Rltos_task_create(p_dummy_task_t const task_to_add, p_stack_type p_stack_top, void(* const p_task_func)(void))
{
	/* Initialise the stack*/
	Rltos_task_stack_init(p_stack_top, p_task_func);

	/* Initialise the task*/
	Init_task((p_task_ctl_t)(task_to_add), (stack_type)p_stack_top, p_task_func);

	/* Add task to running list*/
	Append_task_to_list(&running_task_list, (p_task_ctl_t )task_to_add);
}
/* END OF FUNCTION*/

void Rltos_task_sleep(const rltos_uint tick_count)
{
	/* TODO: Implement sleep code*/
	RLTOS_UNUSED(tick_count);
}
/* END OF FUNCTION*/

