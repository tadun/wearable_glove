#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// LCD address is 0x3F and 24x4 is the resolution
LiquidCrystal_I2C lcd(0x3F, 24, 4);

// initializing the pins for motors and sensors.
int number_of_sensors = 5;
int motor_pins[5] = {2, 3, 4, 5, 6};
int sensor_pins[5] = {A0, A1, A2, A3, A4};
int sensor_values[5] = {0, 0, 0, 0, 0};

void initLCD() // print an initial message on the LCD
{
  lcd.init(); // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(4,0);
  lcd.print("Haptic Glove");
  lcd.setCursor(5,2);
  lcd.print("Biorob Lab");
  delay(4000);
  lcd.clear();
}

void initPin(int a) // initializing inputs and outputs
{
  pinMode(motor_pins[a], OUTPUT);
  pinMode(sensor_values[a], INPUT);
}

void printSerial(int a) // printing out the value for sensor a
{
  String output = "Sensor_" + String(a) + ": ";
  Serial.print(output);
  Serial.print(analogRead(sensor_pins[a])); // gets and prints the value of sensor a
  Serial.print("\t");
}

void printLCD (int a) // showing the value for sensor a on the LCD
{
  String output = "S" + String(a) + ":";
  int b = (a/4)*7; // to make sure the output is on the correct line
  lcd.setCursor(b, a%4);
  lcd.print(output);
  lcd.setCursor(b+3, a%4);
  lcd.print(sensor_values[a]);

}

void runMotor(int a) // turning on and off the motor depending on the sensor values
{
  sensor_values[a] = analogRead(sensor_pins[a]); // read sensor a
  if (sensor_values[a] <= 100) {
    digitalWrite(motor_pins[a], HIGH); // turn motor a on
  }
  else {
    digitalWrite(motor_pins[a], LOW); // turn motor a off
  }
}

void setup() 
{
  initLCD();
  Serial.begin(9600);
  for (int i = 0; i < number_of_sensors; i++) { // initialize all pins
    initPin(i);
  }
}

void loop() 
{
  for (int i = 0; i < number_of_sensors; i++) { // check all sensors show their value and run motors
    runMotor(i);
    printSerial(i);
    printLCD(i);
  }

  Serial.print("\n");
  lcd.clear();
}
