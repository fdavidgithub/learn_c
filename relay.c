/* Relay with Raspberry Pi
 * Francis David (www.fdavid.com.br)
 *
 * Compile
 * gcc -Wall -o relay relay.c -lwiringPi
 */
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

/*
 * Command: gpio readall
 * Show all GPIO from Raspberry Pi in use
 * Use index wPi for wiringPi 
 */
#define GPIO 0  //for BCM, GPIO 17
#define DELAY 1

int main()
{
    if ( wiringPiSetup() == -1 ) {
        printf("Error setting WiringPi\n");
        return 1;
    }

    pinMode(GPIO, OUTPUT);

    while (1) {
        printf("\nRelay ON");
	digitalWrite(GPIO, HIGH);
        sleep(DELAY);

	printf("\nRelay OFF");
	digitalWrite(GPIO, LOW);
	sleep(DELAY);
    }

    return 0;
}

