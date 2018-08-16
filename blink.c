/* Blink is the "Hello World" of the GPIO Raspberry Pi
 *
 * Compile
 * gcc -Wall -o blink blink.c -lwiringPi
 */
#include <wiringPi.h>

#define GPIO_LED 13
#define MS 500

int main()
{
    wiringPiSetup();
    pinMode(GPIO_LED, OUTPUT);

    while (1) {
        digitalWrite(GPIO_LED, HIGH);
        delay(MS);
        digitalWrite(GPIO_LED, LOW);
        delay(MS);
    }

    return 0;
}

