## Introduction

To complete this tutorial, I first recommend going over [the Linux mutex guide](Linux Kernel Module for Hardware Mutex) which will help you understand how kernel modules work.

In this tutorial, we will design a custom device which performs simple [Q number](https://en.wikipedia.org/wiki/Q_(number_format)#Math_operations) operations in verilog, use it in Vitis, and use it in Petalinux.

## Creating a custom peripheral

Xilinx auto-generates [questionable AXI Slave Code](https://support.xilinx.com/s/question/0D52E00006hpXPLSA2/custom-axilite-slave-ip-causing-microblaze-to-stall?language=en_US) which we will slightly modify to delay memory read requests when pipe-lining math operations.

To generate a brand new AXI device, open up Vivado, and go to `Tools -> Create and Package new IP...` and select `Create a new AXI4 peripheral`. We will leave interrupt support off, and use 4 slave registers, with all other options defaulted. 

To edit this IP right away, on the final menu screen select Edit IP. Alternatively, save the IP to your new IP repository folder and open it with your favorite text editor.

## Modifying the AXI-Lite Slave interface

The are a couple things we must do to make a useful slave interface. The first thing is exposing the slave registers that we would like to use to the device we would like to connect. For our design, we will need access to the first 3 slave registers.

To expose these registers, go to `myip_v1_0_S00_AXI.v` line 107 where the slave registers are declared. Simply move lines 107-109 to the top of your module definition and declare the registers as outputs like so:

```verilog
module myip_v1_0_S00_AXI #
(
	// Users to add parameters here

	// User parameters ends
	// Do not modify the parameters beyond this line

	// Width of S_AXI data bus
	parameter integer C_S_AXI_DATA_WIDTH	= 32,
	// Width of S_AXI address bus
	parameter integer C_S_AXI_ADDR_WIDTH	= 4
)
(
	// Users to add ports here
    output reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg0,
    output reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg1,
    output reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg2,
	// User ports ends
...
```
Next, we need to add these port connections to `myip_v1_0.v` to the `myip_v1_0_S00_AXI` module in your design like so:

```verilog
reg [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg0;
reg [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg0;
reg [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg0;
// Instantiation of Axi Bus Interface S00_AXI
myip_v1_0_S00_AXI # ( 
	.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
	.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
) myip_v1_0_S00_AXI_inst (
	.S_AXI_ACLK(s00_axi_aclk),
...
	.S_AXI_RREADY(s00_axi_rready),
	.slv_reg0(slv_reg0),
	.slv_reg1(slv_reg1),
	.slv_reg2(slv_reg02)
);
```
Now that we have prepped the interface for our slave device, it's time to implement the device itself, and manipulate the ARREADY signal in the AXI interface such that a new memory request is not accepted until the device is finished. (This approach limits throughput, but this is not a problem for our use case)