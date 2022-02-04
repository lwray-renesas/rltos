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

/** Notes for isr installation:
 * IAR:
 *      interrupts are installed in rltos_scheduler_asm.asm
 * 
 * CLANG/GCC:
 *      interrupts should be installed by the user application in the vector table
 *      Rltos_yield is the BRK isr (0x7E)
 *      Rltos_tick is the INTIL isr (0x38)
 *      extern these function in the location the interrupt vector table is constructed and ensure they are enterred in the correct place
 */

/** @brief data type the stack pointer points at*/
typedef uint16_t stack_type;

/** @brief data type the stack pointer points at*/
typedef uint16_t * stack_ptr_type;

/** @brief architectures unsigned integer type (explicit bit width)*/
typedef uint16_t rltos_uint;

/** @brief value of unsigned integers maximum value*/
#define RLTOS_UINT_MAX	(0xFFFFU)

/** @brief macro used to prepare for disabling interrupts*/
#define RLTOS_PREPARE_CRITICAL_SECTION()

/** @brief macro used to disable interrupts*/
#define RLTOS_ENTER_CRITICAL_SECTION()	asm("di")

/** @brief macro used to enable interrupts*/
#define RLTOS_EXIT_CRITICAL_SECTION()	asm("ei")

/** @brief macro used to yield a task - can also be fulfilled with a void function(void)*/
#define Rltos_task_yield()  asm("brk")

/** @brief Initialises task stack
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_func - function pointer to the task entry function.
 * @return The value of the stack pointer post initialisation (used to restore context).
 */
stack_ptr_type Rltos_port_stack_init(stack_ptr_type const p_stack_top, void* const p_task_func);

/** @brief Initialises & starts running the RLTOS tick timer (INTITL)*/
void Rltos_port_start_tick_timer(void);

/** @brief The hook called by the idle thread constantly in a while 1 loop - typically used to enter low power state.*/
void Rltos_port_idle_task_hook(void);

/** @brief Function to be provided for enterring first task of kernel - provided in ASM.
 * @details Does not save context, only restores it and returns as though from interrupt.
 */
extern void Rltos_port_enter_first_task(void);

/** @brief Function to be provided for manually yielding task - to be installed as ISR 0x7E (BRK) - provided in ASM.*/
extern void Rltos_port_yield(void);

/** @brief Function to be provided for tick handler ISR - to be installed as tick/timer interrupt (INTITL) - provided in ASM.*/
extern void Rltos_port_tick(void);

#endif /* RLTOS_PORT_H_ */
