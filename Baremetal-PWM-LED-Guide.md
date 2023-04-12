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