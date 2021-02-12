/*
 * am_572x_pru_i2c.h
 * Author Pierrick Rauby : pierrick.rauby@gmail.com
 * Created on Feb8 2021
 * For Texas Instrument am572x:
 *    - based on SPRUHZ6L, section 24.1.6 HS I2C Register Manual p5756
 *    - maps the hardware register of the I2C to struct to use on PRU code
 *    - /!\ Contraty to am335x the am57xx is 16 bit registers for the i2c
 //*
 */


#ifndef AM572x_PRU_I2C_H_
#define AM572x_PRU_I2C_H_

#include <stdint.h>

typedef struct {
  /* PRU_I2C_REVNB_LO register bit field */
  union{
    volatile uint16_t I2C_REVNB_LO;
      volatile struct {
        unsigned I2C_REVNB_LO :16; //15:0
      } I2C_REVNB_LO_bit;
  }; //0x0-0x1

  // 16 bit compensate offset
  //uint8_t rsvd02[2]; //0x2-0x3 

  /* PRU_I2C_REVNB_HI register bit field */
  union{
    volatile uint16_t I2C_REVNB_HI;
      volatile struct {
        unsigned I2C_REVNB_HI :16; //15:0
      } I2C_REVNB_HI_bit;
  }; //0x4-0x5

  // 16 bit compensate offset 
  // no merge with the following in case the compensation is not needed 
  //uint8_t rsvd06[2];  //0x06-0x7
  uint8_t rsvd08[8];  //0x08-0x0F

  /* PRU_I2C_SYSC register bit field */
  union {
    volatile uint16_t I2C_SYSC;

    volatile struct {
      unsigned AUTOIDLE     : 1;        //0:0
      unsigned SRST         : 1;        //1:1
      unsigned ENAWAKEUP    : 1;        //2:2
      unsigned IDLEMODE     : 2;        //4:3
      unsigned RSVD5        : 3;        //7:5
      unsigned CLKACTIVITY  : 2;        //9:8
      unsigned RSVD10       : 6;       //15:10
    } I2C_SYSC_bit;
  };  // 0x10-0x11

  // 16 bit compensate offset 
  //uint8_t rsvd012[2];  //0x12-0x13 
  uint8_t rsvd14[12];  // 0x14-0x1F

  /* I2C_EOI register bit field */
  union {
    volatile uint16_t I2C_EOI;

    volatile struct {
      unsigned LINE_NUMBER     : 1;        //0:0
      unsigned RSVD1           : 15;       //15:1
    } I2C_EOI_bit;
  };  // 0x20-0x21

   //16 bit compensate offset 
  //uint8_t rsvd22[2];  //0x22-0x23 

  /* I2C_IRQSTATUS_RAW register bit field */
  union {
    volatile uint16_t I2C_IRQSTATUS_RAW;

    volatile struct {
      unsigned AL      : 1;      //0
      unsigned NACK    : 1;      //1
      unsigned ARDY    : 1;      //2
      unsigned RRDY    : 1;      //3
      unsigned XRDY    : 1;      //4
      unsigned GC      : 1;      //5
      unsigned STC     : 1;      //6
      unsigned AERR    : 1;      //7
      unsigned BF      : 1;      //8
      unsigned AAS     : 1;      //9
      unsigned XUDF    : 1;      //10
      unsigned ROVR    : 1;      //11
      unsigned BB      : 1;      //12
      unsigned RDR     : 1;      //13
      unsigned XDR     : 1;      //14
      unsigned RSVD15  : 1;      //15
    } I2C_IRQSTATUS_RAW_bit; 
  };  // 0x24-0x25

  // 16 bit compensate offset 
  //uint8_t rsvd26[2];  //0x26-0x27

  /* I2C_IRQSTATUS register bit field */
  union {
    volatile uint16_t I2C_IRQSTATUS;

    volatile struct {
      uint16_t AL      : 1; // 0
      uint16_t NACK    : 1; // 1
      uint16_t ARDY    : 1; // 2
      uint16_t RRDY    : 1; // 3
      uint16_t XRDY    : 1; // 4
      uint16_t GC      : 1; // 5
      uint16_t STC     : 1; // 6
      uint16_t AERR    : 1; // 7
      uint16_t BF      : 1; // 8
      uint16_t AAS     : 1; // 9
      uint16_t XUDF    : 1; // 10
      uint16_t ROVR    : 1; // 11
      uint16_t BB      : 1; // 12
      uint16_t RDR     : 1; // 13
      uint16_t XDR     : 1; // 14
      uint16_t RSVD15 : 1; // 15
    } I2C_IRQSTATUS_bit;
  }; // 0x28-0x29

  // 16 bit compensate offset 
  //uint8_t rsvd2a[2];  //0x2A-0x2B

  /* I2C_IRQENABLE_SET register bit field */
  union {
    volatile uint16_t I2C_IRQENABLE_SET;

    volatile struct {
      uint16_t AL_IE   : 1; // 0
      uint16_t NACK_IE : 1; // 1
      uint16_t ARDY_IE : 1; // 2
      uint16_t RRDY_IE : 1; // 3
      uint16_t XRDY_IE : 1; // 4
      uint16_t GC_IE   : 1; // 5
      uint16_t STC_IE  : 1; // 6
      uint16_t AERR_IE : 1; // 7
      uint16_t BF_IE   : 1; // 8
      uint16_t AAS_IE  : 1; // 9
      uint16_t XUDF    : 1; // 10
      uint16_t ROVR    : 1; // 11
      uint16_t RSVD12  : 1; // 12
      uint16_t RDR_IE  : 1; // 13
      uint16_t XDR_IE  : 1; // 14
      uint16_t RSVD15  : 1; // 15
    } I2C_IRQENABLE_SET_bit;
  }; // 0x2C-0x2D

  // 16 bit compensate offset 
  //uint8_t rsvd2e[2];  //0x2E-0x2F

  /* SYS_I2C_IRQENABLE_CLR register bit field */
  union {
    volatile uint16_t I2C_IRQENABLE_CLR;

    volatile struct {
      uint16_t AL_IE   : 1; // 0
      uint16_t NACK_IE : 1; // 1
      uint16_t ARDY_IE : 1; // 2
      uint16_t RRDY_IE : 1; // 3
      uint16_t XRDY_IE : 1; // 4
      uint16_t GC_IE   : 1; // 5
      uint16_t STC_IE  : 1; // 6
      uint16_t AERR_IE : 1; // 7
      uint16_t BF_IE   : 1; // 8
      uint16_t AAS_IE  : 1; // 9
      uint16_t XUDF    : 1; // 10
      uint16_t ROVR    : 1; // 11
      uint16_t rsvd12  : 1; // 12
      uint16_t RDR_IE  : 1; // 13
      uint16_t XDR_IE  : 1; // 14
      uint16_t RSVD15  : 1; // 15
    } I2C_IRQENABLE_CLR_bit;
  }; // 0x30-0x31

  // 16 bit compensate offset 
  //uint8_t rsvd32[2];  //0x32-0x33

  /* I2C_WE register bit field */
  union {
    volatile uint16_t I2C_WE;

    volatile struct {
      uint32_t AL     : 1; // 0
      uint32_t NACK   : 1; // 1
      uint32_t ARDY   : 1; // 2
      uint32_t DRDY   : 1; // 3
      uint32_t RSVD4  : 1; // 4
      uint32_t GC     : 1; // 5
      uint32_t STC    : 1; // 6
      uint32_t RSVD7  : 1; // 7
      uint32_t BF     : 1; // 8
      uint32_t AAS    : 1; // 9
      uint32_t XUDF   : 1; // 10
      uint32_t ROVR   : 1; // 11
      uint32_t RSVD12 : 1; // 12
      uint32_t RDR    : 1; // 13
      uint32_t XDR    : 1; // 14
      uint32_t RSVD15 : 1; // 15
    } I2C_WE_bit;
  }; // 0x34-0x35

  // 16 bit compensate offset 
  //uint8_t rsvd36[2];  //0x36-0x37


  /* I2C_DMARXENABLE_SET register bit field */
  union {
    volatile uint16_t I2C_DMARXENABLE_SET;

    volatile struct {
      uint16_t DMARX_ENABLE_SET : 1; // 0
      uint16_t RSVD1            : 15; // 15:1
    } I2C_DMARXENABLE_SET_bit;
  }; // 0x38-0x39

  // 16 bit compensate offset 
  //uint8_t rsvd3a[2];  //0x3A-0x3B


  /* I2C_DMATXENABLE_SET register bit field */
  union {
    volatile uint16_t I2C_DMATXENABLE_SET;

    volatile struct {
      uint16_t DMATX_TRANSMIT_SET : 1; // 0
      uint16_t RSVD1              : 15; // 15:1
    } I2C_DMATXENABLE_SET_bit;
  }; // 0x3C-0x3D

  // 16 bit compensate offset 
  //uint8_t rsvd3e[2];  //0x3E-0x3F


  /* I2C_DMARXENABLE_CLR register bit field */
  union {
    volatile uint16_t I2C_DMARXENABLE_CLR;

    volatile struct {
      uint16_t DMARX_ENABLE_CLEAR : 1; // 0
      uint16_t RSVD1              : 15; // 15:1
    } I2C_DMARXENABLE_CLR_bit;
  }; // 0x40-0x41

  // 16 bit compensate offset 
  //uint8_t rsvd42[2];  //0x42-0x43


  /* I2C_DMATXENABLE_CLR register bit field */
  union {
    volatile uint16_t I2C_DMATXENABLE_CLR;

    volatile struct {
      uint16_t DMATX_ENABLE_CLEAR : 1; // 0
      uint16_t RSVD1              : 15; // 15:1
    } I2C_DMATXENABLE_CLR_bit;
  }; // 0x44-0x45

  // 16 bit compensate offset 
  //uint8_t rsvd46[2];  //0x46-0x47


  /* I2C_DMARXWAKE_EN register bit field */
  union {
    volatile uint16_t I2C_DMARXWAKE_EN;

    volatile struct {
      uint16_t AL     : 1; // 0
      uint16_t NACK   : 1; // 1
      uint16_t ARDY   : 1; // 2
      uint16_t DRDY   : 1; // 3
      uint16_t RSVD4  : 1; // 4
      uint16_t GC     : 1; // 5
      uint16_t STC    : 1; // 6
      uint16_t RSVD7  : 1; // 7
      uint16_t BF     : 1; // 8
      uint16_t AAS    : 1; // 9
      uint16_t XUDF   : 1; // 10
      uint16_t ROVR   : 1; // 11
      uint16_t RSVD12 : 1; // 12
      uint16_t RDR    : 1; // 13
      uint16_t XDR    : 1; // 14
      uint16_t RSVD15 : 1; // 15
    } I2C_DMARXWAKE_EN_bit;
  }; // 0x48-0x49

  // 16 bit compensate offset 
  //uint8_t rsvd4a[2];  //0x4A-0x4B


  /* I2C_DMATXWAKE_EN register bit field */
  union {
    volatile uint16_t I2C_DMATXWAKE_EN;

    volatile struct {
      uint16_t AL     : 1; // 0
      uint16_t NACK   : 1; // 1
      uint16_t ARDY   : 1; // 2
      uint16_t DRDY   : 1; // 3
      uint16_t RSVD4  : 1; // 4
      uint16_t GC     : 1; // 5
      uint16_t STC    : 1; // 6
      uint16_t RSVD7  : 1; // 7
      uint16_t BF     : 1; // 8
      uint16_t AAS    : 1; // 9
      uint16_t XUDF   : 1; // 10
      uint16_t ROVR   : 1; // 11
      uint16_t RSVD12 : 1; // 12
      uint16_t RDR    : 1; // 13
      uint16_t XDR    : 1; // 14
      uint16_t RSVD15 : 15; //15
    } I2C_DMATXWAKE_EN_bit;
  }; // 0x4C-0x4D

  // 16 bit compensate offset 
  //uint8_t rsvd4e[2];  //0x4E-0x4F

  uint8_t rsvd50[64];  //0x50-0x8F

  /* I2C_SYSS register bit field */
  union {
    volatile uint16_t I2C_SYSS;

    volatile struct {
      uint16_t RDONE : 1; // 0
      uint16_t RSVD1 : 15; // 15:1
    } I2C_SYSS_bit;
  }; // 0x90-0x91

  // 16 bit compensate offset 
  //uint8_t rsvd92[2];  //0x92-0x93


  /* I2C_BUF register bit field */
  union {
    volatile uint16_t I2C_BUF;

    volatile struct {
      uint16_t TXTRSH     : 6; // 5:0
      uint16_t TXFIFO_CLR : 1; // 6
      uint16_t XDMA_EN    : 1; // 7
      uint16_t RXTRSH     : 6; // 13:8
      uint16_t RXFIFO_CLR : 1; // 14
      uint16_t RDMA_EN    : 1; // 15
    } I2C_BUF_bit;
  }; // 0x94-0x95

  // 16 bit compensate offset 
  //uint8_t rsvd96[2];  //0x96-0x97


  /* I2C_CNT register bit field */
  union {
    volatile uint16_t I2C_CNT;

    volatile struct {
      uint16_t DCOUNT : 16; // 15:0
    } I2C_CNT_bit;
  }; // 0x98-0x99

  // 16 bit compensate offset 
  //uint8_t rsvd9a[2];  //0x9A-0x9B


  /* I2C_DATA register bit field */
  union {
    volatile uint16_t I2C_DATA;

    volatile struct {
      uint32_t DATA : 8; // 7:0
      uint32_t RSVD8 : 8; // 15:8
    } I2C_DATA_bit;
  }; // 0x9C-0x9D

  // 16 bit compensate offset 
  //uint8_t rsvd9e[2];  //0x9E-0x9F


  uint8_t rsvda0[4];  //0xA0-0xA3


  /* I2C_CON register bit field */
  union {
    volatile uint16_t I2C_CON;

    volatile struct {
      uint16_t STT    : 1; // 0
      uint16_t STP    : 1; // 1
      uint16_t RSVD2  : 2; // 3:2
      uint16_t XOA3   : 1; // 4
      uint16_t XOA2   : 1; // 5
      uint16_t XOA1   : 1; // 6
      uint16_t XOA0   : 1; // 7
      uint16_t XSA    : 1; // 8
      uint16_t TRX    : 1; // 9
      uint16_t MST    : 1; // 10
      uint16_t STB    : 1; // 11
      uint16_t OPMODE : 2; // 13:12
      uint16_t RSVD14 : 1; // 14
      uint16_t I2C_EN : 1; // 15
    } I2C_CON_bit;
  }; // 0xA4-0xA5

  // 16 bit compensate offset 
  //uint8_t rsvda6[2];  //0xA6-0xA7


  /* I2C_OA register bit field */
  union {
    volatile uint16_t I2C_OA;

    volatile struct {
      uint16_t OA     : 10; // 9:0
      uint16_t RSVD10 : 3; // 12:10
      uint16_t MCODE  : 3; // 15:12
    } I2C_OA_bit;
  }; // 0xA8-0xA9

  // 16 bit compensate offset 
  //uint8_t rsvdaa[2];  //0xAA-0xAB


  /* I2C_SA register bit field */
  union {
    volatile uint16_t I2C_SA;

    volatile struct {
      uint16_t SA     : 10; // 9:0
      uint16_t RSVD10 : 6; // 15:10
    } I2C_SA_bit;
};
    // 16 bit compensate offset 
    //uint8_t rsvdae[2];  //0xAE-0xAF


    /* I2C_PSC register bit field */
    union {
      volatile uint16_t I2C_PSC;

      volatile struct {
        uint16_t PSC   : 8; // 7:0
        uint16_t RSVD8 : 8; // 15:8
      } I2C_PSC_bit;
    }; // 0xB0-0xB1

    // 16 bit compensate offset
    //uint8_t rsvdb2[2];  //0xB2-0xB3


    /* I2C_SCLL register bit field */
    union {
      volatile uint16_t I2C_SCLL;

      volatile struct {
        uint16_t SCLL   : 8; // 7:0
        uint16_t HSSCLL : 8; // 15:8
      } I2C_SCLL_bit;
    }; // 0xB4-0xB5

    // 16 bit compensate offset
    //uint8_t rsvdb6[2];  //0xB6-0xB7


    /* I2C_SCLH register bit field */
    union {
      volatile uint16_t I2C_SCLH;

      volatile struct {
        uint32_t SCLH   : 8; // 7:0
        uint32_t HSSCLH : 8; // 15:8
      } I2C_SCLH_bit;
    }; // 0xB8-0xB9

    // 16 bit compensate offset
    //uint8_t rsvdba[2];  //0xBA-0xBB


    /* I2C_SYSTEST register bit field */
    union {
      volatile uint16_t I2C_SYSTEST;

      volatile struct {
        uint16_t SDA_O      : 1; // 0
        uint16_t SDA_I      : 1; // 1
        uint16_t SCL_O      : 1; // 2
        uint16_t SCL_I      : 1; // 3
        uint16_t rsvd4      : 1; // 4
        uint16_t SDA_O_FUNC : 1; // 5
        uint16_t SDA_I_FUNC : 1; // 6
        uint16_t SCL_O_FUNC : 1; // 7
        uint16_t SCL_I_FUNC : 1; // 8
        uint16_t rsvd9      : 2; // 10:9
        uint16_t SSB        : 1; // 11
        uint16_t TMODE      : 2; // 13:12
        uint16_t FREE       : 1; // 14
        uint16_t ST_EN      : 1; // 15
      } I2C_SYSTEST_bit;
    }; // 0xBC-0xBD

    // 16 bit compensate offset
    //uint8_t rsvdbe[2];  //0xBE-0xBF

    /* I2C_BUFSTAT register bit field */
    union {
      volatile uint16_t I2C_BUFSTAT;

      volatile struct {
        uint16_t TXSTAT    : 6; // 5:0
        uint16_t RSVD6     : 2; // 7:6
        uint16_t RXSTAT    : 6; // 13:8
        uint16_t FIFODEPTH : 2; // 15:14
      } I2C_BUFSTAT_bit;
    }; // 0xC0-0xC1

    // 16 bit compensate offset
    //uint8_t rsvdc2[2];  //0xC2-0xC3


    /* I2C_OA1 register bit field */
    union {
      volatile uint16_t I2C_OA1;

      volatile struct {
        uint16_t OA1    : 10; // 9:0
        uint16_t RSVD10 : 6; // 15:10
      } I2C_OA1_bit;
    }; // 0xC4-0xC5

    // 16 bit compensate offset
    //uint8_t rsvdc6[2];  //0xC6-0xC7


    /* I2C_OA2 register bit field */
    union {
      volatile uint16_t I2C_OA2;

      volatile struct {
        uint16_t OA2    : 10; // 9:0
        uint16_t RSVD10 : 6; // 15:10
      } I2C_OA2_bit;
    }; // 0xC8-0xC9

    // 16 bit compensate offset
    //uint8_t rsvdca[2];  //0xCA-0xCB


    /* I2C_OA3 register bit field */
    union {
      volatile uint16_t I2C_OA3;

      volatile struct {
        uint16_t OA3    : 10; // 9:0
        uint16_t RSVD10 : 6; // 15:10
      } I2C_OA3_bit;
    }; // 0xCC-0xCD

    // 16 bit compensate offset
    //uint8_t rsvdce[2];  //0xCE-0xCF


    /* I2C_ACTOA register bit field */
    union {
      volatile uint16_t I2C_ACTOA;

      volatile struct {
        uint16_t OA0_ACT : 1; // 0
        uint16_t OA1_ACT : 1; // 1
        uint16_t OA2_ACT : 1; // 2
        uint16_t OA3_ACT : 1; // 3
        uint16_t RSVD4   : 12; // 15:4
      } I2C_ACTOA_bit;
    }; // 0xD0-0xD1


    // 16 bit compensate offset
    //uint8_t rsvdd2[2];  //0xD2-0xD3


    /* SYS_I2C_SBLOCK register bit field */
    union {
      volatile uint16_t I2C_SBLOCK;

      volatile struct {
        uint16_t OA0_EN : 1; // 0
        uint16_t OA1_EN : 1; // 1
        uint16_t OA2_EN : 1; // 2
        uint16_t OA3_EN : 1; // 3
        uint16_t RSVD4  : 12; // 15:4
      } I2C_SBLOCK_bit;
    }; // 0xD4-0xD5

  } pruI2C;
  // TODO: check that the next statement is correct ! :-)
// here I define CT_I2C1 as a pruI2C, but not sure about the __attribute__ part
  volatile __far pruI2C CT_I2C1 __attribute__((cregister("I2C1", near), peripheral));

#endif /*AM572x_PRU_I2C_H */
