/**
 * @addtogroup Rltos_kernel_prv prv
 * The private data (implementation) of the RLTOS kernel.
 * @ingroup Rltos_kernel
 * @{
 */

#include "rltos.h"
#include "task_list/rltos_task_list.h"

/** @brief Function to enter the first rltos task.
 * @details Defined in rltos_kernel.asm - does not save context, only restores it and returns as though from interrupt (RETI).
 * This function MUST be called after initialising the current task pointer (p_current_task_ctl).
 */
extern void Rltos_enter_first_task(void);

/** Get stack initialisation function from port file*/
extern stack_ptr_type Rltos_stack_init(stack_ptr_type p_stack_top, void (*const p_task_func)(void));

/** Get timer initialsation and start function from user application*/
extern void Rltos_start_tick_timer(void);

void Rltos_kernel_enter(void)
{
	Scheduler_init();
	Rltos_start_tick_timer();
	Rltos_enter_first_task();
}
/* END OF FUNCTION*/

void Rltos_task_create(p_dummy_task_t const task_to_add, stack_ptr_type p_stack_top, void (*const p_task_func)(void), rltos_uint task_priority, bool run_task)
{
	/* Initialise the stack*/
	p_stack_top = Rltos_stack_init(p_stack_top, p_task_func);

	/* Initialise the task*/
	Task_init((p_task_ctl_t)(task_to_add), p_stack_top, p_task_func, task_priority, run_task);
}
/* END OF FUNCTION*/

void Rltos_task_sleep(const rltos_uint tick_count)
{
	if (tick_count > 0U)
	{
		/* Idle the current task*/
		Task_set_current_idle(tick_count);

		/* Force yield the task*/
		Rltos_task_yield();
	}
}
/* END OF FUNCTION*/

/*! @} */
