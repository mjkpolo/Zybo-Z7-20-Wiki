# **Using the AD1 PMOD as an Analog-Digital Converter for the Zybo**

## Setup

- The setup for this demo consists of a Vivado Block design along with an imported demo with source code that will modify later.
- Open Vivado and Create a new Project choosing RTL Project
- Next, click on the boards tab and find **Zybo Z7-20**. If the board isn't listed, make sure you have all board files downloaded from Digilent.

## Block Design in Vivado

-Under `IP INTEGRATOR` in the Flow Navigator Window, Create a Block Design.
- Next, press the `+` Button to add an IP from the Diagram Window.
- In the search tab, find and select `ZYNQ7 Processing System`.
- With this block now added to the design, add another IP using the same method, now selecting `PmodAD1_v1_0`.
- Now, Run the Block Automation and Connection Automation Tools (Select All Automation).
- Go to the Board Window on the left, and right-click **JA** and select **Connect Board Component**. 
- Here ensure `Pmod_out` is highlighted, and click OK. You should now see **ja** as the external connection for the AD1 Block in our Diagram.
- After clicking Regenerate Layout, your design should look like this:
- INSERT PICTURE OF DIAGRAM HERE

## Constraints and Assignments

- The next thing to do is add the `Master XDC Files` for the **Z7**. 
- If not downloaded already, head to the `Zybo Z7` page on Digilent's site and under documentation will be a link to download the master files.
- With the master files downloaded, right-click on **constraints** under the `sources` window in Vivado and select `Add Sources`.
- A new window will appear, choose to add a new constraint and click the `Add Files` button where you will add the previously downloaded `Zybo-Z7-Master` file. 

At this stage, the **XDC** file should be viewable under the **Constraints** tab.

- Go inside the master file and replace the comments under the `Pmod Header JA (XADC)` with:
```
set_property -dict { PACKAGE_PIN N15   IOSTANDARD LVCMOS33 } [get_ports { ja_pin1_io }]; #IO_L21P_T3_DQS_AD14P_35 Sch=JA1_R_p		   
set_property -dict { PACKAGE_PIN L14   IOSTANDARD LVCMOS33 } [get_ports { ja_pin2_io }]; #IO_L22P_T3_AD7P_35 Sch=JA2_R_P             
set_property -dict { PACKAGE_PIN K16   IOSTANDARD LVCMOS33 } [get_ports { ja_pin3_io }]; #IO_L24P_T3_AD15P_35 Sch=JA3_R_P            
set_property -dict { PACKAGE_PIN K14   IOSTANDARD LVCMOS33 } [get_ports { ja_pin4_io }]; #IO_L20P_T3_AD6P_35 Sch=JA4_R_P             
set_property -dict { PACKAGE_PIN N16   IOSTANDARD LVCMOS33 } [get_ports { ja_pin7_io }]; #IO_L21N_T3_DQS_AD14N_35 Sch=JA1_R_N        
set_property -dict { PACKAGE_PIN L15   IOSTANDARD LVCMOS33 } [get_ports { ja_pin8_io }]; #IO_L22N_T3_AD7N_35 Sch=JA2_R_N             
set_property -dict { PACKAGE_PIN J16   IOSTANDARD LVCMOS33 } [get_ports { ja_pin9_io }]; #IO_L24N_T3_AD15N_35 Sch=JA3_R_N            
set_property -dict { PACKAGE_PIN J14   IOSTANDARD LVCMOS33 } [get_ports { ja_pin10_io }]; #IO_L20N_T3_AD6N_35 Sch=JA4_R_N   
```
## Creating the Wrapper

-In the sources window, right click on your block design (Should be something like design_1.bd) and slect `Create HDL Wrapper`. Let Vivado manage the wrapper.

- If you receive critical wanrings about **PCW_UIPARAM_DDR_DQS_TO_CLK_DELAY_0 has negative value -0.050**, you can ignore this.

_ It's time to create a Bitstream, in the Flow Navigator under Program and Debug, click `Generate Bitstream` and wait (May take awhile).

## Exporting Design and Launching Vitis

At this point a window should pop up when the Bitstream has finished, click cancel to exit this pop up.

- Under **file**, select `Export`-> `Export Hardware`.
- Be sure to include teh Bitstream when exporting.

With hardware exported into an `XSA`, laucnh `Vitis` under the **Tools** tab. You may have to make a workspace for `Vitis`.
## Vitis Program
With `Vitis` launched, we will now implement the software that will drive what we designed in Vivado.

- Create a new application by doing `File` -> `New` -> `Create New Application`
- Choose the **Create a new platform from hardware (XSA)** tab and browse to find and add `XSA` file that we exported earlier.
- Give a name to your application and click `Next` twice (There is nothing to change in these sections).
- Choose the `Empty Application(C)` Template and **Finish**
- Under the `Explorer` window, find you application and get to the `src` folder inside, right-click and add a new File. Name your file `main.c`.

Add this code to `main.c` and save:

```

/******************************************************************************/
/*                                                                            */
/* main.c -- PmodAD1 Example Project                                          */
/*                                                                            */
/******************************************************************************/
/* Author: Arthur Brown                                                       */
/* Copyright 2017, Digilent Inc.                                              */
/******************************************************************************/
/* Module Description:                                                        */
/*                                                                            */
/* This file contains code for running a demonstration of the PmodAD1 when    */
/* used with the PmodAD1 IP core. This demo initializes the PmodAD1 IP core   */
/* and then polls its sample register, printing the analog voltage last       */
/* sampled by each of the AD1's two channels over UART.                       */
/*                                                                            */
/* Messages printed by this demo can be received by using a serial terminal   */
/* configured with the appropriate Baud rate. 115200 for Zynq systems, and    */
/* whatever the AXI UARTLITE IP is configured with for MicroBlaze systems,    */
/* typically 9600 or 115200 Baud.                                             */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    08/15/2017(ArtVVB):   Created                                           */
/*    02/10/2018(atangzwj): Validated for Vivado 2017.4                       */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include "PmodAD1.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xparameters.h"

PmodAD1 myDevice;
const float ReferenceVoltage = 3.30;

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();

int main() {
   DemoInitialize();
   DemoRun();
   DemoCleanup();
   return 0;
}

void DemoInitialize() {
   EnableCaches();

   AD1_begin(&myDevice, XPAR_PMODAD1_0_AXI_LITE_SAMPLE_BASEADDR);

   // Wait for AD1 to finish powering on
   usleep(1); // 1 us (minimum)
}

void DemoRun() {
   AD1_RawData RawData;
   AD1_PhysicalData PhysicalData;

   while (1) {
      AD1_GetSample(&myDevice, &RawData); // Capture raw samples

      // Convert raw samples into floats scaled to 0 - VDD
      AD1_RawToPhysical(ReferenceVoltage, RawData, &PhysicalData);
// Used for testing validity and correctness, feel free to comment/uncomment
//     xil_printf("Raw Input Data 1: %i;   ", RawData[0]);
//     xil_printf("Raw Input Data 2: %i\r\n", RawData[1]);
//
      printf("Input Data 1: %.02f;   ", PhysicalData[0]);
      printf("Input Data 2: %.02f\r\n", PhysicalData[1]);

      // Do this 5x per second
      usleep(200000);
   }
}

void DemoCleanup() {
   DisableCaches();
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#endif
}

```