/*
 * led.h
 *
 *  Created on: Jul 12, 2026
 *      Author: Abhishek Walia
 */

#ifndef LED_H_
#define LED_H_

// Define the single green LED pin for the Nucleo board
#define LED_GREEN   5

#define DELAY_COUNT_1MS 		 1250U
#define DELAY_COUNT_125MS 		(125U  * DELAY_COUNT_1MS)
#define DELAY_COUNT_250MS 		(250U  * DELAY_COUNT_1MS)
#define DELAY_COUNT_500MS  		(500U  * DELAY_COUNT_1MS)
#define DELAY_COUNT_1S  		(1000U * DELAY_COUNT_1MS)


// Function declarations
void led_init_all(void);
void led_on(uint8_t led_no);
void led_off(uint8_t led_no);
void delay(uint32_t count);
//void simple_delay(volatile uint32_t count);

#endif /* LED_H_ */
