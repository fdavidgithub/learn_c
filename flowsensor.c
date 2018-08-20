/* Flow Sensor with Raspberry Pi
 *
 * Compile
 * gcc -Wall -o flowsensor flowsensor.c -lwiringPi
 */
#include <wiringPi.h>

#define GPIO 18
#define MS 500

int main()
{
    wiringPiSetup();
    pinMode(GPIO, INPUT);

    return 0;
}

