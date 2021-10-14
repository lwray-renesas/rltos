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

void Rltos_create_task(p_dummy_task_t const task_to_add, p_stack_type p_stack_top, void(* const p_task_func)(void))
{
	/* msn = most significant nibble (top 4 bits of 20bit address)*/
	stack_type p_task_f_msn = (stack_type) ((((uint32_t)(p_task_func)) & 0xF0000UL) >> 16U);
	/* lsw = least significant word (bottom 16bit address)*/
	stack_type p_task_f_lsw = (stack_type) (p_task_func);

	--p_stack_top;
	*p_stack_top = p_task_f_msn | (stack_type) (0xC600U);
	--p_stack_top;
	*p_stack_top = p_task_f_lsw;

	/* Init AX*/
	--p_stack_top;
	*p_stack_top = 0xAAAAU;
	/* Init BC*/
	--p_stack_top;
	*p_stack_top = 0xBBBBU;
	/* Init DE*/
	--p_stack_top;
	*p_stack_top = 0xDEDE;
	/* Init HL*/
	--p_stack_top;
	*p_stack_top = 0x2222U;
	/* Init ES & CS*/
	--p_stack_top;
	*p_stack_top = 0x000FU; /* ES & CS*/

	Init_task((p_task_ctl_t)(task_to_add), (stack_type)p_stack_top, p_task_func);

	/* Add task to running list*/
	Append_task_to_list(&running_task_list, (p_task_ctl_t )task_to_add);
}
/* END OF FUNCTION*/

void Rltos_enter_kernel(void)
{
	Init_current_task();
	Rltos_enter_first_task();
}
/* END OF FUNCTION*/

void Rltos_sleep(const rltos_uint tick_count)
{
	/* TODO: Implement sleep code*/
}
/* END OF FUNCTION*/

