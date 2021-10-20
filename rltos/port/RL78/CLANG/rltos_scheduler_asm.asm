;	rltos_kernel.asm
;	@brief assembly code for CLANG compiler of RLTOS kernel.
;	@details
;	Contains the following functions:
;		- void Rltos_enter_first_task(void);
;		- void Rltos_yield(void);
;		- void Rltos_tick(void);

; Global variables required by kernel - pointer to a pointer containg stack pointer
.extern _p_current_task_ctl

; START OF MACRO - saves current CPU context
.macro	SAVE_CONTEXT
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
.endm
; END OF MACRO - saves current CPU context

; START OF MACRO - restores target CPU context
.macro	RESTORE_CONTEXT
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
.endm
; END OF MACRO - restores target CPU context


; START OF FUNCTION - void Rltos_enter_first_task(void)
	.global _Rltos_enter_first_task
	.type	_Rltos_enter_first_task,STT_FUNC
	.text
_Rltos_enter_first_task:
	RESTORE_CONTEXT
	reti
; END OF FUNCTION - void Rltos_enter_first_task(void)


; START OF FUNCTION - void Rltos_yield(void)
	.global _Rltos_yield
	.type	_Rltos_yield,STT_FUNC
	.extern _Rltos_scheduler_switch_context
	.text
_Rltos_yield:
	SAVE_CONTEXT
	CALL	!!_Rltos_scheduler_switch_context ; Runs RLTOS scheduling algorithm
	RESTORE_CONTEXT
	retb
; END OF FUNCTION - void Rltos_yield(void)


; START OF FUNCTION - void Rltos_tick(void)
	.global _Rltos_tick
	.type	_Rltos_tick,STT_FUNC
	.extern _Rltos_scheduler_tick_inc
	.extern _Rltos_scheduler_switch_context
	.text
_Rltos_tick:
	SAVE_CONTEXT
	CALL	!!_Rltos_scheduler_tick_inc ; Runs RLTOS tick increment and task monitor
	CALL	!!_Rltos_scheduler_switch_context ; Runs RLTOS scheduling context switch
	RESTORE_CONTEXT
	reti
; END OF FUNCTION - void Rltos_tick(void)
