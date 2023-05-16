static int integral;
static int perror;

#define KP 1
#define KI 10
#define KD 0


void duty(int setpoint, int current, int dt) {
	int error = setpoint - current;
	int Pout = KP*error;
	integral += dt*error;
	int Iout = KI*integral;
	int Dout = KD*(error - perror)/dt;
	int output = Pout + Iout + Dout;
	perror = error;
}

int mainPID(){
	integral = 0;
	for(;;) {
		//duty(int setpoint, int current);
	}
	return 0;
}
