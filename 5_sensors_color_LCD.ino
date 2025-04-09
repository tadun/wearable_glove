#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_seesaw.h>
#include <Adafruit_TFTShield18.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

Adafruit_TFTShield18 ss;

#define SD_CS    4  // Chip select line for SD card on Shield
#define TFT_CS  10  // Chip select line for TFT display on Shield
#define TFT_DC   8  // Data/command line for TFT on Shield
#define TFT_RST  -1  // Reset line for TFT is handled by seesaw!

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// initializing the pins for sensors
int number_of_sensors = 5;
int pins[5] = {A0, A1, A2, A3, A4};
int values[5] = {0, 0, 0, 0, 0};
int ports[5] = {2, 3, 4, 5, 6};
String colors[5] = {"ST77XX_RED", "ST77XX_YELLOW", "ST77XX_GREEN", "ST77XX_BLUE", "ST77XX_CYAN"};

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

void checkSensor(int a) // turing on and off the motors depending on the sensor values
{
  values[a] = analogRead(pins[a]);
  if (values[a] <= 100) {
    digitalWrite(ports[a], HIGH); //turn motor 1 on
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
    tft.setCursor(100, 25*a);
    tft.print("ON");
  }
  else {
    digitalWrite(ports[a], LOW); //turn motor 1 off
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_BLACK);
    tft.setCursor(100, 25*a);
    tft.print("ON");
  }
}

void printLCD(int a) {
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  tft.setCursor(0, 25*a);
  tft.print("S" + String(a));
  tft.setCursor(45, 25*a);
  tft.print(values[a]);
}

void setup(void) {
  // Reset the TFT
  ss.begin();
  ss.tftReset();

  // Initialize 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST77XX_BLACK);

  // Set backlight on fully
  ss.setBacklight(TFTSHIELD_BACKLIGHT_ON);
  delay(1);

  //initialLCD();
  Serial.begin(9600);
  for (int i = 0; i < number_of_sensors; i++) { // initialize all pins
    initPin(i);
  }
}

void loop() {  
  for (int i = 0; i < number_of_sensors; i++) { // check all sensors and show their value
    checkSensor(i);
    printSerial(i);
    printLCD(i);
  }

  Serial.print("\n");
  delay(100);
}
