#include "rltos_task.h"

dummy_task_t task1;
stack_type task1_stack[128U];

static volatile uint16_t thread1_counter = 0U;

void Thread_1_main(void);

void Thread_1_main(void)
{
	while(1)
	{
		thread1_counter += 1U;
		Rltos_task_sleep(100U);
	}
}
/* END OF THREAD*/
