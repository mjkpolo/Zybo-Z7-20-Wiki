# **Creating a PWM LED Dimmer on Zybo**

## Setup

- Create a new project in Vivado choosing RTL Project.
- Find and select **Zybo Z7-20** for the board design. If the board isn't listed, make sure you have all board files downloaded from Digilent. 


## Block Design in Vivado

- Under `IP INTEGRATOR` in the Flow Navigator Window, Create a Block Design. 
- Next, press the `+` Button to add an IP from the Diagram Window.
- In the search tab, find and select, `Zynq7 Processing System`. 
- With this block now added to the design, we will add the other IP using the method as before, this time selecting `PWM_v2.0`. If it is not showing up, make sure you've imported all the Pmod IP's from Digilent's Repo. 
- Select the `PWM` Block and set the **Number of PWMs** to 8.

## Constraints and Assignments

- Looking still at the block design, right-click the PWM output and `Make External`. 
- You should now see **pwm_0[7:0]** as the output pins on the IP block. 
- Make sure to **Run Block Automation** and **Run Connection Automation** as well.
- Now, go into the constraints and set the JE constraints to the code below:
```
set_property -dict { PACKAGE_PIN V12   IOSTANDARD LVCMOS33 } [get_ports { pwm_0[0] }]; #IO_L4P_T0_34 Sch=je[1]						 
set_property -dict { PACKAGE_PIN W16   IOSTANDARD LVCMOS33 } [get_ports { pwm_0[1] }]; #IO_L18N_T2_34 Sch=je[2]                     
set_property -dict { PACKAGE_PIN J15   IOSTANDARD LVCMOS33 } [get_ports { pwm_0[2] }]; #IO_25_35 Sch=je[3]                          
set_property -dict { PACKAGE_PIN H15   IOSTANDARD LVCMOS33 } [get_ports { pwm_0[3] }]; #IO_L19P_T3_35 Sch=je[4]                     
set_property -dict { PACKAGE_PIN V13   IOSTANDARD LVCMOS33 } [get_ports { pwm_0[4] }]; #IO_L3N_T0_DQS_34 Sch=je[7]                  
set_property -dict { PACKAGE_PIN U17   IOSTANDARD LVCMOS33 } [get_ports { pwm_0[5] }]; #IO_L9N_T1_DQS_34 Sch=je[8]                  
set_property -dict { PACKAGE_PIN T17   IOSTANDARD LVCMOS33 } [get_ports { pwm_0[6] }]; #IO_L20P_T3_34 Sch=je[9]                     
set_property -dict { PACKAGE_PIN Y17   IOSTANDARD LVCMOS33 } [get_ports { pwm_0[7] }]; #IO_L7N_T1_34 Sch=je[10]  

```