//============================================================================
// Name        : osc2tlc5940.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

// spi.c
//
// Example program for bcm2835 library
// Shows how to interface with SPI to transfer a byte to and from an SPI device
//
// After installing bcm2835, you can build this
// with something like:
// gcc -o spi spi.c -l bcm2835
// sudo ./spi
//
// Or you can test it before installing with:
// gcc -o spi -I ../../src ../../src/bcm2835.c spi.c
// sudo ./spi
//
// Author: Mike McCauley
// Copyright (C) 2012 Mike McCauley
// $Id: RF22.h,v 1.21 2012/05/30 01:51:25 mikem Exp $

extern "C" {
#include <bcm2835.h>
}
#include <iostream>
using namespace std;
#define PIN RPI_GPIO_P1_11

void setup() {
	//bcm2835_set_debug(1);
	bcm2835_init();
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(PIN, LOW);
	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);  // The default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);               // The default
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64); // The default
	bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);              // The default
}

class State {
public:
	uint16_t out[48];
	uint8_t spi[72];
	State() {
		for (uint8_t i = 0; i < 48; i++)
			out[i] = 0;
		for (uint8_t j = 0; j < 72; j++)
			spi[j] = 0;
	}
	void update() {
		uint8_t j = 0;
		for (uint8_t i = 47; 1 < i; i -= 2, j += 3) {
			spi[j] = out[i] >> 4;
			spi[j + 1] = out[i] << 4 | out[i - 1] >> 8;
			spi[j + 2] = out[i - 1] & 0xFF;
		}
	}
	void transfer() {
		bcm2835_spi_begin();
		bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); // The default
		bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);           // The default
		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64); // The default
		bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);          // The default
		for (int i = 0; i < 72; i++)
			bcm2835_spi_transfer(spi[i]);
		bcm2835_spi_end();
		bcm2835_delayMicroseconds(5);
		bcm2835_gpio_write(PIN, HIGH);
		bcm2835_delayMicroseconds(1);
		bcm2835_gpio_write(PIN, LOW);
		bcm2835_delayMicroseconds(5);
	}
};

int main(int argc, char **argv) {
	setup();

	State aState;
	aState.out[47] = 4095;
	aState.out[46] = 2047;
	aState.out[45] = 1023;
	aState.out[44] = 511;
	aState.out[43] = 255;
	aState.out[42] = 127;
	aState.out[41] = 63;
	aState.out[40] = 31;
	aState.update();
	for (int i = 47; 39 < i; i--)
		cout << std::hex << (int) aState.out[i] << ' ';
	cout << endl;
	for (int i = 0; i < 16; i++)
		cout << std::hex << (int) aState.spi[i] << ' ';
	cout << endl;
	bcm2835_close();
	return 0;
}

