#include "Encoder.h"
#include "Arduino.h"

Encoder::Encoder(int encoder_output)
{
   encoderOutput = encoder_output;
}

void Encoder::calculate_rpm(int encoder_value, long current_time){
    rpm = (float)((encoder_value * 1000 / (current_time - previousMillis)) * 60 / encoderOutput);
    previousMillis = current_time;
}

int Encoder::get_rpm(){
    return rpm;
}



