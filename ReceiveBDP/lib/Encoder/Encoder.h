#ifndef Encoder_h
#define Encoder_h
#include "Arduino.h"

// Classe Encoder
class Encoder{
private:
    long previousMillis;
    int rpm;
    int encoderOutput;
public:
    Encoder(int encoder_output);
    void calculate_rpm(int encoder_value, long current_time);
    int get_rpm();
};


#endif