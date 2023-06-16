## Introduction

To complete this tutorial, I first recommend going over [the Linux mutex guide](Linux Kernel Module for Hardware Mutex) which will help you understand how kernel modules work.

In this tutorial, we will design a custom device which performs simple [Q number](https://en.wikipedia.org/wiki/Q_(number_format)#Math_operations) operations in verilog, use it in Vitis, and use it in Petalinux.

## Creating a custom peripheral

Xilinx auto-generates [questionable AXI Slave Code](https://support.xilinx.com/s/question/0D52E00006hpXPLSA2/custom-axilite-slave-ip-causing-microblaze-to-stall?language=en_US) which we will slightly modify to delay memory read requests when pipe-lining math operations.

To generate a brand new AXI device, open up Vivado, and go to `Tools -> Create and Package new IP...` and select `Create a new AXI4 peripheral`. We will leave interrupt support off, and use 4 slave registers, with all other options defaulted. 

To edit this IP right away, on the final menu screen select Edit IP. Alternatively, save the IP to your new IP repository folder and open it with your favorite text editor.

## Modifying the AXI-Lite Slave interface

The are a couple things we must do to make a useful slave interface. The first thing is exposing the slave registers that we would like to use to the device we would like to connect. For our design, we will need access to the first 3 slave registers.

To expose these registers, go to `myip_v1_0_S00_AXI.v` line 107 where the slave registers are declared. Simply move lines 107-109 to the top of your module definition and declare the registers as outputs.
Additionally, we will add a couple more signals

- expose the `slv_reg_wren` signal just like you did the `slv_regX`s, so that our device knows when a new transaction should take place.
- add a `done` signal as an input wire, so that we can stall AXI read requests until the device has finished processing the last valid data, given valid data is written when `slv_reg_wren` is high.
- add an `out` signal so that our device's output can be memory mapped to the AXI interface

```verilog
	// Users to add ports here
output reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg0,
output reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg1,
output reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg2,
input wire [C_S_AXI_DATA_WIDTH-1:0]	out,
output wire				slv_reg_wren,
input wire				done,
	// User ports ends
```

Next, we need to add these port connections to `myip_v1_0.v` to the `myip_v1_0_S00_AXI` module in your design like so:

```verilog
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg0;
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg1;
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg2;
wire done;
wire slv_reg_wren;
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
	.slv_reg2(slv_reg2),
	.done(done),
        .slv_reg_wren(slv_reg_wren)
);
```
Next we need to manipulate the `ARREADY` (Read Address Ready) so that a read request is not permitted until `done` is high. To do this, open `myip_v1_0_S00_AXI.v` and find line 321. Change the line like so:
```verilog
	if (~axi_arready && S_AXI_ARVALID && done)
```

Now that we have prepped the interface for our slave device, it's time to implement the device itself. For clarification on why certain values are selected, feel free to checkout the [test benches](https://gitlab.ssec.wisc.edu/nextgenshis/ip_repo/-/tree/7ecf3d40a2f17755066bba3afc805959aaadd06d/qnumbers_1_0/bench) which show our assumptions about the device.

## Adding Custom Sources

Our custom device and its sources are located [here](https://gitlab.ssec.wisc.edu/nextgenshis/ip_repo/-/tree/d8388cf8eef4b2454a21fe5c050455e68f28dc91/qnumbers_1_0/src). Clone or zip this repository so you can add the sources to your device. (You don't need ad1_spi.v or adc_model.v since we're not using them for this design)

Once this folder is cloned, go to the `+` symbol in your sources and add the files. They will automatically be copied to the `src` folder of your custom device. Now open `myip_v1_0.v` back up, and add your device called `newwrapper` to the bottom of the file:

```verilog
// Add user logic here
newwrapper #(.CALC_WIDTH(64), .OP_WIDTH(32)) QMATH(
  .clk(s00_axi_aclk),
  .rst(~s00_axi_aresetn),
  .slv_reg0(slv_reg0),
  .slv_reg1(slv_reg1),
  .slv_reg2(slv_reg2),
  .slv_reg_wren(slv_reg_wren),
  .test_out(out),
  .done(done)
);
// User logic ends
endmodule
```
## Finish Packaging IP

Now that we have our device implemented it, it's time to save the IP and import it into a design with a Zynq or Microblaze! 

To save your IP, find the tab labeled `Package IP - myip` and click every box which does not have a green check mark, and click the blue link to take the default action until the check marks are all green.

The last error you will have is `FREQ_HZ` not being defined. To fix this, you can go to `Ports and Interfaces`, right click and select `Edit Interface... -> Parameters -> Requires User Setting -> FREQ_HZ`, select the `->` arrow to add it, and change the value to `50MHZ` which is what we will most likely be using.

## Block Design

Now we need to add our IP to our block design, so close the temporary IP Packaging project, and go back to your Vivado project. Note the location where `myip_v1` was saved.

To add our custom IP to the block design, we must first add the IP location. Go to `Tools -> Settings... -> IP -> Repository` and make sure the folder containing your design is one of the repository sources.

Open or create a new block design, and place just a Zynq Processing System, and myip_v1. Select all the default options for Block and Connection automation, and you will have the following block design created:

![image](uploads/bff490c4d31781193989f7cc341bf27e/image.png)

Generate the bitstream, export the hardware, and you're finished with the hard part!