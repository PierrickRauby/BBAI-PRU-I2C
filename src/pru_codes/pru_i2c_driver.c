/*
 * pru_i2c_driver.pru1_1.c
 * Created February  2021 by Pierrick Rauby
 * Modified by Pierrick Rauby < PierrickRauby - pierrick.rauby@gmail.com > 
 * An attempt to create an i2c  driver for Beaglebone AI (TI-AM5729 chip), 
 *  initially implemented for i2c1 of the Beaglebone AI as it is connected to 
 *  the on board ADC 
 * To compile use: make 
 * To deploy use: sh ./run.sh
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>            // atoi
#include <string.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>
#include "resource_table_1.h"
#include "init_pins_empty.h"
#include "prugpio.h"
#include "prui2c.h"  // to check 
#include "hw_types.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Host-0 Interrupt sets bit 31 in register R30 */
#define HOST_INT            ((uint32_t) 1 << 31)

/* The PRU-ICSS system events used for RPMsg are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 * Be sure to change the values in resource_table_0.h too.
 */
#define TO_ARM_HOST            18
#define FROM_ARM_HOST        19

// To blink the LED 
//Points to the two GPIO ports that are used
uint32_t *gpio5 = (uint32_t *)GPIO5;

/*
 * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
 */
#define CHAN_NAME            "rpmsg-pru"
#define CHAN_DESC            "Channel 30"
#define CHAN_PORT            30

/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK    4

char payload[RPMSG_BUF_SIZE];



void main(void) {

    struct pru_rpmsg_transport transport;
    uint16_t src, dst, len;
    volatile uint8_t *status;
    int i;
    unsigned int sample;

    /* Allow OCP master port access by the PRU so the PRU can read external memories */
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    // This line is different between AM335x and Am5729 
    CT_INTC.SICR_bit.STATUS_CLR_INDEX = FROM_ARM_HOST;

    /* Make sure the Linux drivers are ready for RPMsg communication */
    status = &resourceTable.rpmsg_vdev.status;


    while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

    /* Initialize the RPMsg transport structure */
    pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

    // I checked with LED, I go up to here at least 

    /* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
    // This seems to work according to dmesg 
    while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);

    while (1) {
      /* Check bit 30 of register R31 to see if the ARM has kicked us */
      if (__R31 & HOST_INT) {
        /* Clear the event status */
    CT_INTC.SICR_bit.STATUS_CLR_INDEX = FROM_ARM_HOST;
    while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS) {

        for(i=0;i<5;++i){
          gpio5[GPIO_SETDATAOUT]   = USR1;  // Turn the USR1 LED on
          __delay_cycles(100000000/5);   // Wait 1/2 second
          gpio5[GPIO_CLEARDATAOUT] = USR1;  // Off
          __delay_cycles(100000000/5); 
        }

          gpio5[GPIO_SETDATAOUT]   = USR1;  // Turn the USR1 LED on
/* For the moment I will hard code the adress:*/
/*  GPIO5 is at address 0x4805B000,
    and the Offset for the DATAOUT register is 13C 
    So I want to read Address 0x4805B13C (Hopefully
*/

        // I read the value of the register into an int 
        sample=HWREG(0x4805B13C);
          // Je place de la merde dans ma payload, je pense que je peux aussi faire un memclear
        memcpy(payload, "\0\0\0\0\0\0\0\0\0\0", 10);
        // je copie la value en int vers ma payload
        ltoa((long)sample, payload);
        //len = strlen(payload) + 1;
        pru_rpmsg_send(&transport, dst, src, payload, 10);

// La suite c'est de la merde que j'ai fait :) 

      /*char *p = (char *)0x4805B13C;*/

        /* echo the message back to the same address from which we just received */
        /* I think the next line is useless */
        /*strcpy((char *)payload, "hello cortex-a8!");*/
        /*pru_rpmsg_send(&transport, dst, src, &p, 16);*/
    }
      }
    }
}

