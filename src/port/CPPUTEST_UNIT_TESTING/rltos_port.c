#include "rltos_port.h"

/** @brief Initialises task stack
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_func - function pointer to the task entry function.
 * @return returns the stack pointer when complete.*/
stack_ptr_type Rltos_port_stack_init(stack_ptr_type const p_stack_top, void* const p_task_func)
{
	/* Do Nothing*/
	(void)(p_task_func);
	return p_stack_top;
}
/* END OF FUNCTION*/

/** @brief Initialises & starts running the RLTOS tick timer (INTITL)*/
void Rltos_port_start_tick_timer(void)
{
	/* Do Nothing*/
}
/* END OF FUNCTION*/

/** @brief Stops running the RLTOS tick timer (INTITL)*/
void Rltos_port_stop_tick_timer(void)
{
	/* Do Nothing*/
}
/* END OF FUNCTION*/

void Rltos_port_idle_task_hook(void)
{
	/* DO NOTHING*/
}
/* END OF FUNCTION*/
