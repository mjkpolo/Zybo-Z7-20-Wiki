#ifndef _PID_H_
#define _PID_H_

// Returns the manipulated variable given a setpoint and current process value
double calculate( double setpoint, double pv, double pFactor, double iFactor);

#endif
