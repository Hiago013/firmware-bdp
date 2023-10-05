#ifndef Controller_h
#define Controller_h
#include "Arduino.h"

// Classe Controlador

class Controller
{
private:
    float kp;
    float kd;
    float ki;
    long prevT;
    float eprev = 0;
    float eintegral = 0;
    float u = 0;
    float dedt = 0;
    int rpm;
    int pwr = 0;


public:
    Controller(float Kp, float Kd, float Ki);
    void calculate_signal_control(int target, int current_measure, long current_time);
    float get_pwm();

};

#endif
