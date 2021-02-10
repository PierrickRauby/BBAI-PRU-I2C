/*
 * am_572x_pru_i2c_driver.h
 * Author Pierrick Rauby : pierrick.rauby@gmail.com
 * Created on Feb10 2021
 * For Texas Instrument am572x:
 *    - based on the TRM SPRUHZ6L 24.1.5 HS I2C Programming Guide p5740
 *    - if not mentionned otherwise the page number in the comments refere to 
 *        the TRM SPRUHZ6L
 * Function:
 *    - Header for am_572x_pru_i2c_driver.pru1_1.c
 *    - Supposed to do what a header is suppose to do...
 *
 */

#ifndef AM572x_PRU_I2C_DRIVER_H_
#define AM572x_PRU_I2C_DRIVER_H_


#include <stdint.h>

uint8_t pru_i2c_driver_init(void);

#endif /* AM572x_PRU_I2C_DRIVER_H_ */
