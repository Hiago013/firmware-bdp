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
double rightMotor(int rpm);
double leftMotor(int rpm);

double rightMotorEncoder(int rpm);
double leftMotorEncoder(int rpm);

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

  motor1.move(leftMotorEncoder(ESP_Data.speed_left));
  motor2.move(rightMotorEncoder(ESP_Data.speed_right));
  
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

double leftMotor(int rpm){
  // PWM | RPM
  // 50    0
  // 100   134
  // 150   220
  // 200   306
  // 255   400
  double x4, x3, x2, x, c, sum;
  x4 = 7.9278 * 1e-9 * pow(rpm, 4);
  x3 = 8.3259 * 1e-6 * pow(abs(rpm), 3);
  x2 = 3.1343 * 1e-3 * pow(rpm, 2);
  x  = 8.3570 * 1e-2 * abs(rpm);
  c  = 50;
  sum = x4 - x3 + x2 + x + c;

  if(sum <= 60){
    sum = 0;
  }

  if(abs(rpm) > 400){
    sum = 255;
  }

  if(rpm < 0){
    return -sum;
  }
  return sum;
}

double rightMotor(int rpm){
  double x4, x3, x2, x, c, sum;
  // PWM | RPM Forward
  // 50    113
  // 100   243
  // 150   386
  // 200   516
  // 255   653
  x4 = 8.6512 * 1e-10 * pow(rpm, 4);
  x3 = 1.6410 * 1e-6 * pow(abs(rpm), 3);
  x2 = 1.0120 * 1e-3 * pow(rpm, 2);
  x  = 6.0410 * 1e-1 * abs(rpm);
  c  = 7.5677;
  sum = -x4 + x3 - x2 + x - c;
  if(rpm < 0){
    return -sum;
  }
  return sum;

}

// Do robô com Encoder

double rightMotorEncoder(int rpm){
  // PWM | RPM
  // 50    115
  // 100   247
  // 150   372
  // 200   507
  // 255   652
  double x4, x3, x2, x, c, sum;
  x4 = 1.6043 * 1e-9 * pow(rpm, 4);
  x3 = 2.4922 * 1e-6 * pow(abs(rpm), 3);
  x2 = 1.3091 * 1e-3 * pow(rpm, 2);
  x  = 1.1758 * 1e-1 * abs(rpm);
  c  = 22.6751;
  sum = x4 - x3 + x2 + x + c;


  if(abs(rpm) > 650){
    sum = 255;
  }

  if(rpm < 0){
    return -sum;
  }
  return sum;
}

double leftMotorEncoder(int rpm){
  // PWM | RPM
  // 50    109
  // 100   237
  // 150   372
  // 200   494
  // 255   618
  double x4, x3, x2, x, c, sum;
  x4 = 1.2618 * 1e-9 * pow(rpm, 4);
  x3 = 2.1282 * 1e-6 * pow(abs(rpm), 3);
  x2 = 1.1496 * 1e-3 * pow(rpm, 2);
  x  = 6.1829 * 1e-1 * abs(rpm);
  c  = 6.3132;
  sum = -x4 + x3 - x2 + x - c;


  if(abs(rpm) > 620){
    sum = 255;
  }

  if(rpm < 0){
    return -sum;
  }
  return sum;
}