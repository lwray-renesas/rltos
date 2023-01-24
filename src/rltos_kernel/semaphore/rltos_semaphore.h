/**
 * @defgroup Rltos_semaphore Semaphore
 * The group defining the RLTOS semaphore.
 * @addtogroup Rltos_semaphore_api Semaphore API
 * The public API (interface) to the RLTOS semaphore module.
 * @ingroup Rltos_semaphore
 * @{
 */

#ifndef RLTOS_SEMAPHORE_H_
#define RLTOS_SEMAPHORE_H_

#ifndef RLTOS_TASK_H_
#error "Must include rltos_task.h before including this file in any source file"
#endif

/** @struct rltos_semaphore_t
 * @brief Semaphore structure defining the semaphore object.
 */
typedef struct
{
    rltos_task_list_t semaphore_task_list; /**< Used to maintain the tasks waiting on the semaphore*/
    rltos_uint current_count; /**< semaphore count value*/
    rltos_uint max_count; /**< semaphore maximum count value*/
    p_rltos_task_t p_current_owner; /**< Used to track the current owner of the semaphore*/
}rltos_semaphore_t;

/** @brief pointer to semaphore structure*/
typedef rltos_semaphore_t * p_rltos_semaphore_t;

/** @brief Initialises semaphore.
 * @param[inout] semaphore_to_create - pointer to semaphore structure from which to create the semaphore.
 * @param[in] max_count - maximum count value for the semaphore to take.
 * @param[in] initial_count - Initial count value of the semaphore.
 * @param[in] initial_owner - pointer to initial task who owns the semaphore.
 */
void Rltos_semaphore_create(p_rltos_semaphore_t semaphore_to_create, const rltos_uint max_count, const rltos_uint initial_count, const p_rltos_task_t initial_owner);

/** @brief Try acquire the semaphore
 * @param[inout] semaphore_to_acquire - pointer to semaphore to work on.
 * @return RLTOS_SUCCESS if successful.
 * RLTOS_TIMEOUT if timeout expired
 * RLTOS_LOCKED if the semaphore is already acquired.
 * RLTOS_MAXIMUM if the semaphore has reached maximum value for the current user.*/
rltos_err_t Rltos_semaphore_acquire(p_rltos_semaphore_t semaphore_to_acquire, const rltos_uint timeout);

/** @brief Try release the semaphore
 * @param[inout] semaphore_to_release - pointer to semaphore to work on.
 * @return RLTOS_SUCCESS if successful
 * RLTOS_RELEASED if the semaphore was not released in the first place
 * RLTOS_INVALID_CONTEXT if the semaphore is being released from a thread which does not own it.*/
rltos_err_t Rltos_semaphore_release(p_rltos_semaphore_t semaphore_to_release);

#endif /* RLTOS_SEMAPHORE_H_ */

/*! @} */
