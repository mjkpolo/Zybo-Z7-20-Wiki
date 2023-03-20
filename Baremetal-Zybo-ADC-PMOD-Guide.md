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

