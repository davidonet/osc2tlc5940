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
#include "lo/lo.h"
#include <stdio.h>

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
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32); // The default
	bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);              // The default
}

class State {
public:
	static State theState;
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
		spi[j] = out[1] >> 4;
		spi[j + 1] = out[1] << 4 | out[0] >> 8;
		spi[j + 2] = out[0] & 0xFF;
		transfer();
	}
	void clear() {
		for (uint8_t i = 0; i < 48; i++)
			out[i] = 0;
		update();
	}
	void full() {
		for (uint8_t i = 0; i < 48; i++)
			out[i] = 4095;
		update();
	}

	void transfer() {
		bcm2835_spi_transfern((char*) spi, 72);
		bcm2835_delayMicroseconds(5);
		bcm2835_gpio_write(PIN, HIGH);
		bcm2835_delayMicroseconds(1);
		bcm2835_gpio_write(PIN, LOW);
		bcm2835_delayMicroseconds(5);
	}
};

State State::theState;

int done = 0;
void error(int num, const char *m, const char *path) {
	cout << "liblo server error" << num << "in path" << path << " : " << m
			<< endl;
}

int generic_handler(const char *path, const char *types, lo_arg **argv,
		int argc, void *data, void *user_data) {
	int i;

	printf("path: <%s>\n", path);
	for (i = 0; i < argc; i++) {
		printf("arg %d '%c' ", i, types[i]);
		lo_arg_pp((lo_type) types[i], (void*) argv[i]);
		printf("\n");
	}
	printf("\n");
	fflush(stdout);

	return 1;
}

int fader1(const char *path, const char *types, lo_arg **argv, int argc,
		void *data, void *user_data) {
	State::theState.out[0] = uint16_t(argv[0]->f * 4095);
	State::theState.update();
	return 0;
}
int fader2(const char *path, const char *types, lo_arg **argv, int argc,
		void *data, void *user_data) {
	State::theState.out[1] = uint16_t(argv[0]->f * 4095);
	State::theState.update();
	return 0;
}
int fader3(const char *path, const char *types, lo_arg **argv, int argc,
		void *data, void *user_data) {
	State::theState.out[2] = uint16_t(argv[0]->f * 4095);
	State::theState.update();
	return 0;
}
int fader4(const char *path, const char *types, lo_arg **argv, int argc,
		void *data, void *user_data) {
	State::theState.out[3] = uint16_t(argv[0]->f * 4095);
	State::theState.update();
	return 0;
}
int fader5(const char *path, const char *types, lo_arg **argv, int argc,
		void *data, void *user_data) {
	for (uint8_t i = 0; i < 48; i++)
		State::theState.out[i] = uint16_t(argv[0]->f * 4095);
	State::theState.update();
	return 0;
}

int toggle1(const char *path, const char *types, lo_arg **argv, int argc,
		void *data, void *user_data) {
	if (argv[0]->f < 0.5f)
		State::theState.clear();
	else
		State::theState.full();
	return 0;
}
int toggle2(const char *path, const char *types, lo_arg **argv, int argc,
		void *data, void *user_data) {
	for (uint8_t i = 0; i < 48; i++)
		State::theState.out[i] = 128;
	State::theState.update();
	return 0;
}

int main(int argc, char **argv) {
	lo_server_thread st = lo_server_thread_new("7770", error);
	lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);
	lo_server_thread_add_method(st, "/1/fader1", "f", fader1, NULL);
	lo_server_thread_add_method(st, "/1/fader2", "f", fader2, NULL);
	lo_server_thread_add_method(st, "/1/fader3", "f", fader3, NULL);
	lo_server_thread_add_method(st, "/1/fader4", "f", fader4, NULL);
	lo_server_thread_add_method(st, "/1/fader5", "f", fader5, NULL);
	lo_server_thread_add_method(st, "/1/toggle1", "f", toggle1, NULL);
	lo_server_thread_add_method(st, "/1/toggle2", "f", toggle2, NULL);

	lo_server_thread_start(st);
	setup();

	while (1) {
		/*
		 for (int c = 0; c < 5; c++) {
		 for (uint8_t i = 0; i < 48; i++) {
		 aState.out[i] = 4095;
		 aState.update();
		 aState.transfer();
		 bcm2835_delay(5);
		 }
		 for (int i = 48; 0 <= i; i--) {
		 aState.out[i] = 0;
		 aState.update();
		 aState.transfer();
		 bcm2835_delay(5);
		 }
		 for (int i = 48; 0 <= i; i--) {
		 aState.out[i] = 4095;
		 aState.update();
		 aState.transfer();
		 bcm2835_delay(5);
		 }
		 for (uint8_t i = 0; i < 48; i++) {
		 aState.out[i] = 0;
		 aState.update();
		 aState.transfer();
		 bcm2835_delay(5);
		 }

		 }
		 for (int c = 0; c < 5; c++) {
		 for (uint8_t i = 0; i < 48; i++) {
		 aState.clear();
		 aState.out[i] = 4095;
		 aState.update();
		 aState.transfer();
		 bcm2835_delay(20);
		 }
		 for (int i = 48; 0 <= i; i--) {
		 aState.clear();
		 aState.out[i] = 4095;
		 aState.update();
		 aState.transfer();
		 bcm2835_delay(20);
		 }
		 }*/
		bcm2835_delay(1);
	}

	bcm2835_spi_end();
	bcm2835_close();
	lo_server_thread_free(st);
	return 0;
}
