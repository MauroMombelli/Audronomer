/*
 * l3g4200d.h
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#ifndef L3G4200D_H_
#define L3G4200D_H_

msg_t gyroscope_init(void);
msg_t gyroscope_read(int16_t *);

uint8_t gyroscope_interrupt_mode(void);
uint8_t gyroscope_interrutp_port(void);
uint8_t gyroscope_ext_pin(void);
extcallback_t gyroscope_interrutp_callback(void);
void gyroscope_interrutp(EXTDriver, expchannel_t);

#endif /* L3G4200D_H_ */
