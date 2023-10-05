#include "Arduino.h"
#include "Drive.h"

Drive::Drive(int IN1pin, int IN2pin, int PWMpin){
  IN1 = IN1pin;
  IN2 = IN2pin;
  PWM = PWMpin;
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(PWM, OUTPUT);
}

void Drive::move(int pwm){
    if(pwm>0){
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    }
    else if(pwm<0){
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
    }
    else{
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
    }
    analogWrite(PWM, abs(pwm));
}

