/*
 * l3g4200d.c
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#include "ch.h"
#include "hal.h"

static const SPIConfig spi1cfg = {
  NULL,
  /* HW dependent part.*/
  GPIOE,
  GPIOE_SPI1_CS,
  SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA,
  0
};

msg_t l3g4200d_init(void) {
	uint8_t txbuf[2];

	spiAcquireBus(&SPID1);
	spiStart(&SPID1, &spi1cfg);
	spiSelect(&SPID1);

	txbuf[0] = 0x20;
	txbuf[1] = 0b11110111;
	spiSend(&SPID1, 2, txbuf);//write

	txbuf[0] = 0x22;
	txbuf[1] = 0b00001000; //enable int2_dataready
	spiSend(&SPID1, 2, txbuf);

	spiUnselect(&SPID1);

	spiSelect(&SPID1);
	uint8_t rxbuf[5];

	txbuf[0] = 0x0F|0xc0; //WHOAMI
	txbuf[1] = 0xff;
	spiExchange(&SPID1, 2, txbuf, rxbuf);//read

	uint8_t chrxbuf[5] = {0x20|0xc0, 0xff, 0xff, 0xff, 0xff};


	spiExchange(&SPID1, 5, chrxbuf, rxbuf);//read

	spiUnselect(&SPID1);
	spiStop(&SPID1);
	spiReleaseBus(&SPID1);

	return rxbuf[1];
}

msg_t read_gyroscope(int16_t *values) {
	/* read from L3GD20 registers and assemble data */
	/* 0xc0 sets read and address increment */
/*
	char txbuf2[24] = {  0xFF }; //all cells to 0xff
	uint8_t i;
	for (i=0; i< 24; i++){
		txbuf2[i] = 0xFF;
	}
	txbuf2[24] = 0xc0 | 0x20; //read register, autoincrement
	char rxbuf2[24];*/


	char txbuf[8] = { 0xc0 | 0x27, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	char rxbuf[8];

	spiAcquireBus(&SPID1);
	spiStart(&SPID1, &spi1cfg);
	spiSelect(&SPID1);

	//spiExchange(&SPID1, 24, txbuf2, rxbuf2);

	spiExchange(&SPID1, 8, txbuf, rxbuf);

	spiUnselect(&SPID1);
	spiStop(&SPID1);
	spiReleaseBus(&SPID1);

	if ( (rxbuf[1] & 0x8) || (rxbuf[1] & 0x7) ) {
		values[0] = ((uint16_t)rxbuf[3] << 8) | rxbuf[2];
		values[1] = ((uint16_t)rxbuf[5] << 8) | rxbuf[4];
		values[2] = ((uint16_t)rxbuf[7] << 8) | rxbuf[6];
		return RDY_OK;
	}else{
		values[0] = values[1] = values[2] = 0;
		return RDY_TIMEOUT;
	}
}
