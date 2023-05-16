//#include <iostream>
//#include <cmath>
#include "pid.h"

static double _dt = 1/100; // TODO what to do for dt? Constant? Timer?
static double _max = 1;
static double  _min = 0;
static double  _Kp; // TODO These are the max vals
static double  _Kd;
static double  _Ki;
static double  _pre_error = 0;
static double  _integral = 0;


double calculate( double setpoint, double pv, double pFactor, double iFactor){

    // Calculate error
    double error = setpoint - pv;

    // Proportional term
    double Pout = _Kp * pFactor * error; // TODO take Kp times the fraction of pFactor from AD1

    // Integral term
    _integral += error * _dt;
    double Iout = _Ki * iFactor * _integral; // TODO take Ki times the fraction of iFactor from AD1

    // Derivative term
    double derivative = (error - _pre_error) / _dt;
    double Dout = _Kd * derivative;

    // Calculate total output
    double output = Pout + Iout + Dout;

    // Restrict to max/min
    if( output > _max )
        output = _max;
    else if( output < _min )
        output = _min;

    // Save error to previous error
    _pre_error = error;

    return output;
}
