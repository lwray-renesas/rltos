/**
 * @addtogroup Rltos_task_prv Task Private
 * The private data (implementation) of the RLTOS kernels tasks.
 * @ingroup Rltos_task_api
 * @{
 */

#include "rltos_task.h"
#include "task_scheduler/rltos_task_scheduler.h"

/*! @} */

void Rltos_kernel_enter(void)
{
	/* Initialise the current task control pointer*/
	Task_scheduler_init();
	Rltos_port_start_tick_timer();
	Rltos_port_enter_first_task();
}
/* END OF FUNCTION*/

void Rltos_task_create(p_dummy_task_t const task_to_create, stack_ptr_type const p_stack_top, p_task_func_t const p_task_func, rltos_uint const task_priority, bool const run_task)
{
	/* Initialise the stack*/
	stack_ptr_type l_p_stack_top = Rltos_port_stack_init(p_stack_top, p_task_func);

	/* Initialise the task*/
	/* cppcheck-suppress misra-c2012-11.3 - Only way to allow for static allocation of objects while still hiding implementation - sizes gauranteed to be same*/
	Task_init((p_task_ctl_t)(task_to_create), l_p_stack_top, p_task_func, task_priority, run_task);
}
/* END OF FUNCTION*/

void Rltos_task_destroy(p_dummy_task_t const task_to_destroy)
{
	/* Denitialise the task*/
	/* cppcheck-suppress misra-c2012-11.3 - Only way to allow for static allocation of objects while still hiding implementation - sizes gauranteed to be same*/
	Task_deinit((p_task_ctl_t)(task_to_destroy));
}
/* END OF FUNCTION*/

void Rltos_task_sleep(const rltos_uint tick_count)
{
	if (tick_count > 0U)
	{
		/* Idle the current task*/
		Task_set_current_idle(tick_count);
	}

	/* Force yield the task*/
	Rltos_task_yield();
}
/* END OF FUNCTION*/
