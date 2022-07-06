;	rltos_kernel.asm
;	@brief assembly code for CLANG compiler of RLTOS kernel.
;	@details
;	Contains the following functions:
;		- void Rltos_enter_first_task(void);
;		- void Rltos_yield(void);
;		- void Rltos_tick(void);

; Global variables required by kernel - pointer to a pointer containg stack pointer
.EXTERN _p_current_task_ctl

; START OF MACRO - saves current CPU context
SAVE_CONTEXT .MACRO
	PUSH	AX		; Save Register Bank
	PUSH	BC
	PUSH	DE
	PUSH	HL
	MOV		A, ES	; Save ES & CS Registers
	XCH		A, X
	MOV		A, CS
	PUSH	AX
	MOVW	AX, !_p_current_task_ctl	; Save Stack pointer
	MOVW	HL, AX
	MOVW	AX, SP
	MOVW	[HL], AX
.ENDM
; END OF MACRO - saves current CPU context

; START OF MACRO - restores target CPU context
RESTORE_CONTEXT .MACRO
	MOVW	AX, !_p_current_task_ctl    ; Restore Stack pointer.
	MOVW	HL, AX
	MOVW	AX, [HL]
	MOVW	SP, AX
	POP		AX                    ; Restore CS & ES Registers.
	MOV		CS, A
	XCH		A, X
	MOV		ES, A
	POP		HL                    ; Restore Register Bank
	POP		DE
	POP		BC
	POP		AX
.ENDM
; END OF MACRO - restores target CPU context


; START OF FUNCTION - uint8_t Rltos_get_interrupt_status(void)
	.PUBLIC _Rltos_get_interrupt_status
	.SECTION	.text, TEXT
_Rltos_get_interrupt_status:
	MOV 	A, PSW
	SHR		A, 7
	ret
; END OF FUNCTION - uint8_t Rltos_get_interrupt_status(void)


; START OF FUNCTION - void Rltos_port_enter_first_task(void)
	.PUBLIC _Rltos_port_enter_first_task
	.SECTION	.text, TEXT
_Rltos_port_enter_first_task:
	RESTORE_CONTEXT
	reti
; END OF FUNCTION - void Rltos_enter_first_task(void)


; START OF FUNCTION - void Rltos_port_yield(void)
	_Rltos_port_yield  .vector 0x007E
	.SECTION    .text, TEXT
_Rltos_port_yield:
	SAVE_CONTEXT
	CALL	!!_Rltos_scheduler_switch_context ; Runs RLTOS scheduling algorithm
	RESTORE_CONTEXT
	retb
; END OF FUNCTION - void Rltos_yield(void)


; START OF FUNCTION - void Rltos_port_tick(void)
	_Rltos_port_tick  .vector 0x0038
	.SECTION    .text, TEXT
_Rltos_port_tick:
	SAVE_CONTEXT
	CALL	!!_Rltos_scheduler_tick_inc ; Runs RLTOS tick increment and task monitor
	CALL	!!_Rltos_scheduler_switch_context ; Runs RLTOS scheduling context switch
	RESTORE_CONTEXT
	CLR1 !0x036B.0 ; Clears interval timer interrupt bit
	reti
; END OF FUNCTION - void Rltos_tick(void)
