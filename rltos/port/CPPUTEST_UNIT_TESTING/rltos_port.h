/*
 * rltos_port.h
 *
 *  Created on: 22 Apr 2021
 *      Author: Louis Wray
 */

#ifndef RLTOS_PORT_H_
#define RLTOS_PORT_H_

/*! Empty definition - no memory model used here*/
#define MEM_TYPE

/** @brief data type the stack pointer points at*/
typedef uint32_t stack_type;

/** @brief data type the stack pointer points at*/
typedef uint32_t MEM_TYPE * p_stack_type;

/** @brief architectures unsigned integer type (explicit bit width)*/
typedef uint32_t rltos_uint;

/** @brief value of unsigned integers maximum value*/
#define RLTOS_UINT_MAX	(0xFFFFFFFFU)

/** @brief macro used to disable interrupts*/
#define RLTOS_ENTER_CRITICAL_SECTION()

/** @brief macro used to enable interrupts*/
#define RLTOS_EXIT_CRITICAL_SECTION()


#endif /* RLTOS_PORT_H_ */
