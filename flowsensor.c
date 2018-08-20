/* Flow Sensor with Raspberry Pi
 * Francis David (www.fdavid.com.br)
 *
 * Compile
 * gcc -Wall -o flowsensor flowsensor.c -lwiringPi
 */
#include <stdio.h>
#include <wiringPi.h>

/*
 * Command: gpio readall
 * Show all GPIO from Raspberry Pi in use
 * Use index wPi for wiringPi 
 */
#define GPIO 1  //for BCM, GPIO 18

#define FLOWRATE    4.5
#define LITERS(PULSE)   (PULSE / (60 * FLOWRATE))

/*
 * When a variable is change out main program, should be declared volatile for the 
 * compiler doesn't optimize.
 * https://en.wikipedia.org/wiki/Volatile_(computer_programming)
 */
volatile int counter=0;

void myInterrypt()
{
    counter++;
}

int main()
{
    int gotOne;
    int myCounter=0;
    
    if ( wiringPiSetup() == -1 ) {
        printf("Error setting WiringPi\n");
        return 1;
    }
    
    wiringPiISR( GPIO, INT_EDGE_FALLING, &myInterrypt );

    while (1) {
        gotOne=0;

        printf("\nWaiting signal...");
        fflush(stdout);
 
        while ( gotOne == 0) {
            if ( counter != myCounter ) {
                printf("\nPulses count: %d or %.2f liters", counter, LITERS(counter) );
                myCounter = counter;
                ++gotOne;
            }
        }
    }

    return 0;
}

