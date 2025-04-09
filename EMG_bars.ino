#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// setting the pins for motors and sensors.
int number_of_sensors = 5;
int motor_pins[5] = {2, 3, 4, 5, 6};
int haptic_pins[5] = {A0, A1, A2, A3, A4};
int haptic_values[5] = {0, 0, 0, 0, 0};
int emg_pin = A8;
int heart_pin = A9;

// EMG data values
int max_analog_dta = 300;  // max analog data
int min_analog_dta = 100;  // min analog data
int static_analog_dta = 0;  // static analog data

int getAnalog(int pin) // get analog value from the EMG
{
    long sum = 0;
    
    for(int i=0; i<32; i++) {
        sum += analogRead(pin);
    }
    
    int dta = sum>>5; // move the value by 5 bits
    
    // make sure the value does not exceed the max and min
    max_analog_dta = dta>max_analog_dta ? dta : max_analog_dta; // if max data
    min_analog_dta = min_analog_dta>dta ? dta : min_analog_dta; // if min data
    
    return dta;
}

int getEMGBar() // map the EMG values to a 1-10 scale
{
  int level;
  int val = getAnalog(emg_pin);
  
  if(val>static_analog_dta) { // larger than static_analog_dta
    level = 5 + map(val, static_analog_dta, max_analog_dta, 0, 10);
  }
  else {
    level = 5 - map(val, min_analog_dta, static_analog_dta, 0, 10);
  }
  delay(10);

  return level;
}

void printHaptic(int a)  // printing out the value for sensor a
{
  String output = "Haptic_" + String(a) + ": ";
  Serial.print(output);
  Serial.print(analogRead(haptic_pins[a]));  // gets and prints the value of sensor a
  Serial.print("\t");
}

void printHRandEMG() {
  int value;
  display.setTextSize(2);

  // get the heart rate sensor value
  display.setCursor(0,0);
  display.print("HR: ");
  display.print(analogRead(heart_pin));

  // get the muscle sensor value
  display.setCursor(0,16);
  display.print("EMG: ");
  value = getEMGBar();
  display.print(value);

  display.display();
}

void runMotor(int a)  // turning on and off the motor depending on the sensor values
{
  haptic_values[a] = analogRead(haptic_pins[a]);  // read sensor a
  if (haptic_values[a] <= 100) {
    digitalWrite(motor_pins[a], HIGH);  // turn motor a on
  } 
  else {
    digitalWrite(motor_pins[a], LOW);  // turn motor a off
  }
}

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

  // initializing the motor pins
  for (int i = 0; i < number_of_sensors; i++) {
    pinMode(motor_pins[i], OUTPUT);
    pinMode(motor_pins[i], INPUT);
  }

  // initialize pins for the heart rate sensor  
  pinMode(52, INPUT); // setup for leads off detection LO +
  pinMode(53, INPUT); // setup for leads off detection LO -

  // get the static value of the EMG sensor
  long sum = 0;

  for(int i=0; i<=10; i++) {
    for(int j=0; j<100; j++) {
        sum += getAnalog(emg_pin);
        delay(1);
    }
  }
  
  Serial.print("static_analog_dta = ");
  Serial.println(sum/1100);
}

void loop() {
  printHRandEMG();
  for (int i = 0; i < number_of_sensors; i++) {  // check all sensors show their value and run motors
    runMotor(i);
    printHaptic(i);
  }
  Serial.print("\n");
  delay(400);
  display.clearDisplay();
}
