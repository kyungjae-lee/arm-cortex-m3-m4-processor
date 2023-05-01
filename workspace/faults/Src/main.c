/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

/* System Handler Control and State Register (SHCRS); one of the System Control Block registers */
#define	SHCSR			(*(uint32_t volatile *)0xE000ED24)
#define USGFAULTENA		(1 << 18)
#define BUSFAULTENA		(1 << 17)
#define MEMFAULTENA		(1 << 16)
/* UsageFault Status Register (UFSR) - 16-bit register */
#define UFSR			(*(uint32_t volatile *)0xE000ED2A)
/* Configuration and Control Register */
#define CCR				(*(uint32_t volatile *)0xE000ED14)
#define DIV_0_TRP		(1 << 4)

/* Function Prototypes */
void UsageFault_Handler_C(uint32_t *pBaseStackFrame);
void Undefined_Instruction();
int Division(int x, int y);


int main(void)
{
	/* Enable all configurable exceptions (i.e., UsageFault, MemManage, BusFault) */
	SHCSR |= USGFAULTENA;	/* Enable UsageFault */
	SHCSR |= BUSFAULTENA;	/* Enable BusFault */
	SHCSR |= MEMFAULTENA;	/* Enable MemManage */

	/* Enable divide-by-zero trap */
	CCR |= DIV_0_TRP;

	/* UsageFault Test - Undefined Instruction (UNDEFINSTR) */
	//Undefined_Instruction();

	/* UsageFault Test - Divide by Zero (DIVBYZERO) */
	Division(10, 0);


	/* Analyze the faults */

    /* Loop forever */
	for(;;);
}

/* HardFault handler */
void HardFault_Handler(void)
{
	printf("Exception: HardFault\n");
	while (1);
}

/* MemManage handler */
void MemManage_Handler(void)
{
	printf("Exception: HardFault\n");
	while (1);
}

/* BusFault handler */
void BusFault_Handler(void)
{

	printf("Exception: BusFault\n");
	while (1);
}

/*
 * The function prologue is a few lines of code at the beginning of a function,
 * which prepare the stack and registers for use within the function.
 * Similarly, the function epilogue appears at the end of the function and restores the stack
 * and registers to the state they were in before the function was called.
 *
 * So, if you want to secure the stack pointer right after the automatic stacking is carried out,
 * it is important that you store the SP before the function prologue further manipulates the stack.
 * One way to work around this situation is to use __attribute__((naked)) function.
 */

__attribute__((naked)) void UsageFault_Handler(void)
{
	/*
	 * Here we capture the value of MSP which happens to be the base address of the stack frame
	 * which got pushed onto stack automatically by the processor during the exception entry
	 * from Thread Mode to Handler Mode.
	 */

	__asm("mrs r0, msp");				/* Secure MSP and store it in r0. (According to AAPCS,
										   the value stored in r0 will be passed to the callee
										   as the first argument. Therefore, secured MSP value
										   will be passed to the first parameter of the
										   UsageFault_Handler_C '*pBaseStackFrame'. */
	__asm("b UsageFault_Handler_C");	/* Branch to UsageFault_Handler_C */
}

/* UsageFault handler */
void UsageFault_Handler_C(uint32_t *pBaseStackFrame)
{
	printf("Exception: UsageFault\n");
	printf("UFSR = %lx\n", UFSR & 0xFFFF);	/* Read 16 bits only */
	printf("MSP = %p\n", pBaseStackFrame);	/* print  */

	/* Print stack frame (to troubleshoot any potential issues occurred when in Thread Mode) */
	printf("r0 	= %lx\n", pBaseStackFrame[0]);
	printf("r1 	= %lx\n", pBaseStackFrame[1]);
	printf("r2 	= %lx\n", pBaseStackFrame[2]);
	printf("r3 	= %lx\n", pBaseStackFrame[3]);
	printf("r12 = %lx\n", pBaseStackFrame[4]);
	printf("lr 	= %lx\n", pBaseStackFrame[5]);
	printf("pc 	= %lx\n", pBaseStackFrame[6]);	/* Stores the address which led to fault */
	printf("xpsr = %lx\n", pBaseStackFrame[7]);

	while (1);
}

void Undefined_Instruction()
{
	/* Force the processor to execute some undefined instruction */
	uint32_t volatile *pSRAM = (uint32_t volatile *)0x20010000;	/* Random address within SRAM */
	*pSRAM = 0xFFFFFFFF;	/* Undefined op-code */

	void (*some_addr)(void);		/* Defining a function pointer */
	some_addr = (void *)0x20010001; /* Setting 'some_addr' to 0x20010001 but not 0x20010000
									   has to do with Thumb ISA. Make sure to understand what's
									   happening behind the scenes.
									   Hint: Any branch instruction that writes to PC (Program Counter)
									   only updates  bits 31:1 and forces '0' into the 0th bit.
									   This is how the system works  well.
									   You can check this by setting the bit[0] to 0 and
									   monitor the value of the UFSR! */
	some_addr();					/* Force jump to 0xFFFFFFFF (undefined instruction) */
}

int Division(int x, int y)
{
	return x / y;
}