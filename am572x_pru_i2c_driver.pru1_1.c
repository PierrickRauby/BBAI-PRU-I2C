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
#include <stdint.h>
#include <pru_cfg.h>
#include <stdio.h>
#include <stdlib.h>            // atoi
#include <string.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <hw_types.h>
#include "am572x_pru_i2c.h"
#include "am572x_pru_i2c_driver.h"


/*#define CM_L4PER_I2C1_CLKCTRL 0x4A0097A0 //(p1079)*/
/* WARNING: the next adress for I2C1 is not correct and actually point to i2c4 */
#define CM_L4PER_I2C1_CLKCTRL 0x4A0097B8 //(p1079)
#define MAX_CYCLES_WAITING    200000 // max cycle of pru clock to wait
#define DEBUG_REG             0x4807A024 //Adress to read a return if fail

volatile pruI2C *PRU_I2C=&CT_I2C1;
uint8_t pru_i2c_initialized=0;
uint8_t pru_i2c_driver_init( uint8_t i2cDevice){
  /* Function that initiate the i2c bus*/
  /* 24.1.5.1.1.1.1 Configure the Module Before Enabling the I2C Controller*/
  /* 1. Enable the functional and interface clocks (p5740).*/
  /* Enable the Gate CM_L4PER_I2C1_CLCKCTRL by writting 0x2 (p1079).*/
  HWREG(CM_L4PER_I2C1_CLKCTRL)=0x2;
  __delay_cycles(100000000/5);   // Wait 1/20 second, not sure if it is useful
  /*Module config (not required) in SYSC*/
  /*Do Not Uncomment the 4 following lines, I don't even know why I don't delete them*/
  /*Investigate on the use of the 4 following lines*/
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
  /*write 0x0 to OA register */
  PRU_I2C->I2C_OA_bit.OA=0x0;
  /* 6. Set the TX threshold (in transmitter mode) and the RX threshold 
     (in receiver mode) by setting the I2Ci.I2C_BUF[5:0] TXTRSH bit field to 
     (TX threshold – 1) and the I2Ci.I2C_BUF[13:8] RXTRSH bit*/
  /*write 0x501, default value to I2C_BUF, that is 0x1 in TXTRSH and 0x5 in 
    RXTRSH */
  PRU_I2C->I2C_BUF_bit.TXTRSH=0x01;
  PRU_I2C->I2C_BUF_bit.RXTRSH=0x05;
  /* 7. Take the I2C controller out of reset by setting the I2Ci.I2C_CON[15] 
     I2C_EN bit to 1*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x1;
  return 1;
}

/* Helper function for polling usefull,
   these function return the value of the bit and poll for 1us max 20000 cycles
   at PRU clock of 200MHz*/

uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(uint8_t i2cDevice){
  /* Test if Bus is free return 0 if yes */
  uint32_t ticks = 0;
  while (PRU_I2C->I2C_IRQSTATUS_RAW_bit.XDR){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 1; /*Bus is occupied*/
    }
  }
  return 0; /*Bus is free*/
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
  uint32_t ticks = 0;
  while (!PRU_I2C->I2C_IRQSTATUS_RAW_bit.XRDY){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 0;
    }
  }
  return 1;
}
uint8_t pru_i2c_poll_I2C_IRQSTATUS_RAW_RRDY(uint8_t i2cDevice){
  uint32_t ticks = 0;
  while (!PRU_I2C->I2C_IRQSTATUS_RAW_bit.RRDY){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING){
      return 0;
    }
  }
  return 1;
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
  uint32_t ticks = 0;
  while (PRU_I2C->I2C_IRQSTATUS_RAW_bit.BF){
    ticks++;
    if (ticks > MAX_CYCLES_WAITING)
    {
      return 0; //Bus is free
    }
  }
  return 1; //Bus is ready
}

/*Read, write function*/
long pru_i2c_driver_read_byte(uint8_t address, uint8_t reg, uint8_t bytes,
    uint8_t *buffer){
  /*TODO: Document this function*/
  if(!pru_i2c_initialized){
    /* If bus is not initialized then try to initialized it*/
    if(!pru_i2c_driver_init(1)){
      return HWREG(DEBUG_REG);
    }
  }
  /* 24.1.5.1.1.1.2 Initialize the I2C Controller*/
  /*1. Configure the I2Ci.I2C_CON register:*/
  /*For master or slave mode, set the I2Ci.I2C_CON[10] MST bit (0: slave;
      1: master).*/
  /*For transmitter or receiver mode, set the I2Ci.I2C_CON[9] TRX bit 
      (0: receiver; 1: transmitter).*/
  PRU_I2C->I2C_CON_bit.MST=0x1; //master
  PRU_I2C->I2C_CON_bit.TRX=0x1; //transmit
/*2. If using an interrupt to transmit and receive data, set the corresponding
  bit in the I2Ci.I2C_IRQENABLE_SET register to 1 (the I2Ci.I2C_IRQENABLE_SET
  [4] XRDY_IE bit for the transmit interrupt, the I2Ci.I2C_IRQENABLE_SET [3]
  RRDY bit for the receive interrupt).*/
  PRU_I2C->I2C_IRQENABLE_SET_bit.XRDY_IE=0x1; //Transmit interrupt
  /*long test=HWREG(0x4807A02C);*/
  /*return test; //Bus is free*/
  /*24.1.5.1.1.1.3 Configure Slave Address and the Data Control Register 
    In master mode, configure the slave address register by programming the 
    I2Ci.I2C_SA[9:0] SA bit field and the number of data bytes 
    (I2C data payload) associated with the transfer by programming the I2Ci.I2C_CNT[15:0] DCOUNT bit field.
  */
  PRU_I2C->I2C_SA_bit.SA=address; // addess argument
  PRU_I2C->I2C_CNT_bit.DCOUNT=1; // 1 byte to transmit
  /*24.1.5.1.1.1.4 Initiate a Transfer
     Poll the I2Ci.I2C_IRQSTATUS_RAW [12] BB bit. If it is cleared to 0 
     (bus not busy), configure the I2Ci.I2C_CON[0] STT and I2Ci.I2C_CON[1] STP 
     bits. To initiate a transfer, the I2Ci.I2C_CON[0] STT bit must be set to 1,
     and it is not mandatory to set the I2Ci.I2C_CON[1] STP bit to 1.
  */
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_BB(1)){return HWREG(DEBUG_REG);}
  PRU_I2C->I2C_CON_bit.STT=0x1;
  /*PRU_I2C->I2C_CON_bit.STP=0x1;*/ //(don't want to send stop condition now
/*24.1.5.1.1.1.6 Transmit Data
  Poll the I2Ci.I2C_IRQSTATUS_RAW [4] XRDY bit, or use the XRDY interrupt 
  (the I2Ci.I2C_IRQENABLE_SET [4] XRDY_IE bit must be set to 1) or the DMA TX
  channel (the I2Ci.I2C_BUF[7] XDMA_EN bit must be set to 1 together with 
  I2C_DMATXENABLE_SET) to write data to the I2Ci.I2C_DATA register.
  If the transfer length does not equal the TX FIFO threshold (the 
  I2Ci.I2C_BUF[5:0] TXTRSH bit field + 1), use the draining feature 
  (enable the XDR interrupt by setting the I2Ci.I2C_IRQENABLE_SET [14] 
  XDR_IE bit to 1).*/
  // wait for the start condition to be written (7micro sec at 200MHz) 
  /*__delay_cycles(1400);   // Wait 1/2 second*/
  if(!pru_i2c_poll_I2C_IRQSTATUS_RAW_XRDY(1)){ return HWREG(DEBUG_REG);}
  /*write register to read*/
  reg=0x11;
  PRU_I2C->I2C_DATA_bit.DATA=reg;

  PRU_I2C->I2C_CON_bit.STT=0x1;
  /* initiate transfer*/
  PRU_I2C->I2C_IRQSTATUS_RAW_bit.XRDY=0x1;
  /* check the interupt as in Fig20-24 (p5744)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return HWREG(DEBUG_REG);}
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return HWREG(DEBUG_REG);}
  /*Check if data as already been access, if not I can processed*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_ARDY(1)){return HWREG(DEBUG_REG);}
  /* wait for the data */
  if(!pru_i2c_poll_I2C_IRQSTATUS_RAW_RRDY(1)){return 7;return HWREG(DEBUG_REG);}
  uint8_t count;
  for(count=0;count<bytes;count++){
    /* Get the data*/
    buffer[count]=PRU_I2C->I2C_DATA;
    /*Clear RRDY to tell that new data is ready to be read*/
    PRU_I2C->I2C_IRQSTATUS_RAW_bit.RRDY=0x1;
    /* Back to the top of diagram in Fig20-24 (p5744)*/
  /* check the interupt as in figure20-24 (p5744)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 8;return HWREG(DEBUG_REG);}
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 9;return HWREG(DEBUG_REG);}
  /* Check that the registers can be accessed again*/
  if(!pru_i2c_poll_I2C_IRQSTATUS_RAW_ARDY(1)){return 10;return HWREG(DEBUG_REG);}
    /*Wait for the next data*/
    if(!pru_i2c_poll_I2C_IRQSTATUS_RAW_RRDY(1)){return 11;return HWREG(DEBUG_REG);}
  }
  /* check the interupt as in figure20-24 (p5744)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 12;return HWREG(DEBUG_REG);}
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 13;return HWREG(DEBUG_REG);}
  /* Check that the registers can be accessed again*/
  if(!pru_i2c_poll_I2C_IRQSTATUS_RAW_ARDY(1)){return 14;return HWREG(DEBUG_REG);}
    /*Wait for the next data*/
    if(!pru_i2c_poll_I2C_IRQSTATUS_RAW_BF(1)){return 15;return HWREG(DEBUG_REG);}
  return count;
}


/* The following function use the section 24.1.5.1.1.3 (p5742) of the TRM
    the diagramme used are Figure 24-19 to Figure 24-21*/
uint8_t pru_i2c_driver_init_from_graph(uint8_t i2cDevice, uint8_t dcount,
    uint8_t address){
  /* this function setups the I2C based on diagram 24-19 (p5743)*/
  /* Close the gate to provide clock to I2C*/
  HWREG(CM_L4PER_I2C1_CLKCTRL)=0x2;
  __delay_cycles(100000000/5);   // Wait 1/20 second, not sure if it is useful
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
  PRU_I2C->I2C_SA_bit.SA=address;
  /*Write I2Ci.I2C_CNT[15:0] DCOUNT bit field (master mode)*/
  PRU_I2C->I2C_CNT_bit.DCOUNT=dcount;
  /*Write I2Ci.I2C_IRQENABLE_SET register (enable interrupt)*/
  /*set bit 4,3,2,1 and 0 (p5766)*/
  PRU_I2C->I2C_IRQENABLE_SET=0x1f;
  /*Write I2Ci.I2C_BUF register (for DMA use)*/
  /* Nothing done here I am not using DMA*/
return 1;
}


long pru_i2c_driver_transmit_byte_from_graph(uint8_t address, uint8_t reg, uint8_t bytes,
    uint8_t *buffer){
  /* this function setups the I2C based on diagram 24-20 (p5743)*/
  if(!pru_i2c_initialized){
    /* If bus is not initialized then try to initialized it*/
    if(!pru_i2c_driver_init(1)){
      return HWREG(DEBUG_REG);
    }
  }
  /*[EXPECTED I2C_IRQENABLE_&((*PRU_I2Cmain).I2C_SBLOCK)CLR = FFFFh]*/
  /*(*PRU_I2C).I2C_IRQENABLE_CLR=0xFFFF;*/
  /*I2Ci.I2C_IRQSTATUS_RAW[12] BB bit = 0?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_BB(1)){return HWREG(DEBUG_REG);}
  /*2Ci.I2C_CON with 8603h or 8601h (F/S mode) */
  /* write 8601h (no STP condition)*/
  PRU_I2C->I2C_CON_bit.I2C_EN=0x0; //EN
  PRU_I2C->I2C_CON_bit.MST=0x1; //master
  PRU_I2C->I2C_CON_bit.TRX=0x1; //transmit
  PRU_I2C->I2C_CON_bit.STT=0x1; //STT
  /*return 42;*/
  return HWREG(0x4807A0A4);
  /*Is ACK returned (NACK=0)? (continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_NACK(1)){return 2; return HWREG(DEBUG_REG);}
  /*Is arbitration lost (AL=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_AL(1)){return 3; return HWREG(DEBUG_REG);}
  /*Can update the registers (ARDY=1)?(continue if no)*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_ARDY(1)){return 4; return HWREG(DEBUG_REG);}
  /*Is send data required to end transfer (XDR=1)?(continue if no)*/
  /*Current implementation assumes that we do not need the draining feature*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(1)){return 5; return HWREG(DEBUG_REG);}
  /*Is send data being requested (XRDY bit=1)?*/
  if(pru_i2c_poll_I2C_IRQSTATUS_RAW_XDR(1)){return 6; return HWREG(DEBUG_REG);}
  PRU_I2C->I2C_DATA_bit.DATA=reg;
  /*Clear XRDY bit (see Note 1*/
  PRU_I2C->I2C_IRQSTATUS_bit.XRDY=1;
  return HWREG(0x4807A024);
  }
