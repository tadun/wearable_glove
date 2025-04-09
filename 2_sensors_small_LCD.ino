#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// LCD address is 0x27 and 16x2 is the resolution
LiquidCrystal_I2C lcd(0x27, 16, 2);

// initializing the pins for sensors
int pins[2] = {A0, A1};
int values[2] = {0, 0};
int ports[2] = {2, 3};

void initialLCD() // Print a message to the LCD.
{
  lcd.init(); // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("Haptic Glove");
  lcd.setCursor(3,1);
  lcd.print("Biorob Lab");
  delay(4000);
  lcd.clear();
}

void initPin(int a) // initializing inputs and outputs
{
  pinMode(ports[a], OUTPUT);
  pinMode(values[a], INPUT);
}

void printSerial(int a) // printing out the value for sensor 1
{
  String output = "Sensor_" + String(a) + ": ";
  Serial.print(output);
  Serial.print(analogRead(pins[a]));
  Serial.print("\t");
}

void printLCD (int a) // showing the value for sensor a on the LCD
{
  String output = "Sensor_" + String(a) + ":";
  lcd.setCursor(0,a);
  lcd.print(output);
  lcd.setCursor(10,a);
  lcd.print(values[a]);
}

void checkSensor(int a) // turing on and off the motors depending on the sensor values
{
  values[a] = analogRead(pins[a]);
  if (values[a] <= 100) {
    digitalWrite(ports[a], HIGH); //turn motor 1 on
  }
  else {
    digitalWrite(ports[a], LOW); //turn motor 1 off
  }
}

void setup() 
{
  initialLCD();
  Serial.begin(9600);
  for (int i = 0; i < 2; i++) { // initialize all pins
    initPin(i);
  }
}

void loop() 
{
  for (int i = 0; i < 2; i++) { // check all sensors and show their value
    checkSensor(i);
    printSerial(i);
    printLCD(i);
  }

  Serial.print("\n");
  delay(200);
  lcd.clear();
}
