#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <stdbool.h>
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "project.h"

#define DISPLAY_VDD_PORT PORTF
#define DISPLAY_VDD_MASK 0x40
#define DISPLAY_VBATT_PORT PORTF
#define DISPLAY_VBATT_MASK 0x20
#define DISPLAY_COMMAND_DATA_PORT PORTF
#define DISPLAY_COMMAND_DATA_MASK 0x10
#define DISPLAY_RESET_PORT PORTG
#define DISPLAY_RESET_MASK 0x200




void delay1(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));							//#define SPI2STAT	*(volatile unsigned*)(0xBF805A10) /* Status */
	SPI2BUF = data;													//#define SPI2BUF		*(volatile unsigned*)(0xBF805A20) /* receive from SPI2 */
	while(!(SPI2STAT & 0x01));
	return SPI2BUF;
}

void display_init() {
	DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
	delay1(10);
	DISPLAY_VDD_PORT &= ~DISPLAY_VDD_MASK;
	delay1(1000000);

	spi_send_recv(0xAE);
	DISPLAY_RESET_PORT &= ~DISPLAY_RESET_MASK;
	delay1(10);
	DISPLAY_RESET_PORT |= DISPLAY_RESET_MASK;
	delay1(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_VBATT_PORT &= ~DISPLAY_VBATT_MASK;
	delay1(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;

	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_update() {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_COMMAND_DATA_PORT |= DISPLAY_COMMAND_DATA_MASK;

		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;

			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
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

char *fixed_to_string(uint16_t num, char *buf) {
	bool neg = false;
	uint32_t n;
	char *tmp;

	if(num & 0x8000) {
		num = ~num + 1;
		neg = true;
	}

	buf += 4;
	n = num >> 8;
	tmp = buf;
	do {
		*--tmp = (n  % 10) + '0';
		n /= 10;
	} while(n);
	if(neg)
		*--tmp = '-';

	n = num;
	if(!(n & 0xFF)) {
		*buf = 0;
		return tmp;
	}
	*buf++ = '.';
	while((n &= 0xFF)) {
		n *= 10;
		*buf++ = (n >> 8) + '0';
	}
	*buf = 0;

	return tmp;
}
