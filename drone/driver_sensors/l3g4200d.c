/*
 * l3g4200d.c
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#include "l3g4200d.h"

static const SPIConfig spi1cfg = { NULL,
/* HW dependent part.*/
GPIOE, GPIOE_SPI1_CS, SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA,
		0 };

static const float dps = 8.75f;
//const float dps = 17.5f;
static const float degree_to_radiant = 0.0174532925f;

#define GIROSCOPE_USE_INTERRUPT TRUE

msg_t gyroscope_init(void) {
	uint8_t txbuf[2];
	uint8_t rxbuf[5];

	spiAcquireBus(&SPID1);
	spiStart(&SPID1, &spi1cfg);

	spiSelect(&SPID1);
	txbuf[0] = 0x20;
	//txbuf[1] = 0b11001111; //ODR 800Hz, cut off 30, normal mode, 3 axis reading
	//txbuf[1] = 0b11111111; //ODR 800Hz, cut off 110, normal mode, 3 axis reading
	txbuf[1] = 0b10111111; //ODR 400Hz, cut off 110, normal mode, 3 axis reading
	spiExchange(&SPID1, 2, txbuf, rxbuf); //write
	spiUnselect(&SPID1);

	spiSelect(&SPID1);
	txbuf[0] = 0x22;
	txbuf[1] = 0b00001000; //enable int2_dataready I2_DRDY
	spiExchange(&SPID1, 2, txbuf, rxbuf); //write
	spiUnselect(&SPID1);

	spiSelect(&SPID1);
	txbuf[0] = 0x23;
	txbuf[1] = 0b11010000; //block data update, MSB, 500°/s
	spiExchange(&SPID1, 2, txbuf, rxbuf); //write
	spiUnselect(&SPID1);

	spiStop(&SPID1);
	spiReleaseBus(&SPID1);

	return rxbuf[1];
}

msg_t gyroscope_read(struct Vector3f *ris) {
	/* read from L3GD20 registers and assemble data */
	/* 0xc0 sets read and address increment */

	char txbuf[8] = { 0xc0 | 0x27, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	char rxbuf[8];

	spiAcquireBus(&SPID1);
	spiStart(&SPID1, &spi1cfg);
	spiSelect(&SPID1);
	spiExchange(&SPID1, 8, txbuf, rxbuf);
	spiUnselect(&SPID1);
	spiStop(&SPID1);
	spiReleaseBus(&SPID1);

	if ((rxbuf[1] & 0x8) || (rxbuf[1] & 0x7)) {
		ris->x = ((uint16_t) rxbuf[2] << 8) | rxbuf[3];
		ris->y = ((uint16_t) rxbuf[4] << 8) | rxbuf[5];
		ris->z = ((uint16_t) rxbuf[6] << 8) | rxbuf[7];

		ris->x = ((ris->x * dps) * (1 / 1000.0)) * degree_to_radiant;
		ris->y = ((ris->y * dps) * (1 / 1000.0)) * degree_to_radiant;
		ris->z = ((ris->z * dps) * (1 / 1000.0)) * degree_to_radiant;

		return RDY_OK;
	} else {
		//values[0] = values[1] = values[2] = 0;
		return RDY_RESET;
	}
}

uint8_t gyroscope_interrupt_mode(void) {
	return EXT_CH_MODE_RISING_EDGE;
}
uint8_t gyroscope_interrutp_port(void) {
	return EXT_MODE_GPIOE;
}
uint8_t gyroscope_ext_pin(void) {
	return 1;
}
