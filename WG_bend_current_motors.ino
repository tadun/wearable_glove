#include <Adafruit_VL53L0X.h>
#include <Wire.h>

#define TCAADDR 0x70

// Definig variables for measuring the XYZ position
Adafruit_VL53L0X lox_y = Adafruit_VL53L0X();
Adafruit_VL53L0X lox_z = Adafruit_VL53L0X();
Adafruit_VL53L0X lox_x = Adafruit_VL53L0X();
Adafruit_VL53L0X lox[3] = {lox_y, lox_z, lox_x};

VL53L0X_RangingMeasurementData_t measure_y;
VL53L0X_RangingMeasurementData_t measure_z;
VL53L0X_RangingMeasurementData_t measure_x;
VL53L0X_RangingMeasurementData_t measurementsXYZ[3] = {measure_y, measure_z, measure_x};

// Defining used pins
int flex_pins[4] = {A0, A1, A2, A3};
int current_pins[4] = {A4, A5, A6, A7};
int emg_pin = A8;
int heart_pin = A9;
int motor_pins[5] = {2, 3, 4, 5};
int serialData;

// Data lists
int current_values[4] = {0, 0, 0, 0};
int flex_values[4] = {0, 0, 0, 0};
int xyz[3] = {0,0,0};

// Switch the active port on the I2C bus expander
void tcaselect(uint8_t i)
{
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
} 

// Read serial data from the IC2 module
void newreadData()
{
  if (Serial.available()) {
    serialData = Serial.read();
  }
}

// Check the current running into the motors
void readCurrent()
{
  for (int i = 0; i < 3; i++){
    current_values[i] = analogRead(current_pins[i]);
  }
}

// For more precise Flex sensors measurements
void calibrateFlex(int index, int max, int min)
{
  flex_values[index] = analogRead(flex_pins[index]);
    if (flex_values[index] > max) {
      flex_values[index] = max;
      }
    else if (flex_values[index] < min) {
      flex_values[0] = min;
      }
}

// Read how much each sensor is bent
void readFlex()
{
  calibrateFlex(0, 810, 795);
  calibrateFlex(1, 800, 700);  
  calibrateFlex(2, 840, 750);
  calibrateFlex(3, 810, 710);
}

// Read the coordinates on the active module
void readXYZ()
{
  for (int i = 0; i <3; i++) {
    tcaselect(i);
    lox_y.rangingTest(&measurementsXYZ[i], true);
    if (measure_y.RangeStatus != 4) {
      if (i == 2) {
        xyz[0] = measure_y.RangeMilliMeter;
      }
      xyz[i+1] = measure_y.RangeMilliMeter;
    }
  } 
}

// Run all the motors at a specified intensity
void runMotors(int input)
{
  for (int i = 0; i < 4; i++){
    analogWrite(motor_pins[i], input);
  }
}

// Decide the speed of the motors
void feedback()
{
  if (serialData == 0) {
    runMotors(0);
  }
  else if (serialData == 1) {
    runMotors(125);
  }
  else{
    runMotors(255); 
  }
}

// XYZ adjustements
void debugXYZ()
{
  int counter = 0;
  xyz[0] = 0 + counter*4;
  xyz[1] = 20 + counter;
  xyz[2] = 30 + counter;
  
  counter++;
  if (counter > 100){
    counter = 0;
  }
}

void setup()
{
  Serial.begin(9600);
  
  // Start all the VL53 sensors
  tcaselect(0);
  lox_y.begin();
  tcaselect(1);
  lox_z.begin();
  tcaselect(2);
  lox_x.begin();
}

void loop()
{
  readCurrent();
  //readFlex();
  //readXYZ();
  //runMotors(100);
  delay(1000);
}
