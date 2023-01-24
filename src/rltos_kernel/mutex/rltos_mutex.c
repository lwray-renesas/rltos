/**
 * @addtogroup Rltos_mutex_prv Mutex Private
 * The private data (implementation) of the RLTOS mutex.
 * @ingroup Rltos_mutex_api
 * @{
 */

#include "rltos_task.h"
#include "task_scheduler/rltos_task_scheduler.h"
#include "rltos_mutex.h"

/** @brief On successful locking of the mutex we can set the owner
 * @param[inout] mutex_to_set - pointer to mutex to work on.
 * @param[in] owner - pointer to task which now owns the mutex.*/
static void Set_mutex_owner(p_rltos_mutex_t mutex_to_set, const p_rltos_task_t owner);

/** @brief On successful release of the mutex we can clear the owner
 * @param[inout] mutex_to_clear - pointer to mutex to work on.*/
static void Clear_mutex_owner(p_rltos_mutex_t mutex_to_clear);

void Rltos_mutex_create(p_rltos_mutex_t mutex_to_create, const rltos_flag_t is_recursive, const p_rltos_task_t initial_owner)
{
    RLTOS_PREPARE_CRITICAL_SECTION();

    RLTOS_ENTER_CRITICAL_SECTION();

    mutex_to_create->p_current_owner = NULL;
    mutex_to_create->is_recursive = is_recursive;
    mutex_to_create->nesting_count = 0U;
    Task_list_init(&mutex_to_create->mutex_task_list);
    
    if(NULL != initial_owner)
    {
        /* Take the mutex in the current thread context - guaranteed successful on creation*/
        Set_mutex_owner(mutex_to_create, initial_owner);
    }

    RLTOS_EXIT_CRITICAL_SECTION();
}
/* END OF FUNCTION*/

rltos_err_t Rltos_mutex_lock(p_rltos_mutex_t mutex_to_lock, rltos_uint timeout)
{
	rltos_err_t err = RLTOS_SUCCESS;

    RLTOS_PREPARE_CRITICAL_SECTION();

    RLTOS_ENTER_CRITICAL_SECTION();

    /* The mutex is not locked OR the mutex is recursive and the calling thread is the current owner*/
    if((RLTOS_FALSE == mutex_to_lock->state) || ((RLTOS_TRUE == mutex_to_lock->is_recursive) && (mutex_to_lock->p_current_owner == Task_get_current())))
    {
    	Set_mutex_owner(mutex_to_lock, Task_get_current()); /* Set the owner i.e. lock the mutex*/

        RLTOS_EXIT_CRITICAL_SECTION();
    }
    else
    {
    	/* If no wait condition, return failed*/
    	if(0U == timeout)
    	{
    		err = RLTOS_LOCKED;

            RLTOS_EXIT_CRITICAL_SECTION();
    	}
    	else
    	{
    		/* Two scenarios, with two possibilities each:
    		 * 1. Infinite wait
    		 * 		The mutex is locked & timeout is infinite.
    		 * 			We stay in the loop waiting for the mutex to become available.
    		 * 			When it does we take it and return success.
    		 * 		The mutex is unlocked & timeout is infinite.
    		 * 			We break out of the loop, take the mutex and return success.
    		 *
    		 * 2. Finite wait
    		 * 		The mutex is locked & timeout is finite.
    		 * 			We break out of loop & return timeout.
    		 * 		The mutex is unlocked & timeout is finite.
    		 * 			We break out of loop & take the mutex and return success.
    		 */
    		do
    		{
            	Task_set_current_wait_on_object(&mutex_to_lock->mutex_task_list, timeout);

                RLTOS_EXIT_CRITICAL_SECTION();

                Rltos_task_yield();

    		    RLTOS_ENTER_CRITICAL_SECTION();
    		}
    		while( (RLTOS_TRUE == mutex_to_lock->state) && (RLTOS_UINT_MAX == timeout) );

    		if(RLTOS_FALSE == mutex_to_lock->state)
    		{
    	    	Set_mutex_owner(mutex_to_lock, Task_get_current()); /* Set the owner i.e. lock the mutex*/

    	    	/* If any other tasks are waiting for the mutex, set them to running here.*/
    	    	for(uint16_t i = 0U; i < mutex_to_lock->mutex_task_list.size; ++i)
    	    	{
    	    		Task_set_running(mutex_to_lock->mutex_task_list.p_head);
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

rltos_err_t Rltos_mutex_release(p_rltos_mutex_t mutex_to_release)
{
	rltos_err_t err = RLTOS_SUCCESS;

    RLTOS_PREPARE_CRITICAL_SECTION();

    RLTOS_ENTER_CRITICAL_SECTION();

    if(RLTOS_FALSE == mutex_to_release->state)
    {
    	err = RLTOS_RELEASED;
    }
    else
    {
    	/* If we are calling release and we do not own the mutex*/
    	if(mutex_to_release->p_current_owner != Task_get_current())
    	{
    		err = RLTOS_INVALID_CONTEXT;
    	}
    	else
    	{
        	Clear_mutex_owner(mutex_to_release);

        	/* If the mutex is now unlocked - and there is a task waiting for it.*/
        	if( (RLTOS_FALSE == mutex_to_release->state) && (NULL != mutex_to_release->mutex_task_list.p_head))
        	{
        		Task_set_running(mutex_to_release->mutex_task_list.p_head);
        	}
    	}
    }

    RLTOS_EXIT_CRITICAL_SECTION();

    return err;
}
/* END OF FUNCTION*/

static void Set_mutex_owner(p_rltos_mutex_t mutex_to_set, const p_rltos_task_t owner)
{
    mutex_to_set->state = RLTOS_TRUE;
    mutex_to_set->p_current_owner = owner;
    
    if(RLTOS_TRUE == mutex_to_set->is_recursive)
    {
        ++mutex_to_set->nesting_count;
    }
}
/* END OF FUNCTION*/

static void Clear_mutex_owner(p_rltos_mutex_t mutex_to_clear)
{
    if(RLTOS_TRUE == mutex_to_clear->is_recursive)
    {
        --mutex_to_clear->nesting_count;

        /* Only clear the owner details if we have recursively unlocked the mutex*/
        if(mutex_to_clear->nesting_count == 0U)
        {
            mutex_to_clear->state = RLTOS_FALSE;
            mutex_to_clear->p_current_owner = NULL;
        }
    }
    else
    {
        mutex_to_clear->state = RLTOS_FALSE;
        mutex_to_clear->p_current_owner = NULL;
    }
}
/* END OF FUNCTION*/

/*! @} */
