/*
 * am_572x_pru_i2c_driver.h
 * Author Pierrick Rauby : pierrick.rauby@gmail.com
 * Created on Feb8 2021
 * For Texas Instrument am572x:
 *    - based on the TRM SPRUHZ6L 24.1.5 HS I2C Programming Guide p5740
 *    - if not mentionned otherwise the page number in the comments refere to 
 *        the TRM SPRUHZ6L
 * Function:
 *    - Use the PRU to configure the i2c4
 *    - The i2c is then used to get data from the Beaglebone AI onboard ADC
 * Compile:
 *    - Compiles with the same Makefile as other PRU example in the Cloud9 
 *        folder
 *
 */
#include <stdint.h>
#include <pru_cfg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <hw_types.h>
#include "am572x_pru_i2c.h"
#include "am572x_pru_i2c_driver.h"
#include "Adafruit_MMA8451.h"

#define CM_L4PER_I2C4_CLKCTRL 0x4A0097B8 //(p1079)
#define MAX_CYCLES_WAITING    200000 // max cycle of pru clock to wait
#define DEBUG_REG             0x48070024 // IRQSTATUS_RAW

volatile pruI2C *PRU_I2C=&CT_I2C4;
uint8_t pru_i2c_initialized=0;

/* Helper function for polling usefull,
   these function return the value of the bit and poll for 1us max 20000 cycles
   at PRU clock of 200MHz*/
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(uint8_t i2cDevice){
  /* Test if TRANSMIST draining IRQ enable, return 1 if yes */
  uint32_t ticks = 0;
  while (PRU_I2C->I2C_IRQSTATUS_RAW_bit.XDR){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 1; /* Transmit Draining feature enabled */
    }
  }
  return 0; /* Transmit Draining feature inactive */
}
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_RDR(uint8_t i2cDevice){
  /* Test if RECEIVE draining IRQ enable, return 1 if yes */
  uint32_t ticks = 0;
  while (PRU_I2C->I2C_IRQSTATUS_RAW_bit.RDR){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 1; /* Received Draining feature enabled*/
    }
  }
  return 0; /*Received Draining feature inactive*/
}
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_BB(uint8_t i2cDevice){
  /* Test if Bus is free return 0 if yes */
  uint32_t ticks = 0;
  while (PRU_I2C->I2C_IRQSTATUS_RAW_bit.BB){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 1; /*Bus is occupied*/
    }
  }
  return 0; /*Bus is free*/
}
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_XRDY(uint8_t i2cDevice){
  /* Test if Transmit data ready, return 1 if yes*/
  uint32_t ticks = 0;
  while (!PRU_I2C->I2C_IRQSTATUS_RAW_bit.XRDY){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 0; /*Transmission ongoing*/
    }
  }
  return 1; /* Transimission ready */
}
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_RRDY(uint8_t i2cDevice){
  /* Test if Receive data ready, return 1 if yes*/
  uint32_t ticks = 0;
  while (!PRU_I2C->I2C_IRQSTATUS_RAW_bit.RRDY){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 0; /* No data available */
    }
  }
  return 1; /* Receive data available */
}
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_ARDY(uint8_t i2cDevice){
  /* Test if Access is ready return 1 if yes*/
  uint32_t ticks = 0;
  while (PRU_I2C->I2C_IRQSTATUS_RAW_bit.ARDY){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 1; /*Access ready */
    }
  }
  return 0; /* Module busy*/
}
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(uint8_t i2cDevice){
  /*Test if NACK return 1 if yes*/
  uint32_t ticks = 0;
  while (PRU_I2C->I2C_IRQSTATUS_RAW_bit.NACK){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 1; /* Not Acknowledge detected*/
    }
  }
  return 0; /*Normal operation */
}
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(uint8_t i2cDevice){
  /*Test if Arbitration Loss return 1 is yes*/
  uint32_t ticks = 0;
  while (PRU_I2C->I2C_IRQSTATUS_RAW_bit.AL){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 1; /*Arbitration Loss detected*/
    }
  }
  return 0; /* Normal operation*/
}
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_BF(uint8_t i2cDevice){
  /*Test if Bus is free*/
  uint32_t ticks = 0;
  while (PRU_I2C->I2C_IRQSTATUS_RAW_bit.BF){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 0; /* No action */
    }
  }
  return 1; /*Bus is ready */
}
/* The following function use the section 24.1.5.1.1.3 (p5742) of the TRM
   the diagramme used are Figure 24-19 to Figure 24-21*/
uint8_t pru_i2c_driver_init(uint8_t i2cDevice, uint16_t dcount,
    uint16_t address){
  /* this function setups the I2C based on diagram 24-19 (p5743)*/
  /* Close the gate to provide clock to I2C*/
  HWREG(CM_L4PER_I2C4_CLKCTRL)=0x2;
  __delay_cycles(20000);   // Wait for clock to be active
  /* Write I2Ci.I2C_PSC[7:0] PSC (prescaler)*/
  /*write 0x9 to setup for fast mode (p5731).*/
  PRU_I2C->I2C_PSC_bit.PSC=0x9;
  /* Write I2Ci.I2C_SCLL (F/S mode: only I2Ci.I2C_SCLL[7:0] SCLL bit field)*/
  /* write 0x5 (p5731)*/
  PRU_I2C->I2C_SCLL_bit.SCLL=0x5;
  /* Write I2Ci.I2C_SCLH (F/S mode: only I2Ci.I2C_SCLH[7:0] SCLH bit field)*/
  /* write 0x7 (p5731)*/
  PRU_I2C->I2C_SCLH_bit.SCLH=0x7;
  /*2Ci.I2C_OA (F/S mode: I2Ci.I2C_OA[9:0] OA bit field)*/
  PRU_I2C->I2C_OA_bit.OA=0x0;
  /* Set I2Ci.I2C_CON[15] I2C_EN bit to 1 (enable module)*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x1;
  /* If in master mode I need to configure SA an DCOUNT*/
  /*TODO: implement if test for master mode*/
  /*Write I2Ci.I2C_SA[9:0] SA bit field (master mode)*/
  /*if(address>0x7f){*/
  /*PRU_I2C->I2C_CON_bit.XSA=0x1;*/
  /*}*/
  PRU_I2C->I2C_SA_bit.SA=address;
  /*Write I2Ci.I2C_CNT[15:0] DCOUNT bit field (master mode)*/
  PRU_I2C->I2C_CNT_bit.DCOUNT=dcount;
  /*pru_i2c_initialized=1;*/
  return 1;
}

uint8_t pru_i2c_driver_transmit_byte(uint16_t address, uint16_t reg,
    uint16_t bytes,uint16_t *buffer){
  /* this function setups the I2C based on diagram 24-20 (p5744)*/
  if(!pru_i2c_initialized){
     /*If bus is not initialized then try to initialized it*/
    if(!pru_i2c_driver_init(1,bytes+1,address)){
    /*if(!pru_i2c_driver_init(1)){*/
      return 10;
    }
  }
  /*[EXPECTED I2C_IRQENABLE_&((*PRU_I2Cmain).I2C_SBLOCK)CLR = FFFFh]*/
  (*PRU_I2C).I2C_IRQENABLE_CLR=0xFFFF;
  /*I2Ci.I2C_IRQSTATUS_RAW[12] BB bit = 0?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_BB(1)){return 11;}
  /*2Ci.I2C_CON with 8603h or 8601h (F/S mode) */
  /* write 8601h (no STP condition)*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x1; //EN
  PRU_I2C->I2C_CON_bit.MST=0x1; //master
  PRU_I2C->I2C_CON_bit.TRX=0x1; //transmit
  PRU_I2C->I2C_CON_bit.STT=0x1; //STT
  PRU_I2C->I2C_CON_bit.STP=0x1; //STP
  /*Is ACK returned (NACK=0)? (continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 12;}
  /*Is arbitration lost (AL=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 13;}
  /*Is send data required to end transfer (XDR=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(1)){
    /*Read I2Ci.I2C_BUFSTAT[5:0] TXSTAT to check the amount of data to be 
      transmitted*/
    uint8_t TXSTAT_value=PRU_I2C->I2C_BUFSTAT_bit.TXSTAT;
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUFSTAT[5:0] TXSTAT times*/
    /*uint8_t i;*/
    /*for(i=0;i<1;i++){*/
      PRU_I2C->I2C_DATA_bit.DATA=reg;
    /*}*/
    /*Clear XDR bit (see Note 1)*/
    PRU_I2C->I2C_IRQSTATUS_bit.XDR=1;
  }
  /*Is send data being requested (XRDY bit=1)?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XRDY(1)){
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUF[5:0] XTRSH + 1 times */
    /* /!\ ERROR IN DOC I2C_BUF[5:0] is TXTRSH not XTRSH*/
    uint8_t TXTRSH_value=PRU_I2C->I2C_BUF_bit.TXTRSH;
    /*uint8_t i;*/
    /*TXTRSH_value+=1;*/
    /*return reg;*/
    /*for(i=0;i<TXTRSH_value;i++){ // not shure about that ! */
      PRU_I2C->I2C_DATA_bit.DATA=reg;
    /*}*/
    /*Clear XRDY bit (see Note 1*/
    PRU_I2C->I2C_IRQSTATUS_bit.XRDY=1;
  }
  // Sending the data now that I have let the device know
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 15;}
  /*Is arbitration lost (AL=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 16;}
  /*Is send data required to end transfer (XDR=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(1)){
    /*Read I2Ci.I2C_BUFSTAT[5:0] TXSTAT to check the amount of data to be 
      transmitted*/
    uint8_t TXSTAT_value=PRU_I2C->I2C_BUFSTAT_bit.TXSTAT;
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUFSTAT[5:0] TXSTAT times*/
    uint16_t i;
    for(i=0;i<bytes;i++){
      PRU_I2C->I2C_DATA_bit.DATA=buffer[0];
    }
    /*Clear XDR bit (see Note 1)*/
    PRU_I2C->I2C_IRQSTATUS_bit.XDR=1;
  }
  /*Is send data being requested (XRDY bit=1)?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XRDY(1)){
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUF[5:0] XTRSH + 1 times */
    /* /!\ ERROR IN DOC I2C_BUF[5:0] is TXTRSH not XTRSH*/
    uint8_t TXTRSH_value=PRU_I2C->I2C_BUF_bit.TXTRSH;
    uint8_t i;
    TXTRSH_value+=1;
    for(i=0;i<TXTRSH_value;i++){
      PRU_I2C->I2C_DATA_bit.DATA=buffer[0];
    }
    /*Clear XRDY bit (see Note 1)*/
    PRU_I2C->I2C_IRQSTATUS_bit.XRDY=1;
  }
    return 1;
}
uint8_t pru_i2c_driver_transmit_bytes(uint16_t address, uint16_t reg,
    uint16_t bytes,uint16_t *buffer){
  /* this function setups the I2C based on diagram 24-20 (p5744)*/
  if(!pru_i2c_initialized){
    if(!pru_i2c_driver_init(1,bytes+1,address)){
      return 10;
    }
  }
  /*[EXPECTED I2C_IRQENABLE_&((*PRU_I2Cmain).I2C_SBLOCK)CLR = FFFFh]*/
  (*PRU_I2C).I2C_IRQENABLE_CLR=0xFFFF;
  /*I2Ci.I2C_IRQSTATUS_RAW[12] BB bit = 0?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_BB(1)){return 11;}
  /*2Ci.I2C_CON with 8603h or 8601h (F/S mode) */
  /* write 8601h (no STP condition)*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x1; //EN
  PRU_I2C->I2C_CON_bit.MST=0x1; //master
  PRU_I2C->I2C_CON_bit.TRX=0x1; //transmit
  PRU_I2C->I2C_CON_bit.STT=0x1; //STT
  PRU_I2C->I2C_CON_bit.STP=0x1; //STP
  /*Is ACK returned (NACK=0)? (continue if no)*/
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 12;}
    /*Is arbitration lost (AL=1)?(continue if no)*/
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 13;}
    /*Is send data required to end transfer (XDR=1)?(continue if no)*/
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(1)){
      /*Read I2Ci.I2C_BUFSTAT[5:0] TXSTAT to check the amount of data to be 
        transmitted*/
      uint16_t TXSTAT_value=PRU_I2C->I2C_BUFSTAT_bit.TXSTAT;
      /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUFSTAT[5:0] TXSTAT times*/
      /*for(i=0;i<1;i++){*///removed that because I only send the register here
        /*TODO: check that I am writin what I want here*/
        PRU_I2C->I2C_DATA_bit.DATA=reg;
      /*}*/
      /*Clear XDR bit (see Note 1)*/
      PRU_I2C->I2C_IRQSTATUS_bit.XDR=1;
    }
    /*Is send data being requested (XRDY bit=1)?*/
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XRDY(1)){
      /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUF[5:0] XTRSH + 1 times */
      /* /!\ ERROR IN DOC I2C_BUF[5:0] is TXTRSH not XTRSH*/
      uint16_t TXTRSH_value=PRU_I2C->I2C_BUF_bit.TXTRSH;
      uint16_t i;
      TXTRSH_value+=1;
      for(i=0;i<TXTRSH_value;i++){ // not shure about that ! 
        PRU_I2C->I2C_DATA_bit.DATA=reg;
      }
      /*Clear XRDY bit (see Note 1*/
      PRU_I2C->I2C_IRQSTATUS_bit.XRDY=1;
    }
  uint16_t l;
  for(l=0;l<bytes;l++){
  __delay_cycles(6000); 
    // Sending the data now that I have let the device know
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 15;}
    /*Is arbitration lost (AL=1)?(continue if no)*/
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 16;}
  /*Is send data required to end transfer (XDR=1)?(continue if no)*/
  // TODO the following for loop is a tentative to send multiple bytes
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(1)){
    /*Read I2Ci.I2C_BUFSTAT[5:0] TXSTAT to check the amount of data to be 
      transmitted*/
    uint8_t TXSTAT_value=PRU_I2C->I2C_BUFSTAT_bit.TXSTAT;
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUFSTAT[5:0] TXSTAT times*/
    uint16_t i;
    for(i=0;i<bytes;i++){
      /*TODO: check that I am writing what I want here*/
      PRU_I2C->I2C_DATA_bit.DATA=buffer[l];
      /*PRU_I2C->I2C_DATA_bit.DATA=0x80;*/
    }
    /*Clear XDR bit (see Note 1)*/
    PRU_I2C->I2C_IRQSTATUS_bit.XDR=1;
  }
  /*Is send data being requested (XRDY bit=1)?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XRDY(1)){
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUF[5:0] XTRSH + 1 times */
    /* /!\ ERROR IN DOC I2C_BUF[5:0] is TXTRSH not XTRSH*/
    uint16_t TXTRSH_value=PRU_I2C->I2C_BUF_bit.TXTRSH;
    uint16_t i;
    TXTRSH_value+=1;
    for(i=0;i<TXTRSH_value;i++){
      PRU_I2C->I2C_DATA_bit.DATA=buffer[l];
    }
    /*Clear XRDY bit (see Note 1)*/
    PRU_I2C->I2C_IRQSTATUS_bit.XRDY=1;
  }
  }
    return 1;
}

uint8_t pru_i2c_driver_receive_byte(uint16_t address, uint16_t reg,
    uint16_t bytes,uint16_t *buffer){
  /* this function setups the I2C based on diagram 24-20 (p5743) and for the
     transmitter part on diagram 24-21 (p5746) */
  if(!pru_i2c_initialized){
     /*If bus is not initialized then try to initialized it*/
    if(!pru_i2c_driver_init(1,bytes+1,address)){
    /*if(!pru_i2c_driver_init(1)){*/
      return 172;
    }
  }
  /*[EXPECTED I2C_IRQENABLE_&((*PRU_I2Cmain).I2C_SBLOCK)CLR = FFFFh]*/
  (*PRU_I2C).I2C_IRQENABLE_CLR=0xFFFF;
  /*I2Ci.I2C_IRQSTATUS_RAW[12] BB bit = 0?*/
  // TODO: attention j'ai retirer le delay suivant ce qui peut cause une bug 
  /*__delay_cycles(20000);*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_BB(1)){return 171;}
  /*2Ci.I2C_CON with 8603h or 8601h (F/S mode) */
  /* write 8601h (no STP condition)*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x1; //EN
  PRU_I2C->I2C_CON_bit.MST=0x1; //master
  PRU_I2C->I2C_CON_bit.TRX=0x1; //receive
  PRU_I2C->I2C_CON_bit.STT=0x1; //STT
  /*Is ACK returned (NACK=0)? (continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 18;}
  /*Is arbitration lost (AL=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 19;}
  /*Is send data required to end transfer (XDR=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(1)){
    /*Read I2Ci.I2C_BUFSTAT[5:0] TXSTAT to check the amount of data to be 
      transmitted*/
    PRU_I2C->I2C_DATA_bit.DATA=reg;
    /*Clear XDR bit (see Note 1)*/
    PRU_I2C->I2C_IRQSTATUS_bit.XDR=1;
  }
  /*Is send data being requested (XRDY bit=1)?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XRDY(1)){
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUF[5:0] XTRSH + 1 times */
    /* /!\ ERROR IN DOC I2C_BUF[5:0] is TXTRSH not XTRSH*/
    uint16_t TXTRSH_value=PRU_I2C->I2C_BUF_bit.TXTRSH;
    uint16_t i;
    TXTRSH_value+=1;
    for(i=0;i<TXTRSH_value;i++){
      PRU_I2C->I2C_DATA_bit.DATA=reg;
    }
    /*Clear XRDY bit (see Note 1*/
    PRU_I2C->I2C_IRQSTATUS_bit.XRDY=1;
    /*return PRU_I2C->I2C_IRQSTATUS_bit=1;*/
  }
  __delay_cycles(6000); 
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 21;}
  /*2Ci.I2C_CON with 8403h or 8401h (F/S mode) */
  /* write 8403h (STP condition this time)*/
  /*PRU_I2C->I2C_CON_bit.I2C_EN=0x1; //EN*/
  PRU_I2C->I2C_CON_bit.MST=0x1; //master
  PRU_I2C->I2C_CON_bit.TRX=0x0; //receive
  PRU_I2C->I2C_CON_bit.STT=0x1; //STT
  PRU_I2C->I2C_CON_bit.STP=0x1; //STP
  /*Is ACK returned (NACK=0)? (continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 22;}
  /*Is arbitration lost (AL=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 23;}
  /*Is send data required to end transfer (XDR=1)?(continue if no)*/
  /*Current implementation assumes that we do not need the draining feature*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_RDR(1)){
    /*Read I2Ci.I2C_BUFSTAT[13:8] RXSTAT to check the amount of data to be 
      received*/
    int16_t RXSTAT_value=PRU_I2C->I2C_BUFSTAT_bit.RXSTAT;
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUFSTAT[13:8] RXSTAT times*/
    uint16_t i;
    for(i=0;i<RXSTAT_value;i++){
      buffer[i]=PRU_I2C->I2C_DATA_bit.DATA;
    }
    /*Clear RDR bit (see Note 1)*/
    PRU_I2C->I2C_IRQSTATUS_bit.RDR=1;
  }
  /*Is enough  data being received (RRDY bit=1)?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_RRDY(1)){
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUF[13:0] XTRSH + 1 times */
    /* /!\ ERROR IN DOC I2C_BUF[13:8] is RXTRSH not RTRSH*/
    uint16_t RXTRSH_value=PRU_I2C->I2C_BUF_bit.RXTRSH;
    uint16_t i;
    RXTRSH_value+=1;
    /*return reg;*/
    for(i=0;i<RXTRSH_value;i++){
      buffer[i]=PRU_I2C->I2C_DATA_bit.DATA;
    }
    /*Clear RRDY bit (see Note 1*/
    PRU_I2C->I2C_IRQSTATUS_bit.RRDY=1;
    /*return PRU_I2C->I2C_IRQSTATUS_bit=1;*/
  }
  return 0;
}

uint8_t pru_i2c_driver_receive_bytes(uint16_t address, uint16_t reg,
    uint16_t bytes,uint16_t *buffer){
  /* this function setups the I2C based on diagram 24-20 (p5743) and for the
     transmitter part on diagram 24-21 (p5746) */
  if(!pru_i2c_initialized){
    if(!pru_i2c_driver_init(1,1,address)){
      return 17;
    }
  }
  /*[EXPECTED I2C_IRQENABLE_&((*PRU_I2Cmain).I2C_SBLOCK)CLR = FFFFh]*/
  (*PRU_I2C).I2C_IRQENABLE_CLR=0xFFFF;
  /*I2Ci.I2C_IRQSTATUS_RAW[12] BB bit = 0?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_BB(1)){return 171;}
  /*2Ci.I2C_CON with 8603h or 8601h (F/S mode) */
  /* write 8601h (no STP condition)*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x1; //EN
  PRU_I2C->I2C_CON_bit.MST=0x1; //master
  PRU_I2C->I2C_CON_bit.TRX=0x1; //transmit
  PRU_I2C->I2C_CON_bit.STT=0x1; //STT
  PRU_I2C->I2C_CON_bit.STP=0x0; //STP
  /*Is ACK returned (NACK=0)? (continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 18;}
  /*Is arbitration lost (AL=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 19;}
  /*Is send data required to end transfer (XDR=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(1)){
    /*Read I2Ci.I2C_BUFSTAT[5:0] TXSTAT to check the amount of data to be 
      transmitted*/
    PRU_I2C->I2C_DATA_bit.DATA=reg;
    /*Clear XDR bit (see Note 1)*/
    PRU_I2C->I2C_IRQSTATUS_bit.XDR=1;
  }
  /*Is send data being requested (XRDY bit=1)?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XRDY(1)){
    /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUF[5:0] XTRSH + 1 times */
    /* /!\ ERROR IN DOC I2C_BUF[5:0] is TXTRSH not XTRSH*/
    uint16_t TXTRSH_value=PRU_I2C->I2C_BUF_bit.TXTRSH;
    TXTRSH_value+=1;
    PRU_I2C->I2C_DATA_bit.DATA=reg;
    /*Clear XRDY bit (see Note 1*/
    PRU_I2C->I2C_IRQSTATUS_bit.XRDY=1;
    /*return PRU_I2C->I2C_IRQSTATUS_bit=1;*/
  }
  __delay_cycles(6000); 
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 21;}
  if(!pru_i2c_initialized){
    if(!pru_i2c_driver_init(1,bytes,address)){
      return 17;
    }
  }
  /*[EXPECTED I2C_IRQENABLE_&((*PRU_I2Cmain).I2C_SBLOCK)CLR = FFFFh]*/
  (*PRU_I2C).I2C_IRQENABLE_CLR=0xFFFF;
  /*I2Ci.I2C_IRQSTATUS_RAW[12] BB bit = 0?*/
  /*2Ci.I2C_CON with 8403h or 8401h (F/S mode) */
  /* write 8403h (STP condition this time)*/
  PRU_I2C->I2C_CON_bit.MST=0x1; //master
  PRU_I2C->I2C_CON_bit.TRX=0x0; //receive
  PRU_I2C->I2C_CON_bit.STT=0x1; //STT
  PRU_I2C->I2C_CON_bit.STP=0x1; //STP
  uint16_t l;
  for(l=0;l<bytes;l++){
    /*Is ACK returned (NACK=0)? (continue if no)*/
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 22;}
    /*Is arbitration lost (AL=1)?(continue if no)*/
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 23;}
    /*Is send data required to end transfer (XDR=1)?(continue if no)*/
    /*Current implementation assumes that we do not need the draining feature*/
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_RDR(1)){
      /*Read I2Ci.I2C_BUFSTAT[13:8] RXSTAT to check the amount of data to be 
        received*/
      int16_t RXSTAT_value=PRU_I2C->I2C_BUFSTAT_bit.RXSTAT;
      /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUFSTAT[13:8] RXSTAT times*/
      uint16_t i;
      for(i=0;i<RXSTAT_value;i++){
        buffer[l]=PRU_I2C->I2C_DATA_bit.DATA;
      }
      /*Clear RDR bit (see Note 1)*/
      PRU_I2C->I2C_IRQSTATUS_bit.RDR=1;
    }
    /*Is enough  data being received (RRDY bit=1)?*/
    if(pru_i2c_poll_I2C_IRQSTATUS_RAW_RRDY(1)){
      /*Write I2Ci.I2C_DATA register for I2Ci.I2C_BUF[13:0] XTRSH + 1 times */
      /* /!\ ERROR IN DOC I2C_BUF[13:8] is RXTRSH not RTRSH*/
      uint16_t RXTRSH_value=PRU_I2C->I2C_BUF_bit.RXTRSH;
      uint16_t i;
      RXTRSH_value+=1;
      /*return reg;*/
      for(i=0;i<RXTRSH_value;i++){
        buffer[l]=PRU_I2C->I2C_DATA_bit.DATA;
      }
      /*Clear RRDY bit (see Note 1*/
      PRU_I2C->I2C_IRQSTATUS_bit.RRDY=1;
    }
  }
  return 0;
}
uint8_t pru_i2c_driver_software_reset(uint8_t i2cDevice){
  /*24.1.4.3 HS I2C Software Reset*/
  /*1. Ensure that the module is disabled */
  /*   (clear the I2Ci.I2C_CON[15] I2C_EN bit to 0).*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x0;
  /*2. Set the I2Ci.I2C_SYSC[1] SRST bit to 1.*/
  PRU_I2C->I2C_SYSC_bit.SRST=0x1;
  /*3. Enable the module by setting I2Ci.I2C_CON[15] I2C_EN bit to 1.*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x1;
  /*4. Check the I2Ci.I2C_SYSS[0] RDONE bit until it is set to 1 to indicate*/
  /*   the software reset is complete.*/
  /* Wait 8 cycles for everything to complete, 8 found experimentaly*/
  __delay_cycles(8);
  return  PRU_I2C->I2C_SYSS_bit.RDONE;
}


