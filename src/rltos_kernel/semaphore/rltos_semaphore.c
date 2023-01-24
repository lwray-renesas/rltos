/**
 * @addtogroup Rltos_semaphore_prv Semaphore Private
 * The private data (implementation) of the RLTOS semaphore.
 * @ingroup Rltos_semaphore_api
 * @{
 */

#include "rltos_task.h"
#include "task_scheduler/rltos_task_scheduler.h"
#include "rltos_semaphore.h"


void Rltos_semaphore_create(p_rltos_semaphore_t semaphore_to_create, const rltos_uint max_count, const rltos_uint initial_count, const p_rltos_task_t initial_owner)
{
	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	semaphore_to_create->max_count = max_count;
	Task_list_init(&semaphore_to_create->semaphore_task_list);

	if(NULL != initial_owner)
	{
		/* Acquire the semaphore*/
		semaphore_to_create->p_current_owner = initial_owner;
		semaphore_to_create->current_count = initial_count;
	}
	else
	{
		semaphore_to_create->current_count = 0U;
	}

	RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

rltos_err_t Rltos_semaphore_acquire(p_rltos_semaphore_t semaphore_to_acquire, const rltos_uint timeout)
{
	rltos_err_t err = RLTOS_SUCCESS;

	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

	if(0U == semaphore_to_acquire->current_count)
	{
		++semaphore_to_acquire->current_count;
		semaphore_to_acquire->p_current_owner = Task_get_current();

		RLTOS_EXIT_CRITICAL_SECTION();
	}
	else if(semaphore_to_acquire->p_current_owner == Task_get_current())
	{
		if(semaphore_to_acquire->current_count < semaphore_to_acquire->max_count)
		{
			++semaphore_to_acquire->current_count;
		}
		else
		{
			err = RLTOS_MAXIMUM;
		}

		RLTOS_EXIT_CRITICAL_SECTION();
	}
	else
	{
		if(0U == timeout)
		{
			err = RLTOS_LOCKED;

			RLTOS_EXIT_CRITICAL_SECTION();
		}
		else
		{
    		/* Two scenarios, with two possibilities each:
    		 * 1. Infinite wait
    		 * 		The semaphore is acquired & timeout is infinite.
    		 * 			We stay in the loop waiting for the semaphore to become available.
    		 * 			When it does we acquire it and return success.
    		 * 		The semaphore is released & timeout is infinite.
    		 * 			We break out of the loop, acquire the semaphore and return success.
    		 *
    		 * 2. Finite wait
    		 * 		The semaphore is acquired & timeout is finite.
    		 * 			We break out of loop & return timeout.
    		 * 		The semaphore is released & timeout is finite.
    		 * 			We break out of loop & acquire the semaphore and return success.
    		 */
    		do
    		{
            	Task_set_current_wait_on_object(&semaphore_to_acquire->semaphore_task_list, timeout);

                RLTOS_EXIT_CRITICAL_SECTION();

                Rltos_task_yield();

    		    RLTOS_ENTER_CRITICAL_SECTION();
    		}
    		while( (0U == semaphore_to_acquire->current_count) && (RLTOS_UINT_MAX == timeout) );

    		if(0U == semaphore_to_acquire->current_count)
    		{
    			++semaphore_to_acquire->current_count;
    			semaphore_to_acquire->p_current_owner = Task_get_current();

    	    	/* If any other tasks are waiting for the mutex, set them to running here.*/
    	    	for(uint16_t i = 0U; i < semaphore_to_acquire->semaphore_task_list.size; ++i)
    	    	{
    	    		Task_set_running(semaphore_to_acquire->semaphore_task_list.p_head);
    	    	}
    		}
    		else
    		{
    			err = RLTOS_TIMEOUT;
    		}

            RLTOS_EXIT_CRITICAL_SECTION();
		}
	}

	return err;
}
/* END OF FUNCTION*/

rltos_err_t Rltos_semaphore_release(p_rltos_semaphore_t semaphore_to_release)
{
	rltos_err_t err = RLTOS_SUCCESS;

	RLTOS_PREPARE_CRITICAL_SECTION();

	RLTOS_ENTER_CRITICAL_SECTION();

    if(0U == semaphore_to_release->current_count)
    {
    	err = RLTOS_RELEASED;
    }
    else
    {
    	/* If we are calling release and we do not own the mutex*/
    	if(semaphore_to_release->p_current_owner != Task_get_current())
    	{
    		err = RLTOS_INVALID_CONTEXT;
    	}
    	else
    	{
    		++semaphore_to_release->current_count;

        	/* If the semaphore is now unlocked - and there is a task waiting for it.*/
        	if( (0U == semaphore_to_release->current_count) && (NULL != semaphore_to_release->semaphore_task_list.p_head))
        	{
        		semaphore_to_release->p_current_owner = NULL;
        		Task_set_running(semaphore_to_release->semaphore_task_list.p_head);
        	}
    	}
    }

	RLTOS_EXIT_CRITICAL_SECTION();

	return err;
}
/* END OF FUNCTION*/

/*! @} */
