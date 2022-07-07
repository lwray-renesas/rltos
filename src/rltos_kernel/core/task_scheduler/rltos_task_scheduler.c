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

/** List containing all temporarily blocked tasks*/
struct task_list_t idle_task_list = {
		NULL, /* Head*/
		NULL, /* Index*/
		0U /* Size*/
};

/** List containing all permanently blocked tasks*/
struct task_list_t stopped_task_list = {
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
bool should_switch_task = true;
/** Rltos idle task stack*/
static stack_type idle_task_stack[RLTOS_IDLE_TASK_STACK_SIZE];
/** Rltos idle task*/
static struct task_ctl_t idle_task_ctl;

/** @brief Idle task function - calls hook function on each execution*/
static void Rltos_idle_thread(void);

/** @brief Function used to insert a task in the idle list (sorted).
 * @details sorts in order from head to last, the tasks with shortest time till running.
 * @param[inout] task_to_insert - task to insert in list.
 */
static inline void Task_insert_in_idle_list(p_task_ctl_t const task_to_insert);

/** @brief Function used to append task to a task list
 * @param[inout] list_for_append - pointer to a task list for which the task should be appended.
 * @param[inout] task_to_append - task to append to list.
 * @param[in] list_index - index of the list to in which to append the task.
 */
static void Task_append_to_list(p_task_list_t const list_for_append, p_task_ctl_t const task_to_append, const list_index_t list_index);

/** @brief Function used to insert a task into a list directly after the index.
 * @param[inout] list_for_append - pointer to a task list for which the task should be inserted.
 * @param[inout] task_to_append - task to insert in the list.
 * @param[in] list_index - index of the list to in which to append the task.
 */
static void Task_insert_in_list_after_index(p_task_list_t const list_for_append, p_task_ctl_t const task_to_append, const list_index_t list_index);

/** @brief Function used to remove task from a task list
 * @param[inout] task_to_remove - task to remove from list.
 * @param[in] list_index - index of the list to in which to remove the task.
 */
static void Task_remove_from_list(p_task_ctl_t const task_to_remove, const list_index_t list_index);

void Rltos_scheduler_tick_inc(void)
{
	/* Increment system tick counter*/
	++rltos_system_tick;

	/* check for wraparound*/
	if (rltos_system_tick == 0U)
	{
		++rltos_wrap_count;
	}
}
/* END OF FUNCTION*/

void Rltos_scheduler_switch_context(void)
{
	/* [If idle tasks are present in idle task list.]
	 * AND
	 * [next idle tasks wrap counter matches system wrap counter.
	 * AND
	 * system tick count has expired the next idle tasks expiry time.]
	 */
	if ((idle_task_list.size > 0U) &&
			((rltos_wrap_count == idle_task_list.p_head->idle_ready_wrap_count) &&
			(rltos_system_tick >= idle_task_list.p_head->idle_ready_time)))
	{
		/* Head of list is always first, list is ordered such that the next expiration time is at the head */
		Task_set_running(idle_task_list.p_head);
	}

	/* If we need to switch task - do so, otherwise mark as needing to switch next time*/
	if (should_switch_task)
	{
		running_task_list.p_index = running_task_list.p_index->p_next_tctl[state_list];
	}
	else
	{
		should_switch_task = true;
	}

	/* If we are ever about to enter to idle task - try switch to the next active task - if the idle task is the only one available, the next item in the list will be the idle task anyway and we will enter it*/
	if (running_task_list.p_index == &idle_task_ctl)
	{
		running_task_list.p_index = running_task_list.p_index->p_next_tctl[state_list];
	}

	p_current_task_ctl = running_task_list.p_index;
}
/* END OF FUNCTION*/

/*! @} */

rltos_err_t Task_scheduler_init(void)
{
	rltos_err_t err = (sizeof(dummy_task_t) == sizeof(struct task_ctl_t) &&
						sizeof(dummy_task_list_t) == sizeof(struct task_list_t)) ? RLTOS_SUCCESS : RLTOS_MEMORY_ERR;

	if (RLTOS_SUCCESS == err)
	{
		RLTOS_PREPARE_CRITICAL_SECTION();

		RLTOS_ENTER_CRITICAL_SECTION();

		/* Manually reset system tick and wrap counters*/
		rltos_system_tick = 0U;
		rltos_wrap_count = 0U;

		/* Initialise the stack*/
		stack_ptr_type l_p_stack_top = Rltos_port_stack_init(&idle_task_stack[RLTOS_IDLE_TASK_STACK_SIZE - 1], &Rltos_idle_thread);

		/* Create the idle task and puit it in the running list*/
		Task_init(&idle_task_ctl, l_p_stack_top, &Rltos_idle_thread, true);

		/* Initialise the current task ctl pointer*/
		running_task_list.p_index = running_task_list.p_head;
		idle_task_list.p_index = idle_task_list.p_head;
		stopped_task_list.p_index = stopped_task_list.p_head;
		p_current_task_ctl = running_task_list.p_index;

		RLTOS_EXIT_CRITICAL_SECTION();
	}

	return err;
}
/* END OF FUNCTION*/

void Task_scheduler_deinit(void)
{
	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	/* Destroy the idle task*/
	Task_deinit(&idle_task_ctl);

	/* Destroy the current task control pointer*/
	p_current_task_ctl = NULL;

	RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

void Task_init(p_task_ctl_t const task_to_init, const stack_ptr_type init_sp, p_task_func_t const init_task_func, bool const task_is_running)
{
	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	/* Set the task function and stack pointer - then NULL init the list parameters*/
	task_to_init->p_task_func = init_task_func;
	task_to_init->stored_sp = init_sp;
	task_to_init->p_next_tctl[state_list] = NULL;
	task_to_init->p_next_tctl[aux_list] = NULL;
	task_to_init->p_prev_tctl[state_list] = NULL;
	task_to_init->p_prev_tctl[aux_list] = NULL;
	task_to_init->p_owners[state_list] = NULL;
	task_to_init->p_owners[aux_list] = NULL;

	task_to_init->idle_ready_time = RLTOS_UINT_MAX;
	task_to_init->idle_time = RLTOS_UINT_MAX;
	task_to_init->idle_ready_wrap_count = 0U;

	/* Set the state list ownership*/
	if (task_is_running)
	{
		Task_set_running(task_to_init);
	}
	else
	{
		Task_set_stopped(task_to_init);
	}

	RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

void Task_deinit(p_task_ctl_t const task_to_deinit)
{
	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	/* Set the task function and stack pointer - then NULL init the list parameters*/
	task_to_deinit->p_task_func = NULL;
	task_to_deinit->stored_sp = NULL;

	/* Remove from any lists*/
	Task_remove_from_list(task_to_deinit, state_list);
	Task_remove_from_list(task_to_deinit, aux_list);

	RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

void Task_list_init(p_task_list_t const list_to_init)
{
	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	list_to_init->size = 0U;
	list_to_init->p_head = NULL;
	list_to_init->p_index = NULL;

	RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

void Task_set_running(p_task_ctl_t const task_to_run)
{
	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	/* If we are owned by an object -> remove from objects list*/
	if (NULL != task_to_run->p_owners[aux_list])
	{
		Task_remove_from_list(task_to_run, aux_list);
	}

	/* Move task from idle state to running state and make it the next task to run*/
	Task_remove_from_list(task_to_run, state_list);
	Task_insert_in_list_after_index(&running_task_list, task_to_run, state_list);

	RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

void Task_resume(p_task_ctl_t const task_to_resume)
{
	/* Only set the task running if it is not waiting on an object*/
	if(task_to_resume->p_owners[aux_list] == NULL)
	{
		Task_set_running(task_to_resume);
	}
}
/* END OF FUNCTION*/

void Task_set_stopped(p_task_ctl_t const task_to_stop)
{
	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	/* If the task to stop is the current task - ensure the scheduler doesn't update the index on next run - it will be done automatically*/
	should_switch_task = !(running_task_list.p_index == task_to_stop);

	/* Move task into stopped task list - don't care about stop list order*/
	Task_remove_from_list(task_to_stop, state_list);
	Task_append_to_list(&stopped_task_list, task_to_stop, state_list);

	RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

void Task_set_current_idle(const rltos_uint time_to_idle)
{
	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	/* Calculate next expiration time*/
	p_current_task_ctl->idle_ready_time = rltos_system_tick + time_to_idle;
	p_current_task_ctl->idle_time = time_to_idle;

	/* Wraparound check*/
	if(p_current_task_ctl->idle_ready_time < rltos_system_tick)
	{
		p_current_task_ctl->idle_ready_wrap_count = rltos_wrap_count + 1U;
	}
	else
	{
		p_current_task_ctl->idle_ready_wrap_count = rltos_wrap_count;
	}

	Task_remove_from_list(p_current_task_ctl, state_list);
	Task_insert_in_idle_list(p_current_task_ctl);

	/* If we have just idled the current index task, the index has implicitly been updated - so the scheduler doesnt need to update the running list index on next run*/
	should_switch_task = false;

	RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

void Task_set_current_wait_on_object(p_task_list_t const owner, const rltos_uint time_to_wait)
{
	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	/* Idle the task if the time to wait is less than max*/
	if(RLTOS_UINT_MAX == time_to_wait)
	{
		Task_set_stopped(p_current_task_ctl);
	}
	else
	{
		Task_set_current_idle(time_to_wait);
	}

	/* Dont care about the order in which they are appended*/
	Task_append_to_list(owner, p_current_task_ctl, aux_list);

	RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

void Task_yield_if_current_task(p_task_ctl_t const task_to_check)
{
	if(task_to_check == p_current_task_ctl)
	{
		Rltos_task_yield();
	}
}
/* END OF FUNCTION*/

static inline void Task_insert_in_idle_list(p_task_ctl_t const task_to_insert)
{
	/* If first task*/
	if (0U == idle_task_list.size)
	{
		/* Make entire list circular to single task*/
		idle_task_list.p_head = task_to_insert;
		idle_task_list.p_index = task_to_insert;
		task_to_insert->p_next_tctl[state_list] = task_to_insert;
		task_to_insert->p_prev_tctl[state_list] = task_to_insert;
	}
	else
	{
		/* Belongs at head of the idle list if: the idle time is smaller than the remaining idle time of the current head.*/
		const bool belongs_at_head = (rltos_uint)(task_to_insert->idle_time) < (rltos_uint)(idle_task_list.p_head->idle_ready_time - rltos_system_tick);

		if(belongs_at_head)
		{
			/* Value belongs at the head/start of the list - Update the head*/
			task_to_insert->p_prev_tctl[state_list] = idle_task_list.p_head->p_prev_tctl[state_list];
			task_to_insert->p_next_tctl[state_list] = idle_task_list.p_head;
			idle_task_list.p_head->p_prev_tctl[state_list]->p_next_tctl[state_list] = task_to_insert;
			idle_task_list.p_head->p_prev_tctl[state_list] = task_to_insert;
			idle_task_list.p_head = task_to_insert;
		}
		else
		{
			/* Belongs at back of the idle list if: the idle time is larger than or equal to the remaining idle time of the current back.*/
			const bool belongs_at_back = (rltos_uint)(task_to_insert->idle_time) >= (rltos_uint)(idle_task_list.p_head->p_prev_tctl[state_list]->idle_ready_time - rltos_system_tick);

			if(belongs_at_back)
			{
				/* Otherwise insert at the end of the list*/
				task_to_insert->p_prev_tctl[state_list] = idle_task_list.p_head->p_prev_tctl[state_list];
				task_to_insert->p_next_tctl[state_list] = idle_task_list.p_head;
				idle_task_list.p_head->p_prev_tctl[state_list]->p_next_tctl[state_list] = task_to_insert;
				idle_task_list.p_head->p_prev_tctl[state_list] = task_to_insert;
			}
			else
			{
				/* Walk the list until we get to a list entry that belongs AFTER the current entry OR we get back to head*/
				p_task_ctl_t walk_value = idle_task_list.p_head->p_next_tctl[state_list];

				/* While the walk values remaining time is smaller than or equal to the idle time of the task to add.
				 * AND
				 * We are not back at head
				 */
				while( 	((rltos_uint)(walk_value->idle_ready_time - rltos_system_tick) <= (rltos_uint)(task_to_insert->idle_time)) &&
						(walk_value != running_task_list.p_head) )
				{
					walk_value = walk_value->p_next_tctl[state_list];
				}

				task_to_insert->p_prev_tctl[state_list] = walk_value->p_prev_tctl[state_list];
				task_to_insert->p_next_tctl[state_list] = walk_value;
				walk_value->p_prev_tctl[state_list]->p_next_tctl[state_list] = task_to_insert;
				walk_value->p_prev_tctl[state_list] = task_to_insert;
			}
		}
	}

	/* Set the new owner list*/
	task_to_insert->p_owners[state_list] = &idle_task_list;

	/* Increment list size*/
	idle_task_list.size += 1U;
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

static void Task_insert_in_list_after_index(p_task_list_t const list_for_append, p_task_ctl_t const task_to_append, const list_index_t list_index)
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
		task_to_append->p_prev_tctl[list_index] = list_for_append->p_index;
		task_to_append->p_next_tctl[list_index] = list_for_append->p_index->p_next_tctl[list_index];
		list_for_append->p_index->p_next_tctl[list_index]->p_prev_tctl[list_index] = task_to_append;
		list_for_append->p_index->p_next_tctl[list_index] = task_to_append;
	}

	/* Set the new owner list*/
	task_to_append->p_owners[list_index] = list_for_append;

	/* Increment list size*/
	list_for_append->size += 1U;
}
/* END OF FUNCTION*/

static void Task_remove_from_list(p_task_ctl_t const task_to_remove, const list_index_t list_index)
{
	/* Only operate on a list where the task is garuanteed to be owned by that list*/
	if (task_to_remove->p_owners[list_index] != NULL)
	{
		task_to_remove->p_owners[list_index]->size -= 1U;

		/* If last task in list*/
		if (task_to_remove->p_owners[list_index]->size == 0U)
		{
			/* Reset the list*/
			task_to_remove->p_owners[list_index]->p_head = NULL;
			task_to_remove->p_owners[list_index]->p_index = NULL;
		}
		else
		{
			/* Otherwise ensure lists connections are updated*/
			task_to_remove->p_prev_tctl[list_index]->p_next_tctl[list_index] = task_to_remove->p_next_tctl[list_index];
			task_to_remove->p_next_tctl[list_index]->p_prev_tctl[list_index] = task_to_remove->p_prev_tctl[list_index];

			if (task_to_remove->p_owners[list_index]->p_head == task_to_remove)
			{
				task_to_remove->p_owners[list_index]->p_head = task_to_remove->p_next_tctl[list_index];
			}

			if (task_to_remove->p_owners[list_index]->p_index == task_to_remove)
			{
				task_to_remove->p_owners[list_index]->p_index = task_to_remove->p_next_tctl[list_index];
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
		Rltos_task_yield();
	}
}
/* END OF FUNCTION*/
