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
 * IAR & CCRL:
 *      interrupts are installed in rltos_scheduler_asm.asm
 *
 * CLANG:
 *      interrupts should be installed by the user application in the vector table
 *      Rltos_port_yield is the BRK isr (0x7E)
 *      Rltos_port_tick is the INTIL isr (0x38)
 *      extern these function in the location the interrupt vector table is constructed and ensure they are enterred in the correct place
 */

/** @brief data type the stack pointer points at*/
typedef uint16_t stack_type;

/** @brief data type the stack pointer points at*/
typedef uint16_t * stack_ptr_type;

/** @brief architectures unsigned integer type (explicit bit width)*/
typedef uint16_t rltos_uint;

/** @brief binary flag, usage is such that the only allowable values are 1 or 0*/
typedef enum
{
    RLTOS_FALSE = 0U,
    RLTOS_TRUE
}rltos_flag_t;

/** @brief pointer type for task functions.
 * @note In the event the target application in an RL78 grows beyond 64k boundary 
*/
typedef void(*p_task_func_t)(void);

/** @brief value of unsigned integers maximum value*/
#define RLTOS_UINT_MAX	(0xFFFFU)

/** @brief macro used to prepare for disabling interrupts*/
#define RLTOS_PREPARE_CRITICAL_SECTION() rltos_flag_t l_int_status = Rltos_interrupts_enabled()

#if defined(__ICCRL78__) || (defined(__clang__) && defined(__RL78__))
#define RLTOS_DI()	asm("di")
#define RLTOS_EI()	asm("ei")
#define RLTOS_BRK()	asm("brk")
#elif defined(__CCRL__)
#define RLTOS_DI()	__DI()
#define RLTOS_EI()	__EI()
#define RLTOS_BRK()	__brk()
#else
#error "Unsupported compiler!"
#endif

/** @brief macro used to disable interrupts*/
#define RLTOS_ENTER_CRITICAL_SECTION()	RLTOS_DI()

/** @brief macro used to enable interrupts*/
#define RLTOS_EXIT_CRITICAL_SECTION()	if(l_int_status == 1U) { RLTOS_EI(); }

/** @brief macro used to yield a task - can also be fulfilled with a void function(void)*/
#define Rltos_task_yield()  RLTOS_BRK()

/** @brief macros used to set the idle task stack size (in stack_types chunks)*/
#define RLTOS_IDLE_TASK_STACK_SIZE	(64U)

/** @brief Initialises & starts running the RLTOS tick timer (INTITL)*/
extern void (*Rltos_port_start_tick_timer)(void);

/** @brief Stops the RLTOS tick timer (INTITL)*/
extern void (*Rltos_port_stop_tick_timer)(void);

/** @brief Initialises task stack
 * @param[in] p_stack_top - pointer to the top of the stack.
 * @param[in] p_task_func - function pointer to the task entry function.
 * @return The value of the stack pointer post initialisation (used to restore context).
 */
stack_ptr_type Rltos_port_stack_init(stack_ptr_type const p_stack_top, p_task_func_t const p_task_func);

/** @brief The hook called by the idle thread constantly in a while 1 loop - typically used to enter low power state.*/
void Rltos_port_idle_task_hook(void);

/** @brief Function to be provided for returning interrupt status - provided in ASM.
 * @return RLTOS_FALSE = interrupt disabled
 * RLTOS_TRUE = interrupt enabled
*/
extern rltos_flag_t Rltos_interrupts_enabled(void);

/** @brief Function to be provided for enterring first task of kernel - provided in ASM.
 * @details Does not save context, only restores it and returns as though from interrupt.
 */
extern void Rltos_port_enter_first_task(void);

/** @brief Function to be provided for manually yielding task - to be installed as ISR 0x7E (BRK) - provided in ASM.
 * @details For the IAR comipiler the ISR is installed in ASM.
 * For the LLVM port this header file should be included in the vector table definition file and the function should be installed in the given vector table location.
*/
extern void Rltos_port_yield(void);

/** @brief Function to be provided for tick handler ISR - to be installed as tick/timer interrupt (INTITL) - provided in ASM.
 * @details For the IAR comipiler the ISR is installed in ASM.
 * For the LLVM port this header file should be included in the vector table definition file and the function should be installed in the given vector table location.
*/
extern void Rltos_port_tick(void);

#endif /* RLTOS_PORT_H_ */
