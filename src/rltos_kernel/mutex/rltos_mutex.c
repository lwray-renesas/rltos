/**
 * @addtogroup Rltos_mutex_prv Mutex Private
 * The private data (implementation) of the RLTOS mutex.
 * @ingroup Rltos_mutex_api
 * @{
 */

#include "rltos_task.h"
#include "task_scheduler/rltos_task_scheduler.h"
#include "rltos_mutex.h"

void Rltos_mutex_create(p_rltos_mutex_t mutex_to_create, const rltos_flag_t is_recursive, const p_rltos_task_t task_inital_owner)
{
    (void)mutex_to_create;
    (void)is_recursive;
    (void)task_inital_owner;
}
/* END OF FUNCTION*/

/*! @} */
