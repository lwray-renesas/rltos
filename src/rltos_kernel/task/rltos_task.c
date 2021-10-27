/**
 * @addtogroup Rltos_task_prv prv
 * The private data (implementation) of the RLTOS kernels tasks.
 * @ingroup Rltos_task
 * @{
 */

#include "rltos_task.h"
#include "task_list/rltos_task_list.h"

/** @brief Function to enter the first rltos task.
 * @details Defined in rltos_kernel.asm - does not save context, only restores it and returns as though from interrupt (RETI).
 * This function MUST be called after initialising the current task pointer (p_current_task_ctl).
 */
extern void Rltos_enter_first_task(void);

/** @brief Initialises stack for creation of task.
 * This function is provided by the porting layer.
 * @return The value of the stack pointer post initialisation (used to restore context).
*/
extern stack_ptr_type Rltos_stack_init(stack_ptr_type p_stack_top, void *const p_task_func);

/** @brief Initialises and start RLTOS tick timer.
 * This function is provided by the application layer.
*/
extern void Rltos_start_tick_timer(void);

void Rltos_kernel_enter(void)
{
	Scheduler_init();
	Rltos_start_tick_timer();
	Rltos_enter_first_task();
}
/* END OF FUNCTION*/

void Rltos_task_create(p_dummy_task_t const task_to_create, stack_ptr_type const p_stack_top, p_task_func_t const p_task_func, rltos_uint const task_priority, bool const run_task)
{
	/* Initialise the stack*/
	stack_ptr_type l_p_stack_top = Rltos_stack_init(p_stack_top, p_task_func);

	/* Initialise the task*/
	Task_init((p_task_ctl_t)(task_to_create), l_p_stack_top, p_task_func, task_priority, run_task);
}
/* END OF FUNCTION*/

void Rltos_task_destroy(p_dummy_task_t const task_to_destroy)
{
	/* Denitialise the task*/
	Task_deinit((p_task_ctl_t)(task_to_destroy));
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
