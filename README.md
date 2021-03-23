# BBAI-PRU-I2C [Work in Progress]
PRU I2C driver for BeagleBone AI. This work presents some simple fonction to 
control High Speed I2C controller of the AM5729 of the BeagleBone AI. 
__ This has been developped for the BBAI TIDL image: 4.14.108-ti-r131 __ There 
are changes in newer images: the call to remoteproc are done differently, the 
PRUs names are also different. However, at the time 4.14.108-ti-r131 is 
what you get if you use the recommanded images from the BeagleBone Foundation 
[website](https://beagleboard.org/latest-images), so I have been using this 
image.

## How to use the codes?

### A simple transaction 

The following describes how to get a simple I2C transaction between the
Beaglebone AI I2C3 (SCL on P9.19, SDA on P9.20) and an 
[MMA8451 accelerometer]
(https://learn.adafruit.com/adafruit-mma8451-accelerometer-breakout) address 
0x1D in the I2C bus. It will enable the Portrait/Landscape detection and set the
debounce counter mode selection to 0 by writing 0x40 in the PL_CFG register 
(address 0x11). Then it will read back the value of this register.

Clone the repository:
```
cd ~ 
git clone https://github.com/PierrickRauby/BBAI-PRU-I2C.git
```

Then you will want to change the am57xx_pru.cmd linker file located under:
`/var/lib/cloud9/common`. Comment:
```
I2C3            : org = 0x48060000 len = 0x00000300    CREGISTER=5
```
And replace by this line instead:
```
I2C1      : org = 0x4807A000 len = 0x00000300  CREGISTER=5
```
This will let the pru-cgt compile understand the hardware register described by
the library.

Once you have change the linker file you can go back to the project's folder:
```
cd ~ 
cd BBAI-PRU-I2C
```
Then compile and run the pru codes by doing:
```
cd pru_codes
make 
```

The make file will do everything for you, including booting the pru. The last
step is to start the user space execution, from the project's folder:
```
cd user_space
make
./user_space
```
The output is the value written in the Portrait Landscape register. 

## Structure of the repository? 

The project is composed of different folders:
- [pru_codes](): contains the `main.c` that call the I2C function and setups the
  BUS depending on the request send from the user space via rpmsg, a `Makefile` 
  to compile the PRU codes, stop the PRU1, place the firmware and start the PRU.
  The folder also includes the driver file `am572x_pru_i2c_driver.pru1_1.c` with 
  the I2C fonctions. Inside pru_codes the  `include` folder contains the struct 
  that describes the HSI2C registers. 
- [user_space](): present an example of user space code using rpmsg to tell
  the PRU to trigger an I2C transaction. The result of the transaction is then 
  sent to the user space with rpmsg.




