/*
 * rltos_scheduler_asm_dummy.c
 *
 *  Created on: 22 Apr 2021
 *      Author: Louis Wray
 */

/** Make scheduler implementation available*/
extern void Rltos_scheduler_impl(void);

/** @brief emtpy function used to emulate the dummy functions*/
static void Save_context(void);

/** @brief emtpy function used to emulate the dummy functions*/
static void Restore_context(void);


/** @brief dummy definition of function referenced in source that is typically provided in asm*/
void Rltos_enter_first_task(void)
{
	Restore_context();
}
/* END OF FUNCTION*/

/** @brief dummy definition of function referenced in source that is typically provided in asm*/
void Rltos_yield(void)
{
	Save_context();
	Rltos_scheduler_impl();
	Restore_context();
}
/* END OF FUNCTION*/

/** @brief dummy definition of function referenced in source that is typically provided in asm*/
void Rltos_tick(void)
{
	Save_context();
	Rltos_scheduler_impl();
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
