/**
 * @addtogroup Rltos_task_scheduler_prv Task Scheduler Private
 * @ingroup Rltos_task_scheduler_api
 * @{
 */

#ifndef RLTOS_TASK_SCHEDULER_PRV_H_
#define RLTOS_TASK_SCHEDULER_PRV_H_

#ifndef RLTOS_TASK_H_
#error "Must include rltos_task.h before including this file in any source file"
#endif

#include "rltos_task_scheduler.h"

/** @brief Function used to check if a task is contained within the task list or not.
 * @param[in] lst - pointer to a task list to check for tasks existence.
 * @param[in] tsk - task to check whether contained in (owned by) list.
 * @param[in] list_index - index of the list to check.
 */
static inline bool Task_is_in_list(p_rltos_task_list_t const lst, p_rltos_task_t const tsk, const list_index_t ind)
{
	return (lst != NULL) && (tsk->p_owners[ind] == lst);
}
/* END OF FUNCTION*/

/*! @} */

/* Variables defined intask list we want to access during unit testing of modules integrating with kernel*/
extern rltos_task_list_t running_task_list;
extern rltos_task_list_t idle_task_list;
extern rltos_task_list_t stopped_task_list;
extern p_rltos_task_t p_current_task_ctl;
extern volatile rltos_uint rltos_system_tick;
extern volatile rltos_uint rltos_wrap_count;
extern bool should_switch_task;

/** @brief implementation of rltos tick increment - called from rltos_scheduler_asm.asm */
void Rltos_scheduler_tick_inc(void);

/** @brief implementation of rltos context switch - called from rltos_scheduler_asm.asm */
void Rltos_scheduler_switch_context(void);

#endif /* RLTOS_TASK_SCHEDULER_PRV_H_ */
