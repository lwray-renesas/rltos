#include "rltos_port.h"

/** @brief Initialises task stack
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_f - function pointer to the task entry function.
 * @return returns the stack pointer when complete.*/
stack_ptr_type Rltos_stack_init(stack_ptr_type const p_stack_top, void(* const p_task_func)(void))
{
    /* msn = most significant nibble (top 4 bits of 20bit address)*/
	stack_type p_task_f_msn = (stack_type) ((((uint32_t)(p_task_func)) & 0xF0000UL) >> 16U);
	/* lsw = least significant word (bottom 16bit address)*/
	stack_type p_task_f_lsw = (stack_type) (p_task_func);

	/* local variable to navigate the stack*/
	stack_ptr_type l_p_stack_top = p_stack_top;

	--l_p_stack_top;
	*l_p_stack_top = p_task_f_msn | (stack_type) (0xC600U); /* Initial PSW can be changed on MSB here*/
	--l_p_stack_top;
	*l_p_stack_top = p_task_f_lsw;

	/* Init AX*/
	--l_p_stack_top;
	*l_p_stack_top = 0xAAAAU;
	/* Init BC*/
	--l_p_stack_top;
	*l_p_stack_top = 0xBBBBU;
	/* Init DE*/
	--l_p_stack_top;
	*l_p_stack_top = 0xDEDEU;
	/* Init HL*/
	--l_p_stack_top;
	*l_p_stack_top = 0x2222U;
	/* Init ES & CS*/
	--l_p_stack_top;
	*l_p_stack_top = 0x000FU; /* ES & CS*/

	return l_p_stack_top;
}
/* END OF FUNCTION*/
