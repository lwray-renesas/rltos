#include "rltos_port.h"

/** @brief Initialises & starts running the RLTOS tick timer (INTITL)*/
void (*Rltos_port_start_tick_timer)(void) = NULL;

/** @brief Stops the RLTOS tick timer (INTITL)*/
void (*Rltos_port_stop_tick_timer)(void) = NULL;

stack_ptr_type Rltos_port_stack_init(stack_ptr_type const p_stack_top, void* const p_task_func)
{
	/* local variable to navigate the stack*/
	stack_ptr_type l_p_stack_top = p_stack_top;

	--l_p_stack_top;
	*l_p_stack_top = (stack_type) (0xC600U); /* Initial PSW can be changed on MSB here*/
	--l_p_stack_top;
	*l_p_stack_top = (stack_type) (p_task_func);

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

void Rltos_port_idle_task_hook(void)
{
	/* Do Nothing*/
}
/* END OF FUNCTION*/
