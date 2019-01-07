#include <stdio.h>
#include "serial.h"

/*
 * https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
 * https://www.cmrr.umn.edu/~strupp/serial.html
 */
int serial_open(const char *device, int baud)
{	
    struct termios options;
    int fd;

    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if ( fd < 0 ) {
        return -1;
    }

    /*
     * Get the current options for the port
     */
    if ( tcgetattr(fd, &options) != 0 ) {   
        return -1;
    }

    /*
     * Set the baud rates
     */
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

    /*
     * No parity (8N1)
     */
//    options.c_cflag &= ~PARENB;
//    options.c_cflag &= ~CSTOPB;
//    options.c_cflag &= ~CSIZE;  /* Mask the character size bits */
//    options.c_cflag |= CS8;     /* Select 8 data bits */

    /*
     * No parity (7S1)
     */
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;  /* Mask the character size bits */
    options.c_cflag |= CS8;     /* Select 8 data bits */

   
    /*
     * Set options for the port
     */
    if ( tcsetattr (fd, TCSANOW, &options) != 0 ) {
        printf("Error while atrib options");
        return -1;
    }

    sleep(1);
	return fd;
}

uint8_t serial_recv(int fd)
{
	uint8_t byte;

    usleep(1000);
    read(fd, &byte, sizeof byte);

	return byte;
}

int serial_send(int fd, uint8_t byte)
{
    int bytes;

	bytes = write(fd, &byte, sizeof byte);
    //usleep(1000);   //Waiting write byte

    return bytes;
}


