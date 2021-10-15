#include "rltos_port.h"

/** @brief Initialises task stack
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_f - function pointer to the task entry function.
 * @return returns the stack pointer when complete.*/
p_stack_type Rltos_stack_init(p_stack_type p_stack_top, void(* const p_task_func)(void))
{
	/* Do Nothing*/
	(void)(p_task_func);
	return p_stack_top;
}
/* END OF FUNCTION*/
