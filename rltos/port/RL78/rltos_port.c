#include "rltos_port.h"

p_stack_type Rltos_task_stack_init(p_stack_type p_stack_top, void(* const p_task_func)(void))
{
    /* msn = most significant nibble (top 4 bits of 20bit address)*/
	stack_type p_task_f_msn = (stack_type) ((((uint32_t)(p_task_func)) & 0xF0000UL) >> 16U);
	/* lsw = least significant word (bottom 16bit address)*/
	stack_type p_task_f_lsw = (stack_type) (p_task_func);

	--p_stack_top;
	*p_stack_top = p_task_f_msn | (stack_type) (0xC600U); /* Initial PSW can be changed on MSB here*/
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

	return p_stack_top;
}
/* END OF FUNCTION*/
