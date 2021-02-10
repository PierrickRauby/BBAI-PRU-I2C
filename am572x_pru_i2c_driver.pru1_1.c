/*
 * am_572x_pru_i2c_driver.h
 * Author Pierrick Rauby : pierrick.rauby@gmail.com
 * Created on Feb8 2021
 * For Texas Instrument am572x:
 *    - based on the TRM SPRUHZ6L 24.1.5 HS I2C Programming Guide p5740
 *    - if not mentionned otherwise the page number in the comments refere to 
 *        the TRM SPRUHZ6L
 * Function:
 *    - Use the PRU to configure the i2c1
 *    - The i2c is then used to get data from the Beaglebone AI onboard ADC
 * Compile:
 *    - Compiles with the same Makefile as other PRU example in the Cloud9 
 *        folder
 *
 */

#include "include/am572x_pru_i2c.h"
#include "include/am572x_pru_i2c_driver.h"
#define CM_L4PER_I2C1_CLKCTRL 0x4A0097A0

volatile pruI2C* PRU_I2C
uint8_t pru_i2c_driver_init( uint8_t i2cDevice){
  /* 24.1.5.1.1.1.1 Configure the Module Before Enabling the I2C Controller*/
  /* 1. Enable the functional and interface clocks (p5740).*/
  /* Enable the Gate CM_L4PER_I2C1_CLCKCTRL by writting 0x2 (p1079).*/
  HWREG(CM_L4PER_I2C1_CLKCTRL)=0x2;

 /*Module config (not required) in SYSC*/
  PRU_I2C->I2C_SYSC_bit.AUTOIDLE=0x0; // Deactivate auto-idle
  PRU_I2C->I2C_SYSC_bit.ENAWAKEUP=0x0; // Deactivate Wakeup mechanisme 
  PRU_I2C->I2C_SYSC_bit.IDLEMODE=0x01; // Deactivate auto-idle
  PRU_I2C->I2C_SYSC_bit.CLKACTIVITY=0x00; // Deactivate auto-idle

  /* 2. Program the I2Ci.I2C_PSC[7:0], to obtain 12MHz.*/
  /*write 0x9 to setup for fast mode (p5731).*/
  PRU_I2C->I2C_PSC_bit.PSC=0x9;

  /* 3. Program the I2Ci.I2C_SCLL[7:0]SCLL and I2Ci.I2C_SCLH[7:0]SCLH bit fields
    to obtain a bit rate of 100 kbps or 400 kbps*/
  /* write 0x5 and 0x7 respectiverly (p5731)*/
  PRU_I2C->I2C_SCLL_bit.SCLL=0x5;
  PRU_I2C->I2C_SCLH_bit.SCLH=0x7;
 
  /* 4. (Optionnal) for Hight Speed I2C not implemented here*/
 
  /* 5. Configure the Own Address of the I2C controller by storing it in the 
    I2Ci.I2C_OAregister. */
  /*write 0x0 to OAregister */
  PRU_I2C->I2C_OA_bit.OA=0x0;
 
  /* 6. Set the TX threshold (in transmitter mode) and the RX threshold 
    (in receiver mode) by setting the I2Ci.I2C_BUF[5:0] TXTRSH bit field to 
    (TX threshold – 1) and the I2Ci.I2C_BUF[13:8] RXTRSH bit*/
  /*write 0x501, default value to I2C_BUF*/ 
  /*TODO: Check that I can actually set the entire register this way without 
    setting each bit field separately*/
  PRU_I2C->I2C_BUF_bit=0x501;
 
  /* 7. Take the I2C controller out of reset by setting the I2Ci.I2C_CON[15] 
     I2C_EN bit to 1*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x1;

  /* 24.1.5.1.1.1.2 Initialize the I2C Controller*/
  /*1. Configure the I2Ci.I2C_CON register:*/
  /*For master or slave mode, set the I2Ci.I2C_CON[10] MST bit (0: slave;
      1: master).*/
  /*For transmitter or receiver mode, set the I2Ci.I2C_CON[9] TRX bit 
      (0: receiver; 1: transmitter).*/
  PRU_I2C->I2C_CON_bit.MST=0x1; //master
  PRU_I2C->I2C_CON_bit.TRX=0x1; //transmit

return 1;




  }
