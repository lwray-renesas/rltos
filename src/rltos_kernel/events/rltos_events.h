/**
 * @defgroup Rltos_event_flags Event flags
 * The group defining the RLTOS event flags.
 * @addtogroup Rltos_event_flags_api Event Flags API
 * The public API (interface) to the RLTOS event flags module.
 * @ingroup Rltos_event_flags
 * @{
 */

#ifndef RLTOS_EVENT_FLAGS_H_
#define RLTOS_EVENT_FLAGS_H_

#ifndef RLTOS_TASK_H_
#error "Must include rltos_task.h before including this file in any source file"
#endif

/** Customisable event flag type*/
typedef uint32_t rltos_event_flag_t;

/** @brief Utility MACRO used to check if all events is set.
 * @param[in] flags - variable containing flags.
 * @param[in] events - the events for checking.
 * @return true if the exact events are detected, false otherwise.*/
#define EVENTS_EXACT_OCCURED(flags,events)	(((flags) & (events)) == events)

/** @brief Utility MACRO used to check if any event is set.
 * @param[in] flags - variable containing flags.
 * @param[in] events - the events for checking.
 * @return true if any events are detected, false otherwise.*/
#define EVENTS_ANY_OCCURED(flags,events)	(((flags) & (events)) != 0U)

/** @struct rltos_event_set_t
 * @brief Structure defining the an event set object.
 */
typedef struct
{
    rltos_task_list_t events_task_list; /**< Used to maintain the tasks waiting on the events contained in the event set*/
    rltos_event_flag_t event_flags; /**< Used to house the flags (each bit is a single event)*/
}rltos_events_t;

/** @brief pointer to event set structure*/
typedef rltos_events_t * p_rltos_events_t;

/** @brief Initialises event set.
 * @param[inout] es_to_create - pointer to event set [es] structure from which to create the event set.
 * @param[in] initial_flags - mask for flags to be set on creation.*/
void Rltos_events_create(p_rltos_events_t events_to_create, const rltos_event_flag_t initial_flags);

/** @brief Set events in a given group.
 * @param events_to_set - pointer to events structure to set events on.
 * @param flags - specific flags/events to set.
 * @return RLTOS_SUCCESS if successful.
 */
rltos_err_t Rltos_events_set(p_rltos_events_t events_to_set, const rltos_event_flag_t flags);

/** @brief Get events in a given group
 * @param[inout] events_to_get - pointer to events structure to get events from.
 * @param[in] flags - specific flags/events to wait for.
 * @param[out] - pointer to write the event flags that were set.
 * @param[in] clear_flags - true to clear flags on successful exit from function, false to not clear.
 * @param[in] all_flags - should all flags be waited for or any (AND vs OR).
 * @param[in] timeout - How long to wait for given flags.
 * @return RLTOS_SUCCESS if successful.
 */
rltos_err_t Rltos_events_get(p_rltos_events_t events_to_get, const rltos_event_flag_t flags, rltos_event_flag_t * output_flags, const rltos_flag_t clear_flags, const rltos_flag_t all_flags, const rltos_uint timeout);

#endif /* RLTOS_EVENT_FLAGS_H_ */

/*! @} */
