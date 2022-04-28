#include "rltos_task.h"

static volatile uint16_t thread1_counter = 0U;

void Thread_1_main(void)
{
	while(1)
	{
		thread1_counter += 1U;
		Rltos_task_sleep(10U);
	}
}
/* END OF THREAD*/
