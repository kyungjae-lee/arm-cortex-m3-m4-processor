/*****************************************************************************************
 * @ File name		: main.c
 * @ Description	: Program to demonstrate how to do arithmetic operations using SVC
 * @ Author			: Kyungjae Lee
 * @ File created	: 05/03/2023
 ****************************************************************************************/

#include <stdint.h>
#include <stdio.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

/* Function Prototypes */
void SVC_Hander_C(uint32_t *pBaseStackFrame);
int32_t add(int32_t x, int32_t y);
int32_t sub(int32_t x, int32_t y);
int32_t mul(int32_t x, int32_t y);
int32_t div(int32_t x, int32_t y);

int main(void)
{
	printf("20 + 10 = %ld\n", add(20, 10));
	printf("20 - 10 = %ld\n", sub(20, 10));
	printf("20 * 10 = %ld\n", mul(20, 10));
	printf("20 / 10 = %ld\n", div(20, 10));

    /* Loop forever */
	for(;;);
}

__attribute__((naked)) void SVC_Handler(void)
{
	__asm("mrs r0, msp");
	__asm("b SVC_Handler_C");
}

void SVC_Handler_C(uint32_t *pBaseStackFrame)
{
	uint16_t arg0, arg1, res;

	uint8_t *pPC = (uint8_t *)pBaseStackFrame[6];
	pPC -= 2;

	// Extract the SVC number (LSByte of the opcode)
	uint8_t svc_number = *pPC & 0xFF;

	arg0 = pBaseStackFrame[0];	// By AAPCS
	arg1 = pBaseStackFrame[1];	// By AAPCS

	switch (svc_number)
	{
	case 36:
		res = arg0 + arg1;
		break;
	case 37:
		res = arg0 - arg1;
		break;
	case 38:
		res = arg0 * arg1;
		break;
	case 39:
		res = arg0 / arg1;
		break;
	default:
		printf("Invalid SVC code.\n");
	}

	pBaseStackFrame[0] = res;	// Store the return value in r0 (AAPCS)
}

int32_t add(int32_t x, int32_t y)
{
	int32_t res;
	__asm volatile("svc #36");
	__asm volatile("mov %0, r0": "=r"(res) ::);
	return res;
}

int32_t sub(int32_t x, int32_t y)
{
	int32_t res;
	__asm volatile("svc #37");
	__asm volatile("mov %0, r0": "=r"(res) ::);
	return res;
}

int32_t mul(int32_t x, int32_t y)
{
	int32_t res;
	__asm volatile("svc #38");
	__asm volatile("mov %0, r0": "=r"(res) ::);
	return res;
}

int32_t div(int32_t x, int32_t y)
{
	int32_t res;
	__asm volatile("svc #39");
	__asm volatile("mov %0, r0": "=r"(res) ::);
	return res;
}
