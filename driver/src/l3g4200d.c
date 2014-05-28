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
	char txbuf[2] = { 0x20, 0xcF };
	char rxbuf[2];

	spiStart(&SPID1, &spi1cfg);

	spiSelect(&SPID1);
	spiExchange(&SPID1, 2, txbuf, rxbuf);
	txbuf[0] = 0x23;
	txbuf[1] = 0xD0;
	spiExchange(&SPID1, 2, txbuf, rxbuf);
	spiUnselect(&SPID1);

	spiStop(&SPID1);

	return rxbuf[1];
}

msg_t read_gyroscope(int16_t *values) {
	/* read from L3GD20 registers and assemble data */
	/* 0xc0 sets read and address increment */
	char txbuf[8] = { 0xc0 | 0x27, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	char rxbuf[8];

	spiStart(&SPID1, &spi1cfg);

	spiSelect(&SPID1);
	spiExchange(&SPID1, 8, txbuf, rxbuf);
	spiUnselect(&SPID1);

	spiStop(&SPID1);

	if ( (rxbuf[1] & 0x8) || (rxbuf[1] & 0x7) ) {
		values[0] = ((int16_t)rxbuf[3] << 8) | rxbuf[2];
		values[1] = ((int16_t)rxbuf[5] << 8) | rxbuf[4];
		values[2] = ((int16_t)rxbuf[7] << 8) | rxbuf[6];
		return RDY_OK;
	}else{
		values[0] = values[1] = values[2] = 0;
		return RDY_RESET;
	}
}
