#include "Adafruit_VL53L0X.h"
//#include "I2Cdev.h"
//#include "MPU6050_6Axis_MotionApps20.h"
#include <Wire.h>

#define TCAADDR 0x70
//MPU6050 mpu;
Adafruit_VL53L0X lox_x = Adafruit_VL53L0X();
Adafruit_VL53L0X lox_y = Adafruit_VL53L0X();
Adafruit_VL53L0X lox_z = Adafruit_VL53L0X();

VL53L0X_RangingMeasurementData_t measure_x;
VL53L0X_RangingMeasurementData_t measure_y;
VL53L0X_RangingMeasurementData_t measure_z;

int dataPack[12];
int haptic_pins[5] = {A0, A1, A2, A3, A4};
int current_pins[5] = {A5, A6, A7};
int motor_pins[5] = {2, 3, 4};
int motor = 0;
//byte touch[1] = {0};
int flex_val[5] = {0, 0, 0, 0, 0};
int press_val[5] = {0, 0, 0, 0, 0};
int xyz[3] = {0,0,0};
int emg_pin = A10;
int heart_pin = A11;
unsigned long count = 0;
unsigned long count_prev = 0;
int cycount;
int counter = 0;
bool masterMode = false;
char touch = '0';
char junk;

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
} 

void sendData(){
  for (int i = 0; i < 9; i++)
    {
      Serial.write((byte)dataPack[i]);
    }
}
void printData(){
  for (int i = 0; i < 8; i++){
    Serial.print(dataPack[i]);
    Serial.print(",");
  }
  Serial.println(dataPack[8]);
}
// void readData() {
//   if (Serial.available() && Serial.read() == 255) {
//     Serial.readBytesUntil(254,touch,1);
//   }
// }
void newreadData(){
  if (Serial.available()){
    touch = Serial.read();
    junk = Serial.read();
    //Serial.println(touch);
  }
}
// void printout(){
//   if (touch == '0'){
//     Serial.println(0);
//   }
//   else if (touch == '1'){
//     Serial.println(125);
//   }
//   else{
//     Serial.println(255); 
//   }
// }

void readcurrent(){
  for (int i = 0; i < 3; i++){
    dataPack[i+9] = analogRead(current_pins[i]);
  }
}
void readbend(){
  flex_val[0] = analogRead(haptic_pins[0]);
    if (flex_val[0] > 810) {
      flex_val[0] = 810;
    }
    else if (flex_val[0] < 795) {
      flex_val[0] = 795;
    }
    dataPack[1] = map(flex_val[0],810,795,0,50);
    
  for (int i = 0; i < 2; i++){
    flex_val[i+1] = analogRead(haptic_pins[i+1]);
    if (flex_val[i+1] > 800) {
      flex_val[i+1] = 800;
    }
    else if (flex_val[i+1] < 700) {
      flex_val[i+1] = 700;
    }
    dataPack[i+2] = map(flex_val[i+1],800,700,0,50);
  }
  
  flex_val[3] = analogRead(haptic_pins[3]);
  if (flex_val[3] > 840) {
    flex_val[3] = 840;
  }
  else if (flex_val[3] < 750) {
    flex_val[3] = 750;
  }
  dataPack[4] = map(flex_val[3],840,750,0,50);
  
  flex_val[4] = analogRead(haptic_pins[4]);
  if (flex_val[4] > 810) {
    flex_val[4] = 810;
  }
  else if (flex_val[4] < 710) {
    flex_val[4] = 710;
  }
  dataPack[5] = map(flex_val[4],810,710,0,50);
}

void readxyz() {
  tcaselect(1);
    lox_z.rangingTest(&measure_z, false); // pass in 'true' to get debug data printout!
    tcaselect(2);
    lox_x.rangingTest(&measure_x, false); // pass in 'true' to get debug data printout!
    tcaselect(0);
    lox_y.rangingTest(&measure_y, false); // pass in 'true' to get debug data printout!
    if (measure_z.RangeStatus != 4) {  // phase failures have incorrect data
    xyz[2] = measure_z.RangeMilliMeter;
  } 
  
  if (measure_x.RangeStatus != 4) {  // phase failures have incorrect data
    xyz[0] = measure_x.RangeMilliMeter;
  } 
  
  if (measure_y.RangeStatus != 4) {  // phase failures have incorrect data
    xyz[1] = measure_y.RangeMilliMeter;
  }
  for (int i = 0; i <3; i++){
    dataPack[i+6] = clamp_convert_xyz(xyz[i]);
  } 
}

int clamp_convert_xyz(int input){
  int output = input;
  if (output > 250){
    output = 250;
  }
  return output;
}
void printFlexraw(){
  for (int i = 0; i <5; i++){
    dataPack[i] = analogRead(haptic_pins[i]);
  }
  for (int i = 0; i < 5; i++){
    Serial.print(dataPack[i]);
    Serial.print("\t");
  }
  Serial.println("");
}
void buzz(int input){
  for (int i = 0; i < 3; i++){
    analogWrite(motor_pins[i],input);
  }
}

void feedback(){
  if (touch == '0'){
    buzz(0);
  }
  else if (touch == '1'){
    buzz(125);
  }
  else{
    buzz(255); 
  }
}
void debugxyz(){
  xyz[0] = 0 + counter*4;
  xyz[1] = 20 + counter;
  xyz[2] = 30 + counter;
  for (int i = 0; i <3; i++){
    dataPack[i+6] = xyz[i];
  }
  counter++;
  if (counter > 100){
    counter = 0;
  }
}

void testing(){
  if (touch == '0'){
    digitalWrite(2,HIGH);
    Serial.print("HIGH ");
    Serial.println(touch);
  }
  else{
    digitalWrite(2,LOW);
    Serial.print("LOW ");
    Serial.println(touch);
  }
}
// void printDataDebug(){
//   for (int i = 0; i < 8; i++){
//     Serial.print(dataPack[i]);
//     Serial.print(",");
//   }
//   Serial.print(dataPack[8]);
//   Serial.print(",");
//   Serial.println(touch);
// }

void setup() {
  Serial.begin(115200);
  dataPack[0] = 255;
  tcaselect(1);
  lox_z.begin();
  tcaselect(2);
  lox_x.begin();
  tcaselect(0);
  lox_y.begin();
}
void loop() {
  count = millis();
  if (count - count_prev >= 200){
    buzz(0);
    //newreadData();
    //readbend();
    //readxyz();
    //debugxyz();
    //readcurrent();
//    if (!masterMode){
//    feedback();
//    }
    //testing();
    //sendData();
    //printData();
    //printDataDebug();
    count_prev = count;
  }
}
