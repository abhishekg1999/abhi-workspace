/*
 * main.h
 *
 *  Created on: Jul 11, 2026
 *      Author: Abhishek Walia
 */

#ifndef MAIN_H_
#define MAIN_H_

#define MAX_TASK			5
/* stack memory calculation */
#define SIZE_TASK_STACK		1024U
#define SIZE_SCHED_STACK	1024U

#define SRAM_START		0x20000000U
#define SIZE_SRAM		((32)*(1024))
#define SRAM_END		((SRAM_START) + (SIZE_SRAM))

#define T1_STACK_START		SRAM_END
#define T2_STACK_START		((SRAM_END) - (1*SIZE_TASK_STACK))
#define T3_STACK_START		((SRAM_END) - (2*SIZE_TASK_STACK))
#define T4_STACK_START		((SRAM_END) - (3*SIZE_TASK_STACK))
#define IDLE_STACK_START    ((SRAM_END) - (4*SIZE_TASK_STACK))
#define SCHED_STACK_START	((SRAM_END) - (5*SIZE_TASK_STACK))

/* configure systick for 1ms */
#define TICK_HZ				1000U    // for 1ms timer
#define HSI_CLOCK			16000000U  //system clock
#define SYSTICK_TIM_CLK		HSI_CLOCK  // assign system clock to systick

#define TASK_READY_STATE	0x00
#define TASK_BLOCKED_STATE	0xFF

#define INTERRUPT_DISABLE()		do{__asm volatile("MOV R0,#0x1"); __asm volatile("MSR PRIMASK,R0");} while(0)
#define INTERRUPT_ENABLE()		do{__asm volatile("MOV R0,#0x0"); __asm volatile("MSR PRIMASK,R0");} while(0)

#endif /* MAIN_H_ */
