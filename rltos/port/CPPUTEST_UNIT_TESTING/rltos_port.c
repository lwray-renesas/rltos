#include "rltos_port.h"

p_stack_type Rltos_task_stack_init(p_stack_type p_stack_top, void(* const p_task_func)(void))
{
	/* Do Nothing*/
	(void)(p_stack_top);
	return p_stack_top;
}
/* END OF FUNCTION*/