#include <Arduino.h>

#include "Drive.h"
#include "Encoder.h"
#include "Controller.h"

#include <esp_now.h>
#include <WiFi.h>

#define AIN1  2
#define AIN2  4
#define PWMA 13
#define BIN1 16
#define BIN2 17
#define PWMB  5 // 19 (Valor Sacola)

#define ENCA1 23
#define ENCA2 22
#define ENCB1 26
#define ENCB2 25 // 27 (Valor Sacola)

#define TIME_TURN_OFF 1000 // Tempo de espera para o robô parar
// 48:E7:29:9A:2D:A0
// 08:D1:F9:99:E5:C

int ENCODER_OUTPUT = 187;
double kp = .8, ki = 0.008, kd = 0.1;

Drive motor1(AIN1, AIN2, PWMA);
Drive motor2(BIN1, BIN2, PWMB);
Encoder enc1(ENCODER_OUTPUT);
Encoder enc2(ENCODER_OUTPUT);

Controller controller1(kp, ki, kd);
Controller controller2(kp, ki, kd);

unsigned long turnoffMotorTime = 0;
volatile long encoderValue1 = 0;
volatile long encoderValue2 = 0;
int interval = 50;
long previousMillis = 0;
long currentMillis = 0;
int rpm1 = 0;
int rpm2 = 0;
int pwm1 = 0, pwm2 = 0;

// Define a data structure
typedef struct struct_message {
  int speed_left, speed_right;
} struct_message;


// Create a structured object
struct_message ESP_Data = {0, 0};

// put function declarations here:
void ReadEncoder1();
void ReadEncoder2();
double rightMotor1(int rpm);
double leftMotor1(int rpm);

double rightMotor2(int rpm);
double leftMotor2(int rpm);

double rightMotor3(int rpm);
double leftMotor3(int rpm);

// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  Serial.println("Chegou alguma coisa");
  memcpy(&ESP_Data, incomingData, sizeof(ESP_Data));
  turnoffMotorTime = millis();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  attachInterrupt(digitalPinToInterrupt(ENCA1), &ReadEncoder1, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCB1), &ReadEncoder2, FALLING);
  pinMode(ENCA2, INPUT);
  pinMode(ENCB2, INPUT);

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);

  /*
  motor1.move(25);
  motor2.move(25);
  delay(10000);
  
  motor1.move(50);
  motor2.move(50);
  delay(10000);

  motor1.move(100);
  motor2.move(100);
  delay(10000);

  motor1.move(150);
  motor2.move(150);
  delay(10000);

  motor1.move(200);
  motor2.move(200);
  delay(10000);
  motor1.move(255);
  motor2.move(255);
  delay(10000);
  
  */



}

void loop() {
  // put your main code here, to run repeatedly:


  /* // 7C:9E:BD:45:36:C0
  while (Serial.available()) {
    ESP_Data.speed_left = Serial.readStringUntil('\n').toFloat();
    ESP_Data.speed_right  = ESP_Data.speed_left;
  }

  Serial.print(600);
  Serial.print(" - ");
  Serial.print(rpm1);
  Serial.print(" - ");
  Serial.print(-600);
  Serial.print(" - ");
  Serial.println(rpm2);
  */
    



  // Update RPM value on every second
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    enc1.calculate_rpm(encoderValue1, currentMillis);
    rpm1 = enc1.get_rpm();
    controller1.calculate_signal_control(ESP_Data.speed_left, rpm1, currentMillis);
    encoderValue1 = 0;
    pwm1 = controller1.get_pwm();
    

    enc2.calculate_rpm(encoderValue2, currentMillis);
    rpm2 = enc2.get_rpm();
    controller2.calculate_signal_control(ESP_Data.speed_right, rpm2, currentMillis);
    encoderValue2 = 0;
    pwm2 = controller2.get_pwm();
  }

  motor1.move(leftMotor3(ESP_Data.speed_left));
  motor2.move(rightMotor3(ESP_Data.speed_right));

  
  if(millis() - turnoffMotorTime > TIME_TURN_OFF){
   ESP_Data.speed_left = 0;
   ESP_Data.speed_right = 0;
   turnoffMotorTime = millis();
  }
  

}

// put function definitions here:
void ReadEncoder1(){
  if (digitalRead(ENCA2) > 0) {
    encoderValue1++;
  } else {
    encoderValue1--;
  }

}
void ReadEncoder2(){
  if (digitalRead(ENCB2) > 0) {
    encoderValue2++;
  } else {
    encoderValue2--;
  }
}

int RPM2PWM(double rpm){
  if(rpm >= 0){
    return map(rpm, 0, 500, 0, 255);
  }
  else{
    return map(rpm, 0, -500, 0, 255);
  }
}

// ROBOT 01 (Este é o com redução)

double rightMotor1(int rpm){
  // PWM | RPM
  // 25    45
  // 50    96
  // 100   194
  // 150   296
  // 200   395
  // 255   491
  double pwm;
  pwm = 0.4571 + 0.5121 * abs(rpm);


  if(abs(rpm) > 490){
    pwm = 255;
  }

  if(rpm < 0){
    return -pwm;
  }
  return pwm;
}

double leftMotor1(int rpm){
  // PWM | RPM
  // 25    36
  // 50    78
  // 100   162
  // 150   249
  // 200   328
  // 255   409
  double pwm;
  pwm = 0.9363 + 0.6129 * abs(rpm);


  if(abs(rpm) > 409){
    pwm = 255;
  }

  if(rpm < 0){
    return -pwm;
  }
  return pwm;
}

// ROBOT 2 (Este é o segundo a ser criado)

double leftMotor2(int rpm){
  // PWM | RPM
  // 25    30
  // 50    75
  // 100   160
  // 150   248
  // 200   336
  // 255   426
  double pwm;

  pwm = 0.579 * abs(rpm) + 6.962;

  if(abs(rpm) > 426){
    pwm = 255;
  }

  if(rpm < 0){
    return -pwm;
  }
  return pwm;
}

double rightMotor2(int rpm){
  double pwm;
  // PWM | RPM Forward
  // 25    25
  // 50    122
  // 100   265
  // 150   407
  // 200   546
  // 255   695

  pwm = 0.3471 * abs(rpm) + 10.83;

  if(abs(rpm) > 690){
    pwm = 255;
  }
  if(rpm < 0){
    return -pwm;
  }
  return pwm;

}

// ROBOT 03 ( Antigo com Encoder )

double rightMotor3(int rpm){
  // PWM | RPM
  // 25      53
  // 50    122
  // 100   257
  // 150   372
  // 200   527
  // 255   670
  double pwm;

  pwm = 5.516 + 0.3733 * abs(rpm);


  if(abs(rpm) > 660){
    pwm = 255;
  }

  if(rpm < 0){
    return -pwm;
  }
  return pwm;
}

double leftMotor3(int rpm){
  // PWM | RPM
  // 25    53
  // 50    124
  // 100   263
  // 150   402
  // 200   533
  // 255   686
  double pwm;

  pwm = 0.3643 * abs(rpm) + 5.857;

  if(abs(rpm) > 680){
    pwm = 255;
  }

  if(rpm < 0){
    return -pwm;
  }
  return pwm;
}


