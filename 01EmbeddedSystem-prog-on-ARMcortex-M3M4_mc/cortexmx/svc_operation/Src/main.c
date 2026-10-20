/**
WAP to add , subtract , multiply , divide 2 operands using SVC handler and return the result to thread mode code
and print the result. thread mode code should pass 2 operands via stack frame.

 */

#include <stdio.h>
#include <stdint.h>

void SVC_Handler_c(uint32_t *pBaseOFStackFrame){

	uint32_t arg0, arg1, res;
	printf("In SVC_Handler\n");
	uint8_t *pReturn_addr = (uint8_t*)pBaseOFStackFrame[6]; //store PC(return address) to get SVC number

	//decrement the return address by 2 to print to opcode of the SVC instruction in the program memory
	pReturn_addr -=2;
	uint8_t svc_number = *pReturn_addr;
	printf("SVC number is : %d\n", svc_number);

	arg0 = pBaseOFStackFrame[0]; //store r0 to arg0
	arg1 = pBaseOFStackFrame[1]; //store r1 to arg1

	switch(svc_number) {

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
		printf("Invalid svc code\n");
	}

	pBaseOFStackFrame[0] = res;

}

uint32_t add_numbers(uint32_t x, uint32_t y) {

	uint32_t res;
	__asm volatile("SVC #36");
	 __asm ("MOV %0, R0": "=r"(res) ::);
	 return res;
}

uint32_t sub_numbers(uint32_t x, uint32_t y) {

	uint32_t res;
	__asm volatile("SVC #37");
	 __asm ("MOV %0, R0": "=r"(res) ::);
	 return res;
}
uint32_t mul_numbers(uint32_t x, uint32_t y) {

	uint32_t res;
	__asm volatile("SVC #38");
	 __asm ("MOV %0, R0": "=r"(res) ::);
	 return res;
}
uint32_t div_numbers(uint32_t x, uint32_t y) {

	uint32_t res;
	__asm volatile("SVC #39");
	 __asm ("MOV %0, R0": "=r"(res) ::);
	 return res;
}

int main(void)
{
	uint32_t res;

	res = add_numbers(20, 10);
	printf("add res = %ld\n", res);

	res = sub_numbers(20, 10);
	printf("add res = %ld\n", res);

	res = mul_numbers(20, 10);
	printf("add res = %ld\n", res);

	res = div_numbers(20, 10);
	printf("add res = %ld\n", res);


	for(;;);
}

__attribute ((naked)) void SVC_Handler (void){
	__asm ("MRS R0, MSP");
	__asm ("B SVC_Handler_c");
}
