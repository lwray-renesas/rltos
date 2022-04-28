#include "rltos_task.h"

static volatile uint16_t thread0_counter = 0U;

void Thread_0_main(void)
{
	while(1)
	{
		thread0_counter += 1U;
		Rltos_task_sleep(5U);
	}
}
/* END OF THREAD*/
