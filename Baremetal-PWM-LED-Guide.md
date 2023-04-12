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

- The next thing to do is add the `Master XDC Files` for the **Z7**. 
- If not downloaded already, head to the `Zybo Z7` page on Digilent's site and under documentation will be a link to download the master files.
- With the master files downloaded, right-click on **constraints** under the `sources` window in Vivado and select `Add Sources`.
- A new window will appear, choose to add a new constraint and click the `Add Files` button where you will add the previously downloaded `Zybo-Z7-Master` file. 

At this stage, the **XDC** file should be viewable under the **Constraints** tab.
- Now, go into the constraints and set the `Pmod Header JE` to the code below:
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
int main(){
	PWM_Enable(0x43C00000);

	PWM_Set_Period(0x43C00000, 1000);
	int i = 0;
	bool top = false;
	bool bottom = false;
	while(1){
		if(i > 1024){
			top = true;
			bottom = false;
		}
		if(i < 0 || i == 0){
			bottom = true;
			top = false;
		}
		if(top){
			i = i - 100;
		}
		if(bottom){
			i = i + 100;
		}

		PWM_Set_Duty(0x43C00000, i, 0);
		PWM_Set_Duty(0x43C00000, i, 1);
		PWM_Set_Duty(0x43C00000, i, 2);
		PWM_Set_Duty(0x43C00000, i, 3);
		usleep(80000);


	}
	return 0;
}

```
- This code will be communicating to the onboard PWM drivers that come with the Pmod, and will dim up and dim down the led's hooked up to the Pmod. 

### Attached Pictures

- Here's some attaches of the demo:
- 