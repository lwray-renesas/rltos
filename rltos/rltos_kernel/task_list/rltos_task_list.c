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
	/* Task specific data*/
	stack_ptr_type stored_sp;				 	/* Stored value of the stack pointer*/
	void (*p_task_func)(void);				 	/* Function pointer for entry of point of task*/
	rltos_uint	sleep_expiration_time;			/* Value representing the time this tasks sleep status expires*/

	/* List specific data*/
	struct Task_ctl_t MEM_TYPE *p_next_tctl[2]; 	/* Pointer to the next item - can exist in two lists at once*/
	struct Task_ctl_t MEM_TYPE *p_prev_tctl[2]; 	/* Pointer to the previous item - can exist in two lists at once*/
	rltos_uint sorting_value[2];				 	/* value used to sort the list*/
};

/** @brief Struct definition containing all items required to operate a task list*/
struct Task_list_t
{
	p_task_ctl_t p_head;						/* Head of list*/
	p_task_ctl_t p_index;						/* current index of list*/
	rltos_uint size;							/* Size of list*/
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

/** Pointer to current running task*/
p_task_ctl_t p_current_task_ctl = NULL;

/** Rltos system tick counter*/
rltos_uint	rltos_system_tick = 0U;

void Scheduler_init(void)
{
	/* Scheduler initialisation consists of setting the pointer to the current operating task*/
	p_current_task_ctl = running_task_list.p_head;
}
/* END OF FUNCTION*/

void Task_init(p_task_ctl_t const task_to_init, const stack_ptr_type init_sp, void (*const init_task_func)(void))
{
	/* Set the task function and stack pointer - then NULL init the list parameters*/
	task_to_init->p_task_func = init_task_func;
	task_to_init->stored_sp = init_sp;
	task_to_init->p_next_tctl[state_list] = NULL;
	task_to_init->p_next_tctl[aux_list] = NULL;
	task_to_init->p_prev_tctl[state_list] = NULL;
	task_to_init->p_prev_tctl[aux_list] = NULL;
}
/* END OF FUNCTION*/

void Task_list_init(p_task_list_t const list_to_init, p_task_ctl_t const first_task, const list_index_t list_index)
{
	list_to_init->size = 0U;

	/* If we have not been passed a first task to add - NULL init list*/
	if (NULL == first_task)
	{
		list_to_init->p_head = NULL;
		list_to_init->p_index = NULL;
	}
	else
	{
		/* Otherwise append the first task to the list*/
		Task_insert_in_list(list_to_init, first_task, list_index);
	}
}
/* END OF FUNCTION*/

void Task_insert_in_list(p_task_list_t const list_for_insert, p_task_ctl_t const task_to_insert, const list_index_t list_index)
{
	/* If first task*/
	if (0U == list_for_insert->size)
	{
		/* Make entire list circular to single task*/
		list_for_insert->p_head = task_to_insert;
		list_for_insert->p_index = task_to_insert;
		task_to_insert->p_next_tctl[list_index] = task_to_insert;
		task_to_insert->p_prev_tctl[list_index] = task_to_insert;
	}
	else
	{
		/* Otherwise insert at the end of the list*/
		task_to_insert->p_prev_tctl[list_index] = list_for_insert->p_head->p_prev_tctl[list_index];
		task_to_insert->p_next_tctl[list_index] = list_for_insert->p_head;
		list_for_insert->p_head->p_prev_tctl[list_index]->p_next_tctl[list_index] = task_to_insert;
		list_for_insert->p_head->p_prev_tctl[list_index] = task_to_insert;
	}

	/* Increment list size*/
	list_for_insert->size += 1U;
}
/* END OF FUNCTION*/

void Task_remove_from_list(p_task_list_t const list_for_remove, p_task_ctl_t const task_to_remove, const list_index_t list_index)
{
	/* Only operate on a list with non-zero size*/
	if (list_for_remove->size > 0U)
	{
		list_for_remove->size -= 1U;

		/* If last task in list*/
		if(list_for_remove->size == 0U)
		{
			/* Reset the list*/
			list_for_remove->p_head = NULL;
			list_for_remove->p_index = NULL;
		}
		else
		{
			/* Otherwise ensure lists connections are updated*/
			task_to_remove->p_prev_tctl[list_index]->p_next_tctl[list_index] = task_to_remove->p_next_tctl[list_index];
			task_to_remove->p_next_tctl[list_index]->p_prev_tctl[list_index] = task_to_remove->p_prev_tctl[list_index];

			if (list_for_remove->p_head == task_to_remove)
			{
				list_for_remove->p_head = task_to_remove->p_next_tctl[list_index];
			}

			if (list_for_remove->p_index == task_to_remove)
			{
				list_for_remove->p_index = task_to_remove->p_next_tctl[list_index];
			}
		}

		/* Remove tasks next and previous entries*/
		task_to_remove->p_prev_tctl[list_index] = NULL;
		task_to_remove->p_next_tctl[list_index] = NULL;
	}
}
/* END OF FUNCTION*/

/**@brief implementation of rltos rescheduling algorithm - called from rltos_scheduler_asm.asm */
void Rltos_scheduler_impl(void)
{
	++rltos_system_tick;
	running_task_list.p_index = running_task_list.p_index->p_next_tctl[state_list];
	p_current_task_ctl = running_task_list.p_index;
}
/* END OF FUNCTION*/
