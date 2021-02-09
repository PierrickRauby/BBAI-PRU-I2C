// Definies the address of the I2C1 bus and PRU address
// Information taken from SPRUHZ6L, pages indicated at
// the beginning of each section 


#if defined(CHIP) && defined(CHIP_IS_am57xx)
#warning "Found AI" 

// These address are from AM5729

// Gate to enable Functionnal and Interface Clocks (SPRUHZ6L p1079)
#define CM_L4_PER_I2C1_CLCKCTRL 0x4A0097A0


// I2C Based address (SPRUHZ6L p5756)
#define I2C1 0x48070000
#define I2C2 0x48072000
#define I2C3 0x48060000
#define I2C4 0x4807A000
#define I2C5 0x4807C000

// Offset tables for I2C registers (SPRUHZ6L p5756 and p5757) 

// I2C_REVNB_LO Offset
#define I2C_REVNB_LO 0x0
// I2C_REVNB_HI Offset
#define I2C_REVNB_HI 0x4
// I2C_SYSC Offset
#define I2C_SYSC 0x10
// I2C_EOI Offset
#define I2C_EIO 0x20
// I2C_IRQSTATUS_RAW Offset
#define I2C_IRQSTATUS_RAW 0x24
// I2C_IRQSTATUS Offset
#define I2C_IRQSTATUS 0x28
// I2C_IRQENABLE_SET Offset
#define I2C_IRQENABLE_SET 0x2C
// I2C_IRQENABLE_CLR Offset
#define I2C_IRQENABLE_CLR 0x30
// I2C_WE Offset
#define I2C_WE 0x34
// I2C_DMARXENABLE_SET Offset
#define I2C_DMARXENABLE_SET 0x38
// I2C_DMATXENABLE_SET Offset
#define I2C_DMATXENABLE_SET 0x3C
// I2C_DMARXENABLE_CLR Offset
#define I2C_DMARXENABLE_CLR 0x40
// I2C_DMATXENABLE_CLR Offset
#define I2C_DMATXENABLE_CLR 0x44
// I2C_DMARXWAKE_EN Offset
#define I2C_DMARXWAKE_EN 0x48
// I2C_DMATXWAKE_EN Offset
#define I2C_DMATXWAKE_EN 0x4C
// I2C_SYSS Offset
#define I2C_SYSS 0x90
// I2C_BUF Offset
#define I2C_BUF 0x94
// I2C_CNT Offset
#define I2C_CNT 0x98
// I2C_DATA Offset
#define I2C_DATA 0x9C
// I2C_CON Offset
#define I2C_CON 0xA4
// I2C_OA Offset
#define I2C_OA 0xA8
// I2C_SA Offset
#define I2C_SA 0xAC
// I2C_PSC Offset
#define I2C_PSC 0xB0
// I2C_SCLL Offset
#define I2C_SCLL 0xB4
// I2C_SCLH Offset
#define I2C_SCLH 0xB8
// I2C_SYSTEST Offset
#define I2C_SYSTEST 0xBC
// I2C_BUFSTAT Offset
#define I2C_BUFSTAT 0xC0
// I2C_OA1 Offset
#define I2C_OA1 0xC4
// I2C_OA2 Offset
#define I2C_OA2 0xC8
// I2C_OA3 Offset
#define I2C_OA3 0xCC
// I2C_ACTOA Offset
#define I2C_ACTOA 0xD0
// I2C_SBLOCK Offset
#define I2C_SBLOCK 0xD4
