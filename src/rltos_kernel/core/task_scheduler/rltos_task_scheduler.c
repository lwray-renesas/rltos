/**
 * @addtogroup Rltos_task_scheduler_prv Task Scheduler Private
 * The private data (implementation) of the RLTOS task list.
 * @ingroup Rltos_task_scheduler_api
 * @{
 */

#include "rltos_task.h"
#include "rltos_task_scheduler_prv.h"

/** List containing all running taks*/
struct task_list_t running_task_list = {
	NULL, /* Head*/
	NULL, /* Index*/
	0U /* Size*/
};

/** List containing all idle/blocked tasks*/
struct task_list_t idle_task_list = {
	NULL, /* Head*/
	NULL, /* Index*/
	0U /* Size*/
};
/** Pointer to current running task*/
p_task_ctl_t p_current_task_ctl = NULL;

/** Rltos system tick counter*/
volatile rltos_uint rltos_system_tick = 0U;
/** Rltos wrap around tracker*/
volatile rltos_uint rltos_wrap_count = 0U;
/** Rltos next time to remove task from idle list*/
rltos_uint rltos_next_idle_ready_tick = RLTOS_UINT_MAX;
/** Rltos wrap next idle tasks wrap around tracker*/
rltos_uint rltos_next_idle_ready_wrap_count = 0U;
/** Rltos next time to remove task from idle list*/
bool should_switch_task = true;
/** Rltos idle task stack*/
static stack_type idle_task_stack[32];
/** Rltos idle task*/
static struct task_ctl_t idle_task_ctl;

/** @brief Idle task function - calls hook function on each execution*/
static void Rltos_idle_thread(void);

/** @brief Function used to insert a task in a sorted task list
 * @param[inout] list_for_append - pointer to a task list for which the task should be inserted.
 * @param[inout] task_to_append - task to insert in list.
 * @param[in] list_index - index of the list to in which to insert the task.
 * @param[in] sorting_value - value used to sort list order.
 */
static void Task_insert_in_list(p_task_list_t const list_for_insert, p_task_ctl_t const task_to_insert, const list_index_t list_index, const rltos_uint sorting_value);

/** @brief Function used to append task to a task list
 * @param[inout] list_for_append - pointer to a task list for which the task should be appended.
 * @param[inout] task_to_append - task to append to list.
 * @param[in] list_index - index of the list to in which to append the task.
 */
static void Task_append_to_list(p_task_list_t const list_for_append, p_task_ctl_t const task_to_append, const list_index_t list_index);

/** @brief Function used to remove task from a task list
 * @param[inout] list_for_remove - pointer to a task list from which the task should be removed.
 * @param[inout] task_to_remove - task to remove from list.
 * @param[in] list_index - index of the list to in which to remove the task.
 */
static void Task_remove_from_list(p_task_list_t const list_for_remove, p_task_ctl_t const task_to_remove, const list_index_t list_index);

void Rltos_scheduler_tick_inc(void)
{
	/* Increment system tick counter*/
	++rltos_system_tick;

	/* check for wraparound*/
	if (rltos_system_tick == 0U)
	{
		++rltos_wrap_count;
	}

	/* While idle tasks are present in idle task list.
	* AND 
	* next idle tasks wrap counter matches system wrap counter.
	* AND 
	* system tick count has expired the next idles tasks expiry time.
	*/
	while ((idle_task_list.size > 0U) &&
		   (rltos_wrap_count == rltos_next_idle_ready_wrap_count) &&
		   (rltos_system_tick >= rltos_next_idle_ready_tick))
	{
		/* Head of list is always first, list is ordered such that the next expiration time is at the head
		* Function also updates the system parameters rltos_next_idle_ready_wrap_count and rltos_next_idle_ready_tick
		*/
		Task_set_running(idle_task_list.p_head);
	}
}
/* END OF FUNCTION*/

void Rltos_scheduler_switch_context(void)
{
	/* If we need to switch task - do so, otherwise mark as needing to switch next time*/
	if (should_switch_task)
	{
		running_task_list.p_index = running_task_list.p_index->p_next_tctl[state_list];
	}
	else
	{
		should_switch_task = true;
	}

	p_current_task_ctl = running_task_list.p_index;
}
/* END OF FUNCTION*/

/*! @} */

void Task_scheduler_init(void)
{
	/* Initialise the stack*/
	stack_ptr_type l_p_stack_top = Rltos_port_stack_init(idle_task_stack, &Rltos_idle_thread);

	/* Create the idle task and puit it in the running list*/
	Task_init(&idle_task_ctl, l_p_stack_top, &Rltos_idle_thread, RLTOS_UINT_MAX, true);

	/* Initialise the current task ctl pointer*/
	p_current_task_ctl = running_task_list.p_head;
}
/* END OF FUNCTION*/

void Task_scheduler_deinit(void)
{
	/* Create the idle task and puit it in the running list*/
	Task_deinit(&idle_task_ctl);

	/* Initialise the current task ctl pointer*/
	p_current_task_ctl = NULL;
}
/* END OF FUNCTION*/

void Task_init(p_task_ctl_t const task_to_init, const stack_ptr_type init_sp, p_task_func_t const init_task_func, rltos_uint const priority, bool const task_is_running)
{
	/* Set the task function and stack pointer - then NULL init the list parameters*/
	task_to_init->p_task_func = init_task_func;
	task_to_init->stored_sp = init_sp;
	task_to_init->p_next_tctl[state_list] = NULL;
	task_to_init->p_next_tctl[aux_list] = NULL;
	task_to_init->p_prev_tctl[state_list] = NULL;
	task_to_init->p_prev_tctl[aux_list] = NULL;
	task_to_init->p_owners[state_list] = NULL;
	task_to_init->p_owners[aux_list] = NULL;

	task_to_init->priority = priority;
	task_to_init->idle_ready_time = RLTOS_UINT_MAX;
	task_to_init->idle_time = RLTOS_UINT_MAX;
	task_to_init->idle_wrap_count = 0U;

	/* Set the state list ownership*/
	if (task_is_running)
	{
		Task_insert_in_list(&running_task_list, task_to_init, state_list, task_to_init->priority);
	}
	else
	{
		Task_insert_in_list(&idle_task_list, task_to_init, state_list, task_to_init->idle_time);
	}
}
/* END OF FUNCTION*/

void Task_deinit(p_task_ctl_t const task_to_deinit)
{
	const bool was_head_of_idle_list = idle_task_list.p_head == task_to_deinit;

	/* Set the task function and stack pointer - then NULL init the list parameters*/
	task_to_deinit->p_task_func = NULL;
	task_to_deinit->stored_sp = NULL;

	/* Remove from any lists*/
	Task_remove_from_list(task_to_deinit->p_owners[state_list], task_to_deinit, state_list);
	Task_remove_from_list(task_to_deinit->p_owners[aux_list], task_to_deinit, aux_list);

	if (was_head_of_idle_list)
	{
		/* If the idle task list is empty - invalidate the next time variable*/
		if (0U == idle_task_list.size)
		{
			rltos_next_idle_ready_wrap_count = RLTOS_UINT_MAX;
			rltos_next_idle_ready_tick = RLTOS_UINT_MAX;
		}
		else
		{
			/* Otherwise update it*/
			rltos_next_idle_ready_wrap_count = idle_task_list.p_head->idle_wrap_count;
			rltos_next_idle_ready_tick = idle_task_list.p_head->idle_ready_time;
		}
	}
}
/* END OF FUNCTION*/

void Task_list_init(p_task_list_t const list_to_init)
{
	list_to_init->size = 0U;
	list_to_init->p_head = NULL;
	list_to_init->p_index = NULL;
}
/* END OF FUNCTION*/

void Task_set_running(p_task_ctl_t const task_to_run)
{
	const bool was_head_of_idle_list = idle_task_list.p_head == task_to_run;

	/* If we are owned by an object -> remove from objects list*/
	if (NULL != task_to_run->p_owners[aux_list])
	{
		Task_remove_from_list(task_to_run->p_owners[aux_list], task_to_run, aux_list);
	}

	/* Move task from idle state to running state*/
	Task_remove_from_list(&idle_task_list, task_to_run, state_list);
	Task_insert_in_list(&running_task_list, task_to_run, state_list, task_to_run->priority);

	if (was_head_of_idle_list)
	{
		/* If the idle task list is empty - invalidate the next time variable*/
		if (0U == idle_task_list.size)
		{
			rltos_next_idle_ready_wrap_count = RLTOS_UINT_MAX;
			rltos_next_idle_ready_tick = RLTOS_UINT_MAX;
		}
		else
		{
			/* Otherwise update it*/
			rltos_next_idle_ready_wrap_count = idle_task_list.p_head->idle_wrap_count;
			rltos_next_idle_ready_tick = idle_task_list.p_head->idle_ready_time;
		}
	}
}
/* END OF FUNCTION*/

void Task_set_current_idle(const rltos_uint time_to_idle)
{
	/* Calculate next expiration time*/
	p_current_task_ctl->idle_ready_time = rltos_system_tick + time_to_idle;
	p_current_task_ctl->idle_time = time_to_idle;
	/* Wraparound check*/
	if(p_current_task_ctl->idle_ready_time < rltos_system_tick)
	{
		p_current_task_ctl->idle_wrap_count = rltos_wrap_count + 1U;
	}
	else
	{
		p_current_task_ctl->idle_wrap_count = rltos_wrap_count;
	}

	/* If list is empty - just fill parameters*/
	if (idle_task_list.size == 0U)
	{
		rltos_next_idle_ready_tick = p_current_task_ctl->idle_ready_time;
		rltos_next_idle_ready_wrap_count = p_current_task_ctl->idle_wrap_count;
	}
	else
	{
		if ((p_current_task_ctl->idle_wrap_count + 1U) == rltos_next_idle_ready_wrap_count)
		{
			/* The current task calculated wrap count is before the next one*/
			/* Idle tick count can only wrap around once, so we check one wrap ahead*/
			rltos_next_idle_ready_tick = p_current_task_ctl->idle_ready_time;
			rltos_next_idle_ready_wrap_count = p_current_task_ctl->idle_wrap_count;
		}
		else if (p_current_task_ctl->idle_wrap_count == rltos_next_idle_ready_wrap_count)
		{
			/* Equal wrap counts - check if computed expiration tick time is quicker */
			if (p_current_task_ctl->idle_ready_time < rltos_next_idle_ready_tick)
			{
				rltos_next_idle_ready_tick = p_current_task_ctl->idle_ready_time;
			}
		}
		else
		{
			/* computed wrap count is too large - cannot possibly be next in line*/
		}
	}

	Task_remove_from_list(&running_task_list, p_current_task_ctl, state_list);
	Task_insert_in_list(&idle_task_list, p_current_task_ctl, state_list, p_current_task_ctl->idle_time);

	/* If we have just idled the current index task, the index has implicitly been updated - so the scheduler doesnt need to update the running list index on next run*/
	should_switch_task = false;
}
/* END OF FUNCTION*/

void Task_set_current_wait_on_object(p_task_list_t const owner, const rltos_uint time_to_wait)
{
	/* Idle the task until it is told to run, either by the scheduler or the owning objects signal*/
	Task_set_current_idle(time_to_wait);

	/* Dont care about the order in which they are appended*/
	Task_append_to_list(owner, p_current_task_ctl, aux_list);
}
/* END OF FUNCTION*/

static void Task_insert_in_list(p_task_list_t const list_for_insert, p_task_ctl_t const task_to_insert, const list_index_t list_index, const rltos_uint sorting_value)
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
		/* If the value belongs at the end of the list, append it*/ 
		if(sorting_value >= list_for_insert->p_head->p_prev_tctl[list_index]->sorting_values[list_index])
		{
			task_to_insert->p_prev_tctl[list_index] = list_for_insert->p_head->p_prev_tctl[list_index];
			task_to_insert->p_next_tctl[list_index] = list_for_insert->p_head;
			list_for_insert->p_head->p_prev_tctl[list_index]->p_next_tctl[list_index] = task_to_insert;
			list_for_insert->p_head->p_prev_tctl[list_index] = task_to_insert;
		}
		else if(sorting_value < list_for_insert->p_head->sorting_values[list_index])
		{
			/* Update the head*/
			task_to_insert->p_prev_tctl[list_index] = list_for_insert->p_head->p_prev_tctl[list_index];
			task_to_insert->p_next_tctl[list_index] = list_for_insert->p_head;
			list_for_insert->p_head->p_prev_tctl[list_index]->p_next_tctl[list_index] = task_to_insert;
			list_for_insert->p_head->p_prev_tctl[list_index] = task_to_insert;
			list_for_insert->p_head = task_to_insert;
		}
		else
		{
			/* Walk the list until we get to a list entry that belongs AFTER the current entry OR we get back to head*/
			p_task_ctl_t walk_value = list_for_insert->p_head->p_next_tctl[list_index];
			while( (walk_value->sorting_values[list_index] <= sorting_value) && (walk_value != list_for_insert->p_head) )
			{
				walk_value = walk_value->p_next_tctl[list_index];
			}
			
			task_to_insert->p_prev_tctl[list_index] = walk_value->p_prev_tctl[list_index];
			task_to_insert->p_next_tctl[list_index] = walk_value;
			walk_value->p_prev_tctl[list_index]->p_next_tctl[list_index] = task_to_insert;
			walk_value->p_prev_tctl[list_index] = task_to_insert;
		}
	}

	/* Set the new owner list*/
	task_to_insert->p_owners[list_index] = list_for_insert;
	/* Set the new sorting value*/
	task_to_insert->sorting_values[list_index] = sorting_value;

	/* Increment list size*/
	list_for_insert->size += 1U;
}
/* END OF FUNCTION*/

static void Task_append_to_list(p_task_list_t const list_for_append, p_task_ctl_t const task_to_append, const list_index_t list_index)
{
	/* If first task*/
	if (0U == list_for_append->size)
	{
		/* Make entire list circular to single task*/
		list_for_append->p_head = task_to_append;
		list_for_append->p_index = task_to_append;
		task_to_append->p_next_tctl[list_index] = task_to_append;
		task_to_append->p_prev_tctl[list_index] = task_to_append;
	}
	else
	{
		/* Otherwise insert at the end of the list*/
		task_to_append->p_prev_tctl[list_index] = list_for_append->p_head->p_prev_tctl[list_index];
		task_to_append->p_next_tctl[list_index] = list_for_append->p_head;
		list_for_append->p_head->p_prev_tctl[list_index]->p_next_tctl[list_index] = task_to_append;
		list_for_append->p_head->p_prev_tctl[list_index] = task_to_append;
	}

	/* Set the new owner list*/
	task_to_append->p_owners[list_index] = list_for_append;

	/* Increment list size*/
	list_for_append->size += 1U;
}
/* END OF FUNCTION*/

static void Task_remove_from_list(p_task_list_t const list_for_remove, p_task_ctl_t const task_to_remove, const list_index_t list_index)
{
	/* Only operate on a list where the task is garuanteed to be owned by that list*/
	if (Task_is_in_list(list_for_remove, task_to_remove, list_index))
	{
		list_for_remove->size -= 1U;

		/* If last task in list*/
		if (list_for_remove->size == 0U)
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

		/* Remove list as owner*/
		task_to_remove->p_owners[list_index] = NULL;

		/* Remove tasks next and previous entries*/
		task_to_remove->p_prev_tctl[list_index] = NULL;
		task_to_remove->p_next_tctl[list_index] = NULL;
	}
}
/* END OF FUNCTION*/

static void Rltos_idle_thread(void)
{
	while(1)
	{
		Rltos_port_idle_task_hook();
	}
}
/* END OF FUNCTION*/
