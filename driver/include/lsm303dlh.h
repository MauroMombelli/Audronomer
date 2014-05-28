/*
 * lsm303dlh.h
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#ifndef LSM303DLH_H_
#define LSM303DLH_H_

msg_t lsm303dlh_init(void);
msg_t read_acceleration(int16_t *);
msg_t read_magetometer(int16_t *);

#endif /* LSM303DLH_H_ */
