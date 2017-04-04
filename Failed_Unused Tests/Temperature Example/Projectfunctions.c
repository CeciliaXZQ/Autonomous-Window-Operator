#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "project.h"

int timeoutcount = 0;

void user_isr( void )
{

    return;
}

/* Setup function for control and status registers:
 * Control bit 0: enable start condition
 * Control bit 1: enable restart condition
 * Control bit 2: enable stop condition
 * Control bit 3: enable receive mode
 * Control bit 4: enable acknowledge bit
 * Status bit 14: enable master transmit */



void i2c1_idle(){
	while(I2C1CON & 0x1F || I2C1STAT & (1 << 14));
}

/* Send function (initialization)
 * the return value lets the master know that the slave devices are
 * on the bus and are ready to operate */

bool i2c1_send(uint8_t data) {
	i2c1_idle();
	I2C1TRN = data;									//
	i2c1_idle();
	return !(I2C1STAT & (1 << 15)); //ACKSTAT
}

uint8_t i2c1_recv() {
	i2c1_idle();
	I2C1CONSET = 1 << 3; //RCEN = 1
	i2c1_idle();
	I2C1STATCLR = 1 << 6; //I2COV = 0
	return I2C1RCV;
}

void i2c1_ack() {												// initiates not acknowledge condition for I2C
	i2c1_idle();
	I2C1CONCLR = 1 << 5; 									// set so that an acknowledge is sent
	I2C1CONSET = 1 << 4; 									// start acknowledge sequence
}

void i2c1_nack() {											// initiates not acknowledge condition for I2C
	i2c1_idle();
	I2C1CONSET = 1 << 5; 									// set so that a not-acknowledge is sent
	I2C1CONSET = 1 << 4; 									// start acknowledge sequence
}

void i2c1_start() {											// initates start condition for I2C
	i2c1_idle();
	I2C1CONSET = 1 << 0; 									// start condition enable bit (bit 0) set to 1
	i2c1_idle();
}

void i2c1_restart() {										// initiates restart condition for I2C
	i2c1_idle();
	I2C1CONSET = 1 << 1; 									// restart condition enable bit (bit 1) set to 1
	i2c1_idle();
}

void i2c1_stop() {											// initiates stop condition for I2C
	i2c1_idle();
	I2C1CONSET = 1 << 2; 									// stop condition enable bit (bit 2) set to 1
	i2c1_idle();
}


uint32_t strlen(char *str) {
	uint32_t n = 0;
	while(*str++)
		n++;
	return n;
}
