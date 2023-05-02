/*****************************************************************************************
 * @ File name		: main.c
 * @ Description	: Program to demonstrate how to extract SVC number
 * @ Author			: Kyungjae Lee
 * @ File created	: 05/01/2023
 ****************************************************************************************/

/*
 * Instruction
 *
 * 1. Write a main() function where you should execute the SVC instruction with an argument.
 *    e.g., svc #0x05
 * 2. Implement the SVC handler
 * 3. In the SVC handler extract the SVC number and print it using printf().
 * 4. Increment the SVC number by 4 and return it to the thread mode.
 */

#include <stdint.h>
#include <stdio.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

/* Function Prototypes */
void SVC_Handler_C(uint32_t *pBaseStackFrame);


int main(void)
{
	/*
	 * The SVC instruction causes SVC exception.
	 *
	 * 'imm' is ignored by the processor. If required, it can be retrieved by the exception
	 * handler to determine what service is being requested.
	 */

	__asm("svc #8");

#if 0	/* This method is not recommended. Use inline gcc assembly code instead! */
	uint32_t register data __asm("r0");	/* Tells the compiler to associate 'data' with r0
										   instead of creating 'data' on stack. (It is up to
										   the compiler whether or not it will actually
										   store 'data' in the register.) */
#endif

	uint32_t data;
	__asm volatile("mov %0, r0": "=r"(data) ::);	/* Store r0 value to 'data' */

	printf("data = %ld\n", data);

    /* Loop forever */
	for(;;);
}

__attribute__((naked)) void SVC_Handler(void)
{
	/* Capture the MSP value */
	__asm("mrs r0, msp");
	__asm("b SVC_Handler_C");	/* r0 value (msp) will be passed as the 1st arg */
}

void SVC_Handler_C(uint32_t *pBaseStackFrame)
{
	printf("SVC Hander\n");
	uint8_t *pPC = (uint8_t *)pBaseStackFrame[6];	/* Declaring the pointer to PC as
													   (uint8_t *) to make it easier to
													   do the following decrement operation */
	pPC -= 2;	/* Since Thumb ISA enforces 16-bit alignment on all instructions, this
				   operation will yield pPC pointing to the instruction located right before
				   where the PC is pointing to. In other words, pPC will now point to the
				   SVC instruction that invoked the SVC_Handler. */

	/* Extract SVC number from SVC op-code (the LSB in little-endian) */
	uint8_t svc_number = *pPC & 0xFF;
	printf("SVC number: %d\n", svc_number);

	/* Increment the SVC number by 4 and return it to the Thread Mode */
	svc_number += 4;

	pBaseStackFrame[0] = svc_number;

}
