/*
 * simple_ping_pong_user_space.pru1_1.c
 * Modified by Pierrick Rauby < PierrickRauby - pierrick.rauby@gmail.com >
 * Based on the examples distributed by TI
 * Cedistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of Texas Instruments Incorporated nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 * To compile use: make 
 * To deploy use: sh ./run.sh
*/



#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>

#define MAX_BUFFER_SIZE        512
char readBuf[MAX_BUFFER_SIZE];

#define DEVICE_NAME        "/dev/rpmsg_pru30"

int main(void)
{
    struct pollfd pollfds[1];
    /*int i;*/
    long result = 0;

    /* Open the rpmsg_pru character device file */
    pollfds[0].fd = open(DEVICE_NAME, O_RDWR);

    /*
     * If the RPMsg channel doesn't exist yet the character device
     * won't either.
     * Make sure the PRU firmware is loaded and that the rpmsg_pru
     * module is inserted.
     */
    if (pollfds[0].fd < 0) {
        printf("Failed to open %s\n", DEVICE_NAME);
        return -1;
    }

    /* The RPMsg channel exists and the character device is opened */
    printf("Opened %s, sending address to read to PRU\n", DEVICE_NAME);
    /* In this example only the adress to read with the PRU is sent*/
    result = write(pollfds[0].fd, "Place Add", 10);
    if (result > 0)
      printf("Waiting for PRU answer through character device %s\n", DEVICE_NAME);

    /* Poll until we receive a message from the PRU and then print it */
    result = read(pollfds[0].fd, readBuf, 11);
    if (result > 0)
      printf("Answer received from PRU:\n\n %s\n\n", readBuf);


    /* Close the rpmsg_pru character device file */
    close(pollfds[0].fd);

    return 0;
}
