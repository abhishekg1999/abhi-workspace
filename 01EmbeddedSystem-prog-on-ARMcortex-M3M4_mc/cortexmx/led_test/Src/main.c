#include <stdint.h>
#include <led.h>

int main(void)
{
    // 1. Initialize the GPIOA Pin 5 register settings
    led_init_all();

    // 2. Infinite test loop
    while(1)
    {
        led_on(LED_GREEN);
        delay(DELAY_COUNT_1MS); // Keep LED on for a moment

        led_off(LED_GREEN);
        delay(DELAY_COUNT_1MS); // Keep LED off for a moment
    }

}

