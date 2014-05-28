/*
 * l3g4200d.h
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#ifndef L3G4200D_H_
#define L3G4200D_H_

msg_t l3g4200d_init(void);
msg_t read_gyroscope(int16_t *);

#endif /* L3G4200D_H_ */
