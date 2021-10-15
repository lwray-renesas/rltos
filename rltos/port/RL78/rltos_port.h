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

/** DETERMINE THE COMPILER IN USE*/
#if defined(__ICCRL78__)
#define IAR_RL78_COMPILER
#elif defined(__clang__) && defined(__RL78__)
#define LLVM_RL78_COMPILER
#else
#define UNKNOWN_COMPILER
#endif

/** DETERMINE THE MEMORY MODEL IN USE*/
#if defined(IAR_RL78_COMPILER)
#if __DATA_MODEL__ != NEAR
#define MEM_TYPE __far
#endif
#endif

/** If MEM_TYPE not detected manually set it (empty by default)*/
#ifndef MEM_TYPE
#define MEM_TYPE
#endif

/** @brief data type the stack pointer points at*/
typedef uint16_t stack_type;

/** @brief data type the stack pointer points at*/
typedef uint16_t MEM_TYPE * p_stack_type;

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
