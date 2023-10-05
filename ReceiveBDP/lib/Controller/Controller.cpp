#include "Controller.h"
#include "Arduino.h"

Controller::Controller(float Kp, float Kd, float Ki)
{
    kp = Kp;
    kd = Kd;
    ki = Ki;
}

void Controller::calculate_signal_control(int target, int current_measure, long current_time)
{
    rpm = current_measure;
    float e = target - current_measure;
    float deltaT = ((float) (current_time - prevT))/( 1.0e3 );
    prevT = current_time;

    // derivative
    dedt = (e-eprev)/(deltaT);

    // integral
    eintegral = eintegral + e*deltaT;

    // control signal
    u = kp*e + kd*dedt + ki*eintegral;

    eprev = e;

}

float Controller::get_pwm(){
    pwr += u;
    if (pwr >255){pwr = 255;}
    else if(pwr<-255){pwr = -255;}
    return pwr;
}