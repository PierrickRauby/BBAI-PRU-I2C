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

uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(uint8_t i2cDevice);
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_RDR(uint8_t i2cDevice);
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_BB(uint8_t i2cDevice);
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_XRDY(uint8_t i2cDevice);
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_RRDY(uint8_t i2cDevice);
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_ARDY(uint8_t i2cDevice);
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(uint8_t i2cDevice);
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(uint8_t i2cDevice);
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_BF(uint8_t i2cDevice);
uint8_t pru_i2c_driver_init(uint8_t i2cDevice, uint8_t dcount,
    uint8_t address);
long pru_i2c_driver_transmit_byte(uint8_t address, uint8_t reg,
    uint8_t bytes, uint8_t *buffer);
long pru_i2c_driver_transmit_bytes(uint8_t address, uint8_t reg,
    uint8_t bytes, uint8_t *buffer);
long pru_i2c_driver_receive_byte(uint8_t address, uint8_t reg,
    uint8_t bytes,uint8_t *buffer);
long pru_i2c_driver_receive_bytes(uint8_t address, uint8_t reg,
    uint8_t bytes,uint8_t *buffer);
uint8_t pru_i2c_driver_software_reset(uint8_t i2cDevice);
#endif /* AM572x_PRU_I2C_DRIVER_H_ */
