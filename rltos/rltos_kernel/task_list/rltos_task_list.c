/*
 * rltos_task_list.c
 *
 *  Created on: 22 Apr 2021
 *      Author: Louis Wray
 */

#include "rltos.h"
#include "rltos_task_list.h"

/** @brief Definition of task control structure*/
struct Task_ctl_t
{
	stack_type stored_sp; /* Stored value of the stack pointer*/
	void(*p_task_func)(void);	/* Function pointer for entry of point of task*/
	struct Task_ctl_t MEM_TYPE * p_next_tctl; /* Pointer to the next task control*/
	struct Task_ctl_t MEM_TYPE * p_prev_tctl; /* Pointer to the previous task control*/
};

/** @brief Struct definition containing all items required to operate a task list*/
struct Task_list_t
{
	p_task_ctl_t p_head;
	p_task_ctl_t p_index;
	rltos_uint size;
};

/** Running list*/
MEM_TYPE struct Task_list_t running_task_list = {
		.p_head = NULL,
		.p_index = NULL,
		.size = 0U
};

/** Idle list*/
MEM_TYPE struct Task_list_t idle_task_list = {
		.p_head = NULL,
		.p_index = NULL,
		.size = 0U
};
/** Pointer to idle task list*/

/** Pointer to current running task*/
p_task_ctl_t p_current_task_ctl	= NULL;

void Scheduler_init(void)
{
	p_current_task_ctl = running_task_list.p_head;
}
/* END OF FUNCTION*/

void Task_init(p_task_ctl_t const task_to_init, const stack_type init_sp, void(* const init_task_func)(void))
{
	task_to_init->p_task_func = init_task_func;
	task_to_init->stored_sp = init_sp;
	task_to_init->p_next_tctl = NULL;
	task_to_init->p_prev_tctl = NULL;
}
/* END OF FUNCTION*/

void Task_append_to_list(p_task_list_t const list_to_append, p_task_ctl_t const task_to_append)
{
	if(0U == list_to_append->size)
	{
		list_to_append->p_head = task_to_append;
		list_to_append->p_index = task_to_append;
		task_to_append->p_next_tctl = task_to_append;
		task_to_append->p_prev_tctl = task_to_append;
	}
	else
	{
		task_to_append->p_prev_tctl = list_to_append->p_head->p_prev_tctl;
		task_to_append->p_next_tctl = list_to_append->p_head;
		list_to_append->p_head->p_prev_tctl->p_next_tctl = task_to_append;
		list_to_append->p_head->p_prev_tctl = task_to_append;
	}

	list_to_append->size += 1U;
}
/* END OF FUNCTION*/

void Task_remove_from_list(p_task_list_t const list_for_remove, p_task_ctl_t const task_to_remove)
{
	if(list_for_remove->size > 0U)
	{
		list_for_remove->size -= 1U;
	}

	if((task_to_remove->p_next_tctl != NULL) && (task_to_remove->p_prev_tctl != NULL))
	{
		task_to_remove->p_prev_tctl->p_next_tctl = task_to_remove->p_next_tctl;
		task_to_remove->p_next_tctl->p_prev_tctl = task_to_remove->p_prev_tctl;

		if(list_for_remove->p_head == task_to_remove)
		{
			list_for_remove->p_head = task_to_remove->p_next_tctl;
		}

		if(list_for_remove->p_index == task_to_remove)
		{
			list_for_remove->p_index = task_to_remove->p_next_tctl;
		}

		task_to_remove->p_prev_tctl = NULL;
		task_to_remove->p_next_tctl = NULL;
	}
}
/* END OF FUNCTION*/

/**@brief implementation of rltos rescheduling algorithm - called from rltos_scheduler_asm.asm */
void Rltos_scheduler_impl(void)
{
	running_task_list.p_index = running_task_list.p_index->p_next_tctl;
	p_current_task_ctl = running_task_list.p_index;
}
/* END OF FUNCTION*/
