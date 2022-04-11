#include "rltos_task.h"

dummy_task_t task0;
stack_type task0_stack[256U] = {0U};

static volatile uint16_t thread0_counter = 0U;

void Thread_0_main(void);

void Thread_0_main(void)
{
	while(1)
	{
		thread0_counter += 1U;
		Rltos_task_sleep(10U);
	}
}
/* END OF THREAD*/
