/**
 * @addtogroup Rltos_event_flags_prv Event Flags Private
 * The private data (implementation) of the RLTOS event flags.
 * @ingroup Rltos_event_flags_api
 * @{
 */

#include "rltos_task.h"
#include "task_scheduler/rltos_task_scheduler.h"
#include "rltos_events.h"


void Rltos_events_create(p_rltos_events_t events_to_create, const rltos_event_flag_t initial_flags)
{
	Task_list_init(&events_to_create->events_task_list);
	events_to_create->event_flags = initial_flags;
}
/* END OF FUNCTION*/

rltos_err_t Rltos_events_set(p_rltos_events_t events_to_set, const rltos_event_flag_t flags)
{
	rltos_err_t err = RLTOS_SUCCESS;

	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	/* If the bits intending on being set are already set*/
	if((events_to_set->event_flags & flags) >= flags)
	{
		err = RLTOS_ALREADY_SET;
	}
	else
	{
		/* Set the flags*/
		events_to_set->event_flags |= flags;

    	/* If the event is now set - and there is a task waiting for it - set it running.*/
		if(NULL != events_to_set->events_task_list.p_head)
		{
			Task_set_running(events_to_set->events_task_list.p_head);
		}
	}

	RLTOS_EXIT_CRITICAL_SECTION();

	return err;
}
/* END OF FUNCTION*/

rltos_err_t Rltos_events_get(p_rltos_events_t events_to_get, const rltos_event_flag_t flags, rltos_event_flag_t * output_flags, const rltos_flag_t clear_flags, const rltos_flag_t all_flags, const rltos_uint timeout)
{
	rltos_err_t err = RLTOS_SUCCESS;
	bool necessary_events_set = false;

	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	*output_flags = 0U;

	/* Check which event flags are necessary & are they set*/
	necessary_events_set = all_flags ? EVENTS_EXACT_OCCURED(events_to_get->event_flags, flags) : EVENTS_ANY_OCCURED(events_to_get->event_flags, flags);

	if(necessary_events_set)
	{
		*output_flags = events_to_get->event_flags;

		if(RLTOS_TRUE == clear_flags)
		{
			events_to_get->event_flags &= ~flags;
		}

		RLTOS_EXIT_CRITICAL_SECTION();
	}
	else
	{
		/* Two scenarios, with two possibilities each:
		 * 1. Infinite wait
		 * 		The events are not set & timeout is infinite.
		 * 			We stay in the loop waiting for the events to be set.
		 * 			When they do we take action and return success.
		 * 		The events are set & timeout is infinite.
		 * 			We break out of the loop, take action and return success.
		 *
		 * 2. Finite wait
		 * 		The events are not set & timeout is finite.
		 * 			We break out of loop & return timeout.
		 * 		The events are set & timeout is finite.
		 * 			We break out of loop & take action and return success.
		 */
		do
		{
			Task_set_current_wait_on_object(&events_to_get->events_task_list, timeout);

			RLTOS_EXIT_CRITICAL_SECTION();

			Rltos_task_yield();

			RLTOS_ENTER_CRITICAL_SECTION();

			/* Check which event flags are necessary & are they set*/
			necessary_events_set = all_flags ? EVENTS_EXACT_OCCURED(events_to_get->event_flags, flags) : EVENTS_ANY_OCCURED(events_to_get->event_flags, flags);
		}
		while( (!necessary_events_set) && (RLTOS_UINT_MAX == timeout) );

		if(necessary_events_set)
		{
			*output_flags = events_to_get->event_flags;

			/* If we have got the events and we need to clear the flags*/
			if(RLTOS_TRUE == clear_flags)
			{
				events_to_get->event_flags &= ~flags;
			}

			/* If any other tasks are waiting for the event flags, set them to running here.*/
			for(uint16_t i = 0U; i < events_to_get->events_task_list.size; ++i)
			{
				Task_set_running(events_to_get->events_task_list.p_head);
			}
		}
		else
		{
			err = RLTOS_TIMEOUT;
		}

		RLTOS_EXIT_CRITICAL_SECTION();
	}

	return err;
}
/* END OF FUNCTION*/

/*! @} */
