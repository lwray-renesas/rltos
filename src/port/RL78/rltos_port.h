/*
 * rltos_port.h
 *
 *  Created on: 22 Apr 2021
 *      Author: Louis Wray
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

/** @brief macro used to disable interrupts*/
#define RLTOS_ENTER_CRITICAL_SECTION()	asm("di")

/** @brief macro used to enable interrupts*/
#define RLTOS_EXIT_CRITICAL_SECTION()	asm("ei")

/** @brief macro used to yield a task - typically implemented with inline asm or intrinsic*/
#define Rltos_task_yield()  asm("brk")

#endif /* RLTOS_PORT_H_ */
