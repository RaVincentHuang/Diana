.arch armv7ve
.arm
.section .data
.align
__N:
	.4byte	2010
.section .bss
.align
__x:
	.space 4
.align
__A:
	.space 16160400
.align
__B:
	.space 8040
.align
__C:
	.space 8040
.global main
.section .text
main:
entry__0:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}
	vpush	{s8, s9, s10}
	sub	sp,	sp, #4
	movw	r9, :lower16: __A
	movt	r9, :upper16: __A
	movw	r8, :lower16: __B
	movt	r8, :upper16: __B
	movw	r7, :lower16: __C
	movt	r7, :upper16: __C
	bl	getint
	mov r10,	r0
	mov	r0, #0
	cmp r0,	r10
	blt	innerBB__40
	b	guard__4
header__1:
	mov	r0, #0
	cmp r0,	r10
	blt	innerBB__42
	add	r0,	r11, #1
	cmp r0,	r10
	blt	innerBB__41
	b	guard__4
header__2:
	bl	getint
	mov r1,	r0
	movw	r0, #8040
	mul	r0,	r11,	r0
	add	r0,	r9,	r0
	str	r1, [r0, r6, lsl #2]
	add	r0,	r6, #1
	cmp r0,	r10
	blt	innerBB__43
	add	r0,	r11, #1
	cmp r0,	r10
	blt	innerBB__41
	b	guard__4
latch__3:
	add	r0,	r11, #1
	cmp r0,	r10
	blt	innerBB__41
	b	guard__4
guard__4:
	mov	r0, #0
	cmp r0,	r10
	blt	innerBB__38
	mov	r0, #57
	bl	_sysy_starttime
	mov	r0,	r10,	asr #31
	add	r0,	r10,	r0,	lsr #30
	mov	r6,	r0,	asr #2
	mov	r0, #0
	vmov s8,	r0
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__9
	vmov r0,	s9
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__12
	b	mv.copy.exit__19
header__5:
	bl	getint
	vmov r1,	s8
	str	r0, [r8, r1, lsl #2]
	vmov r1,	s8
	add	r0,	r1, #1
	cmp r0,	r10
	blt	innerBB__39
	mov	r0, #57
	bl	_sysy_starttime
	mov	r0,	r10,	asr #31
	add	r0,	r10,	r0,	lsr #30
	mov	r6,	r0,	asr #2
	mov	r0, #0
	vmov s8,	r0
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__9
	vmov r0,	s9
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__12
	b	mv.copy.exit__19
exit__6:
	mov	r0, #57
	bl	_sysy_starttime
	mov	r0,	r10,	asr #31
	add	r0,	r10,	r0,	lsr #30
	mov	r6,	r0,	asr #2
	mov	r0, #0
	vmov s8,	r0
	b	header__7
header__7:
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__9
	vmov r0,	s9
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__12
	b	mv.copy.exit__19
exit__8:
	mov	r0, #65
	bl	_sysy_stoptime
	mov r0,	r10
	mov r1,	r7
	bl	putarray
	mov	r0, #0
	add	sp,	sp, #4
	vpop	{s8, s9, s10}
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}
	bx	lr
mv.copy.preHeader__9:
	bl	__thread_create
	vmov s9,	r0
	vmov r0,	s9
	mul	r2,	r0,	r6
	vmov r0,	s9
	add	r0,	r0, #1
	mul	r1,	r0,	r6
	mov r0,	r2
	mov	r2, #0
	str	r2, [r7, r0, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r1
	blt	innerBB__31
	vmov r0,	s9
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__12
	b	mv.copy.exit__19
mv.copy.header__10:
	mov	r2, #0
	str	r2, [r7, r0, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r1
	blt	innerBB__31
	b	mv.copy.guard__11
mv.copy.guard__11:
	vmov r0,	s9
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__12
	b	mv.copy.exit__19
mv.copy.preHeader__12:
	bl	__thread_create
	vmov s10,	r0
	vmov r0,	s10
	mul	r1,	r0,	r6
	vmov r0,	s10
	add	r0,	r0, #1
	mul	r2,	r0,	r6
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__14
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__33
	b	mv.copy.exit__19
mv.copy.header__13:
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__14
	b	mv.copy.latch__18
mv.copy.preHeader__14:
	mov	r0, #0
	b	mv.copy.header__15
mv.copy.header__15:
	movw	r3, #8040
	mul	r3,	r1,	r3
	add	r3,	r9,	r3
	ldr	r12, [r3, r0, lsl #2]
	cmp	r12, #0
	beq	mv.copy.latch__17
	ldr	r11, [r7, r1, lsl #2]
	ldr	r3, [r8, r0, lsl #2]
	mul	r3,	r12,	r3
	add	r3,	r11,	r3
	str	r3, [r7, r1, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r10
	blt	innerBB__32
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__33
	b	mv.copy.exit__19
mv.copy.else__16:
	ldr	r11, [r7, r1, lsl #2]
	ldr	r3, [r8, r0, lsl #2]
	mul	r3,	r12,	r3
	add	r3,	r11,	r3
	str	r3, [r7, r1, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r10
	blt	innerBB__32
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__33
	b	mv.copy.exit__19
mv.copy.latch__17:
	add	r0,	r0, #1
	cmp r0,	r10
	blt	innerBB__32
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__33
	b	mv.copy.exit__19
mv.copy.latch__18:
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__33
	b	mv.copy.exit__19
mv.copy.exit__19:
	vmov r0,	s10
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__20
	mov r0,	r5
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__23
	b	mv.copy.exit__30
mv.copy.preHeader__20:
	bl	__thread_create
	mov r5,	r0
	mul	r2,	r5,	r6
	add	r0,	r5, #1
	mul	r1,	r0,	r6
	mov r0,	r2
	mov	r2, #0
	str	r2, [r8, r0, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r1
	blt	innerBB__34
	mov r0,	r5
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__23
	b	mv.copy.exit__30
mv.copy.header__21:
	mov	r2, #0
	str	r2, [r8, r0, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r1
	blt	innerBB__34
	b	mv.copy.guard__22
mv.copy.guard__22:
	mov r0,	r5
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__23
	b	mv.copy.exit__30
mv.copy.preHeader__23:
	bl	__thread_create
	mov r4,	r0
	mul	r1,	r4,	r6
	add	r0,	r4, #1
	mul	r2,	r0,	r6
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__25
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__36
	b	mv.copy.exit__30
mv.copy.header__24:
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__25
	b	mv.copy.latch__29
mv.copy.preHeader__25:
	mov	r0, #0
	b	mv.copy.header__26
mv.copy.header__26:
	movw	r3, #8040
	mul	r3,	r1,	r3
	add	r3,	r9,	r3
	ldr	r12, [r3, r0, lsl #2]
	cmp	r12, #0
	beq	mv.copy.latch__28
	ldr	r11, [r8, r1, lsl #2]
	ldr	r3, [r7, r0, lsl #2]
	mul	r3,	r12,	r3
	add	r3,	r11,	r3
	str	r3, [r8, r1, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r10
	blt	innerBB__35
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__36
	b	mv.copy.exit__30
mv.copy.else__27:
	ldr	r11, [r8, r1, lsl #2]
	ldr	r3, [r7, r0, lsl #2]
	mul	r3,	r12,	r3
	add	r3,	r11,	r3
	str	r3, [r8, r1, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r10
	blt	innerBB__35
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__36
	b	mv.copy.exit__30
mv.copy.latch__28:
	add	r0,	r0, #1
	cmp r0,	r10
	blt	innerBB__35
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__36
	b	mv.copy.exit__30
mv.copy.latch__29:
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__36
	b	mv.copy.exit__30
mv.copy.exit__30:
	mov r0,	r4
	bl	__thread_join
	vmov r0,	s8
	add	r0,	r0, #1
	cmp	r0, #50
	blt	innerBB__37
	mov	r0, #65
	bl	_sysy_stoptime
	mov r0,	r10
	mov r1,	r7
	bl	putarray
	mov	r0, #0
	add	sp,	sp, #4
	vpop	{s8, s9, s10}
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}
	bx	lr
innerBB__31:
	mov	r2, #0
	str	r2, [r7, r0, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r1
	blt	innerBB__31
	vmov r0,	s9
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__12
	b	mv.copy.exit__19
innerBB__32:
	b	mv.copy.header__15
innerBB__33:
	mov r1,	r0
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__14
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__33
	b	mv.copy.exit__19
innerBB__34:
	mov	r2, #0
	str	r2, [r8, r0, lsl #2]
	add	r0,	r0, #1
	cmp r0,	r1
	blt	innerBB__34
	mov r0,	r5
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__23
	b	mv.copy.exit__30
innerBB__35:
	b	mv.copy.header__26
innerBB__36:
	mov r1,	r0
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__25
	add	r0,	r1, #1
	cmp r0,	r2
	blt	innerBB__36
	b	mv.copy.exit__30
innerBB__37:
	vmov s8,	r0
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__9
	vmov r0,	s9
	bl	__thread_join
	mov	r0, #0
	cmp r0,	r10
	blt	mv.copy.preHeader__12
	b	mv.copy.exit__19
innerBB__38:
	mov	r1, #0
	vmov s8,	r1
	b	header__5
innerBB__39:
	mov r1,	r0
	vmov s8,	r1
	b	header__5
innerBB__40:
	mov	r11, #0
	mov	r0, #0
	cmp r0,	r10
	blt	innerBB__42
	add	r0,	r11, #1
	cmp r0,	r10
	blt	innerBB__41
	b	guard__4
innerBB__41:
	mov r11,	r0
	mov	r0, #0
	cmp r0,	r10
	blt	innerBB__42
	add	r0,	r11, #1
	cmp r0,	r10
	blt	innerBB__41
	b	guard__4
innerBB__42:
	mov	r6, #0
	b	header__2
innerBB__43:
	mov r6,	r0
	b	header__2
__thread_create:
 	sub sp, sp, #16777216
 	push {r0, r1, r2, r3}
 	mov r3, r7
 	mov r2, #4
 __thread_create_1:
 	sub r2, r2, #1
 	cmp r2, #0
 	beq __thread_create_2
 	mov r7, #120
 	mov r0, #273
 	mov r1, sp
 	swi #0
 	cmp r0, #0
 	bne __thread_create_1
 __thread_create_2:
 	mov r10, r2
 	mov r7, r3
 	pop {r0, r1, r2, r3}
 	add sp, sp, #16777216
 	bx lr
 __thread_join:
 	cmp r10, #0
 	beq __thread_join_2
 __thread_join_1:
 	mov r7, #1
 	swi #0
 __thread_join_2:
 	push {r0, r1, r2, r3}
 	mov r1, #4
 __thread_join_3:
 	sub r1, r1, #1
 	cmp r1, #0
 	beq __thread_join_4
 	push {r1, lr}
 	sub sp, sp, #4
 	mov r0, sp
 	bl wait
 	add sp, sp, #4
 	pop {r1, lr}
 	b __thread_join_3
 __thread_join_4:
 	pop {r0, r1, r2, r3}
 	mov r10, #0
 	bx lr
