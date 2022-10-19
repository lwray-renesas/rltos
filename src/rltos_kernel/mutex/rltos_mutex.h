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
    const p_rltos_task_t p_current_owner; /**< Used to track the current owner of the mutex*/
}rltos_mutex_t;

/** @brief pointer to mutex structure*/
typedef rltos_mutex_t * p_rltos_mutex_t;

/** @brief Initialises mutex.
 * @param[inout] mutex_to_create - pointer to mutex structure from which to create the mutex.
 * @param[in] is_recurrsive - flag indicating whether the mutex is recursive or not.
 * @param[in] task_initial_owner - pointer to task which initially owns the mutex (NULL for no owner)*/
void Rltos_mutex_create(p_rltos_mutex_t mutex_to_create, const rltos_flag_t is_recursive, const p_rltos_task_t task_inital_owner);

#endif /* RLTOS_MUTEX_H_ */

/*! @} */
