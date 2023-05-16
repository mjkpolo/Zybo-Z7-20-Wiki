#include <stdio.h>
#include <stdbool.h>
#include <sleep.h>
#include <math.h>
#include "PWM.h"
#include "xparameters.h"
#include "PmodAD1.h"
#include "xil_io.h"
#include "xil_types.h"
#include "pid.h"

PmodAD1 myDevice; // AD1.1
PmodAD1 myDevice1; // AD1.2
AD1_RawData RawData; // Raw Bits from AD1.1
AD1_PhysicalData PhysicalData; // Physical from AD1.1
AD1_RawData RawData1; // Raw Bits from AD1.2
AD1_PhysicalData PhysicalData1; // Physical Voltage from AD1.2
u32 addrPWM = 0x43C20000; // Base address of PWM for OD1

double current = 0; // Will be given during test
double a = 1.02263735E-3; // Given Therm Coeff
double b = 2.40323378E-4; // Given Therm Coeff
double c = 1.51856209E-7; // Given Therm Coeff
const float ReferenceVoltage = 3.30; // Ref Voltage for AD1
const double setPoint = 313; // 313 Kelvins
float array[3]; // [0] = AD1.1 A0 = Kp, [1] = AD1.1 A1 = Ki, [2] = AD1.2 A0 = Thermistor being measured

void getAD1Data(float* array);
void AD1Initialize();

int main(){
	// Init PWM and AD1
	PWM_Enable(addrPWM);
	PWM_Set_Period(addrPWM, 1000);
	AD1Initialize();

	// Continuously Sample Voltage
	while(1){
		// One AD1 is for the thermistor Voltage we are measuring, One is for setting the porportional Gain, one for setting the integral gain.
		getAD1Data(array);

		// Get R with Steinhart-Hart Eqn
		double r = array[2] / current;
		double log_r = log(r);
		double log_r3 = log_r * log_r * log_r;
		double temp = 1.0 / (a + (b * log_r) + (c * log_r3));

		// Calc PWM Duty with PID based off of Current Read-in Temperature

		// Array 0 and 1 are bit proportion that will be multiplied by Kp and Ki max vals, use that product for PID calculation
		// Example for array[0]/[1] Read-in 2458, 2458/4096 = 0.6
		double pidValTemp = calculate(setPoint, temp, (double)array[0], (double)array[1]);

		// Use pidVal to set Duty Cycle from PWM on OD1
		PWM_Set_Duty(addrPWM, (u32)pidValTemp, 0);
		PWM_Set_Duty(addrPWM, (u32)pidValTemp, 1);
		PWM_Set_Duty(addrPWM, (u32)pidValTemp, 2);
		PWM_Set_Duty(addrPWM, (u32)pidValTemp, 3);

		// Display current read-in temperature
		printf("%lf", temp);
	}

	return 0;
}

void AD1Initialize(){
	// Init both devices
	AD1_begin(&myDevice, XPAR_PMODAD1_0_AXI_LITE_SAMPLE_BASEADDR);
	AD1_begin(&myDevice1, XPAR_PMODAD1_1_AXI_LITE_SAMPLE_BASEADDR);

	// Wait for AD1 to finish powering on
	usleep(1);
}

void getAD1Data(float* array){
	  // Capture raw samples
	  AD1_GetSample(&myDevice, &RawData);
      AD1_GetSample(&myDevice1, &RawData1);

      // Convert raw samples into floats scaled to 0 - VDD
	  AD1_RawToPhysical(ReferenceVoltage, RawData, &PhysicalData);
	  AD1_RawToPhysical(ReferenceVoltage, RawData1, &PhysicalData1);

	  /*
	  xil_printf("Raw Input Data 1: %i;   ", RawData[0]);
	  xil_printf("Raw Input Data 2: %i\r\n", RawData[1]);
	  printf("Input Data 1: %.02f;   ", PhysicalData[0]);
	  printf("Input Data 2: %.02f\r\n", PhysicalData[1]);

	  Do this 5x per second
	  usleep(200000);
	  */

	  // Add values from the AD1's respectively
	  array[0] = RawData[0] / (4095); // For P, get % factor of Kp max
	  array[1] = RawData[1] / (4095); // For I, get % factor of Ki max
	  array[2] = PhysicalData1[0]; // For Voltage

}


