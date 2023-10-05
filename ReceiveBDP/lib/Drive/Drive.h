#ifndef Drive_h
#define Drive_h
#include "Arduino.h"

// Classe Driver
class Drive{
private:
    int IN1, IN2, PWM;

public:
    Drive(int IN1, int IN2, int PWM);
    void move(int pwm);
};

#endif