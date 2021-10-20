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

/** @brief data type the stack pointer points at*/
typedef uint32_t stack_type;

/** @brief the stack pointer data type*/
typedef uint32_t * stack_ptr_type;

/** @brief architectures unsigned integer type (explicit bit width)*/
typedef uint32_t rltos_uint;

/** @brief value of unsigned integers maximum value*/
#define RLTOS_UINT_MAX	(0xFFFFFFFFU)

/** @brief macro used to disable interrupts*/
#define RLTOS_ENTER_CRITICAL_SECTION() (void)

/** @brief macro used to enable interrupts*/
#define RLTOS_EXIT_CRITICAL_SECTION() (void)

/** @brief macro used to yield a task - typically implemented with inline asm or intrinsic*/
#define Rltos_task_yield() (void)

#endif /* RLTOS_PORT_H_ */
