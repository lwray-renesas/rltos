/*
 * rltos_scheduler_asm_dummy.c
 *
 *  Created on: 22 Apr 2021
 *      Author: Louis Wray
 */

/** Make scheduler implementation available*/
extern void Rltos_scheduler_switch_context(void);
extern void Rltos_scheduler_tick_inc(void);

/** @brief empty function used to emulate the dummy functions*/
static void Save_context(void);

/** @brief empty function used to emulate the dummy functions*/
static void Restore_context(void);

void Rltos_port_enter_first_task(void)
{
	Restore_context();
}
/* END OF FUNCTION*/

void Rltos_port_yield(void)
{
	Save_context();
	Rltos_scheduler_switch_context();
	Restore_context();
}
/* END OF FUNCTION*/

void Rltos_port_tick(void)
{
	Save_context();
	Rltos_scheduler_tick_inc();
	Rltos_scheduler_switch_context();
	Restore_context();
}
/* END OF FUNCTION*/

static void Save_context(void)
{
	
}
/* END OF FUNCTION*/

static void Restore_context(void)
{
	
}
/* END OF FUNCTION*/
