#include "rltos_task.h"

/* For this example to work the following must be executed:
 * 1. Add the path to your rltos_port.h file to the compiler include path list.
 * 2. Add the path to the rltos_task.h file to the compiler include path list.
 * 3. Define the function Init_hardware and ensure it initialises the function pointers Rltos_port_start_tick_timer & Rltos_port_stop_tick_timer.
 * 4. Ensure the tick interrupt handler Rltos_port_tick is installed to the relevant vector table entry.
 * 5. [PORT DEPENDENT] Ensure the yield interrupt handler (when implemented through software interrupt/brk) is installed to the relevant vector table entry.
 */

/* Task 0 Components - defined in thread0.c*/
rltos_task_t task0;
stack_type task0_stack[64U] = {0U};
extern void Thread_0_main(void);

/* Task 1 Components - defined in thread1.c*/
rltos_task_t task1;
stack_type task1_stack[64U] = {0U};
extern void Thread_1_main(void);

/* Fucntion for initialising Rltos_port_start_tick_timer & Rltos_port_stop_tick_timer function pointers + any other necessary hw setup*/
extern void Init_hardware(void);

int main(void)
{
	/* Initialise all necessary hardware and tick timer API*/
	Init_hardware();

	/* Create the tasks*/
	Rltos_task_create(&task0, &task0_stack[63U], Thread_0_main, true);
	Rltos_task_create(&task1, &task1_stack[63U], Thread_1_main, true);

	/* Start the kernel*/
	Rltos_kernel_enter();

	/* Should Never Get Here*/
	return 0;
}
