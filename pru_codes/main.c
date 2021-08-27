/*
 * main.c
 * Modified by Pierrick Rauby < PierrickRauby - pierrick.rauby@gmail.com >
 * Based on the cloud9 examples:
 * https://github.com/jadonk/cloud9-examples/blob/master/BeagleBone/
 *       AI/pru/blinkInternalLED.pru1_1.c
 * This code is configuring the MMA8451Q to get some data form the 
 *  accelerometer
 * To compile use: make 
 * To deploy use: sh ./run.sh
*/


#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table_1.h"
#include "prugpio.h"
#include <stdio.h>
#include <stdlib.h>            // atoi
#include <string.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>
#include "hw_types.h"
#include "am572x_pru_i2c_driver.h"
#include "am572x_pru_i2c.h" // to remove 

volatile register unsigned int __R30;
volatile register unsigned int __R31;

/* Host-0 Interrupt sets bit 30 in register R31 */
#define HOST_INT            ((uint32_t) 1 << 31)

/* The PRU-ICSS system events used for RPMsg are defined in the Linux 
   device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 * Be sure to change the values in resource_table_0.h too.
 */
#define TO_ARM_HOST            18
#define FROM_ARM_HOST        19
#define CHAN_NAME            "rpmsg-pru"
#define CHAN_DESC            "Channel 30"
#define CHAN_PORT            30
/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK    4
char payload[RPMSG_BUF_SIZE];
struct pru_rpmsg_transport transport;
uint16_t src, dst, len;
volatile uint8_t *status;
int i;
unsigned long sample;
// Points to the two GPIO ports that are used
uint32_t *gpio5 = (uint32_t *)GPIO5;

volatile pruI2C *PRU_I2Cmain=&CT_I2C4;
uint8_t pru_i2c_test_function( uint8_t i2cDevice){
    /* Allow OCP master port access by the PRU so the PRU can read external 
       memories */
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
    // This line is different between AM335x and Am5729 
    CT_INTC.SICR_bit.STATUS_CLR_INDEX = FROM_ARM_HOST;
    /* Make sure the Linux drivers are ready for RPMsg communication */
    status = &resourceTable.rpmsg_vdev.status;
    while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));
    /* Initialize the RPMsg transport structure */
    pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0,
        &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);
    /* Create the RPMsg channel between the PRU and ARM user space using the 
       transport structure. */
    while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, 
          CHAN_PORT) != PRU_RPMSG_SUCCESS);
    while (1) {
      /* Check bit 30 of register R31 to see if the ARM has kicked us */
    uint32_t *gpio5 = (uint32_t *)GPIO5;
    gpio5[GPIO_SETDATAOUT]   = USR1;  // Turn the USR1 LED on
    /*__delay_cycles(1000000000/5);   // Wait 1/2 second*/
    /*gpio5[GPIO_CLEARDATAOUT] = USR1;  // Off*/
    /*__delay_cycles(1000000000/5);   // Wait 1/2 second*/
      if (__R31 & HOST_INT) {
        /* Clear the event status */
        CT_INTC.SICR_bit.STATUS_CLR_INDEX = FROM_ARM_HOST;
        while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) 
            == PRU_RPMSG_SUCCESS) {
          uint8_t address=0x1F;
          uint16_t reg=0x13;
          uint16_t result[16];
          /* 1st do a reset of the I2C bus*/
          pru_i2c_driver_software_reset(1);
          /* Initialize the i2c bus 1 for a 2 byte transaction*/
          /* Receive the data from the sensor register specified above*/
          // Read multiple bytes (Not working yet!)
          pru_i2c_driver_receive_byte(address,
              reg,0,result);
          sample=(long)result[0];
          /*sample=returned_value;*/
          memcpy(payload, "\0\0\0\0\0\0\0\0\0\0\0", 11);
          ltoa((long)sample, payload);
          len = strlen(payload) + 1;
          /*memcpy(payload, "ifsdfsdf", 11);*/
          /* send data to user space with RPmsg */
          pru_rpmsg_send(&transport, dst, src, payload, 11);
    }
      }
    }
}
void main(void) {
  pru_i2c_test_function(1);
}

// Turns off triggers
#pragma DATA_SECTION(init_pins, ".init_pins")
#pragma RETAIN(init_pins)
const char init_pins[] =  
  "/sys/class/leds/beaglebone:green:usr1/trigger\0none\0" \
  "/sys/class/leds/beaglebone:green:usr2/trigger\0none\0" \
  "\0\0";
