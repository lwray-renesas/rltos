/*
 * rltos_port.h
 *
 *  Created on: 22 Apr 2021
 *      Author: Louis Wray
 * 
 *  This file defines the interface for RLTOS to work on any given CPU - each macro/function/type must be fulfilled in some way to provide the port for a given CPU/compiler.
 */

#ifndef RLTOS_PORT_H_
#define RLTOS_PORT_H_

/** The following definitions MUST be provided:
 * - uint8_t
 * - int8_t
 * - uint16_t
 * - int16_t
 * - uint32_t
 * - int32_t
 * - bool
 * - true
 * - false
 * - NULL
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** @brief data type the stack pointer points at*/
typedef uint32_t stack_type;

/** @brief the stack pointer data type*/
typedef uint32_t * stack_ptr_type;

/** @brief architectures unsigned integer type (explicit bit width)*/
typedef uint32_t rltos_uint;

/** @brief value of unsigned integers maximum value*/
#define RLTOS_UINT_MAX	(0xFFFFFFFFU)

/** @brief macro used to prepare for disabling interrupts*/
#define RLTOS_PREPARE_CRITICAL_SECTION()

/** @brief macro used to disable interrupts*/
#define RLTOS_ENTER_CRITICAL_SECTION()

/** @brief macro used to enable interrupts*/
#define RLTOS_EXIT_CRITICAL_SECTION()

/** @brief macro used to yield a task - can also be fulfilled with a void function(void)*/
#define Rltos_task_yield()

/** @brief Initialises task stack
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_func - function pointer to the task entry function.
 * @return The value of the stack pointer post initialisation (used to restore context).
 */
stack_ptr_type Rltos_port_stack_init(stack_ptr_type const p_stack_top, void* const p_task_func);

/** @brief Initialises & starts running the RLTOS tick timer (INTITL)*/
void Rltos_port_start_tick_timer(void);

/** @brief the hook called by the idle thread constantly in a while 1 loop*/
void Rltos_port_idle_task_hook(void);

/** @brief Function to be provided for returning interrupt status - provided in ASM.
 * @return 0 = interrupt disabled
 * 1 = interrupt enabled
*/
uint8_t Rltos_get_interrupt_status(void);

/** @brief Function to be provided for enterring first task of kernel - restores context and returns as though fom ISR.*/
void Rltos_port_enter_first_task(void);

/** @brief Function to be provided for yielding task*/
void Rltos_port_yield(void);

/** @brief Function to be provided for tick handler ISR*/
void Rltos_port_tick(void);

#endif /* RLTOS_PORT_H_ */
