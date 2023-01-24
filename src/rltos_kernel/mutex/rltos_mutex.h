/**
 * @defgroup Rltos_mutex Mutex
 * The group defining the RLTOS mutex.
 * @addtogroup Rltos_mutex_api Mutex API
 * The public API (interface) to the RLTOS mutex module.
 * @ingroup Rltos_mutex
 * @{
 */

#ifndef RLTOS_MUTEX_H_
#define RLTOS_MUTEX_H_

#ifndef RLTOS_TASK_H_
#error "Must include rltos_task.h before including this file in any source file"
#endif

/** @struct rltos_mutex_t
 * @brief Mutex structure defining the mutex object.
 */
typedef struct  
{
    rltos_task_list_t mutex_task_list; /**< Used to maintain the tasks waiting on the mutex*/
    rltos_flag_t is_recursive; /**< Indicating whether or not the mutex is recursive*/
    rltos_flag_t state; /**< Used to indicate the state of the lock*/
    p_rltos_task_t p_current_owner; /**< Used to track the current owner of the mutex*/
    rltos_uint nesting_count; /**< nesting count for recursive operation*/
}rltos_mutex_t;

/** @brief pointer to mutex structure*/
typedef rltos_mutex_t * p_rltos_mutex_t;

/** @brief Initialises mutex.
 * @details Must be called from a thread if the desire is to have an initial owner.
 * @param[inout] mutex_to_create - pointer to mutex structure from which to create the mutex.
 * @param[in] is_recurrsive - flag indicating whether the mutex is recursive or not.
 * @param[in] initial_owner - pointer to initial task who owns the mutex.
 * */
void Rltos_mutex_create(p_rltos_mutex_t mutex_to_create, const rltos_flag_t is_recursive, const p_rltos_task_t initial_owner);

/** @brief Try lock the mutex
 * @param[inout] mutex_to_lock - pointer to mutex to work on.
 * @return RLTOS_SUCCESS if successful.
 * RLTOS_TIMEOUT if timeout expired
 * RLTOS_LOCKED if the mutex is already locked but its not recursive*/
rltos_err_t Rltos_mutex_lock(p_rltos_mutex_t mutex_to_lock, rltos_uint timeout);

/** @brief Try release the mutex
 * @param[inout] mutex_to_release - pointer to mutex to work on.
 * @return RLTOS_SUCCESS if successful
 * RLTOS_RELEASED if the mutex was not locked in the first place
 * RLTOS_INVALID_CONTEXT if the mutex is being released from a thread which does not own it.*/
rltos_err_t Rltos_mutex_release(p_rltos_mutex_t mutex_to_release);

#endif /* RLTOS_MUTEX_H_ */

/*! @} */
