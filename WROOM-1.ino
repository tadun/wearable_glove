// Import libraries
#include <SPI.h>
#include <Wire.h>
#include "WiFi.h"
#include <esp_now.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// OLED resolution
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x3C, 0xE9, 0x0E, 0x85, 0x86, 0xE0};

// Name the gyroscope
Adafruit_MPU6050 mpu;

// Initialize lists for pins
int number_of_pins = 4;
int motor_pins[4] = {4, 5, 18, 19};
int sensor_pins[4] = {32, 33, 34, 35};
int values[5] = {0, 0, 0, 0, 0};

// Define the data structure for sending data
typedef struct struct_message {
  char a[32];
  int sensors[4];
  int rotation[3];
  int acceleration[3];
  int temperature;
  int HR;
  int EMG;
} struct_message;

// Create a structured objects
struct_message inData;
struct_message outData;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  Serial.print("\r\nLast Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback function executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) 
{
  memcpy(&inData, incomingData, sizeof(inData));
  
  printSerial();
  printOLED();

  for (int i = 0; i < number_of_pins; i++) { // check all sensors and show their value
    if (inData.sensors[i] <= 3900) {
      digitalWrite(motor_pins[i], HIGH); //turn motor i on
      delay(1000);
      digitalWrite(motor_pins[i], LOW);
      }
    else {
      digitalWrite(motor_pins[i], LOW); //turn motor i off
    }
  }
}

void printSerial() 
{
  // Printing pressure sensor values
  Serial.print("Sensor");
  for (int i = 0; i < number_of_pins; i++) {
    Serial.print(" ");
    Serial.print(char(65+i));
    Serial.print(":");
    Serial.print(inData.sensors[i]);
  }
  Serial.println();

  // Printing acceleration
  Serial.print("Acceleration");
  for (int i = 0; i < 3; i++) {
    Serial.print(" ");
    Serial.print(char(88+i));
    Serial.print(":");
    Serial.print(inData.acceleration[i]);
  }
  Serial.println(" m/s^2");

  // Printing rotation
  Serial.print("Rotation");
  for (int i = 0; i < 3; i++) {
    Serial.print(" ");
    Serial.print(char(88+i));
    Serial.print(":");
    Serial.print(inData.rotation[i]);
  }
  Serial.println(" rad/s");
  
  // Printing temperature
  Serial.print("Temperature: ");
  Serial.print(inData.temperature);
  Serial.println(" degC");

  Serial.print("Heart Rate: ");
  Serial.print(inData.HR);
  Serial.print(" EMG: ");
  Serial.println(inData.EMG);
}

void printOLED()
{
  // Heading
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(20, 0);
  display.print("INCOMING DATA");

  // Pressure sensors
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 8);
  display.print("Press");

  for(int i = 0; i < 5; i++) {
    display.setCursor(0, 17 + 9*i);
    display.print(char(65+i)); //A, B, C, D, E
    display.print(":");
    display.print(outData.sensors[i]);    
  } 

  // Acceleration and temperature
  display.setCursor(42, 8);
  display.print("Accel");

  for(int i = 0; i < 3; i++) {
    display.setCursor(42, 17 + 9*i);
    display.print(char(88+i)); //X, Y, Z
    display.print(":");
    display.print(0);
  }
  
  display.setCursor(42, 44);
  display.print("T:");
  display.print(0);

  // Rotation, Hearth Rate, EMG
  display.setCursor(80, 8);
  display.print("Gyro");

  for(int i = 0; i < 3; i++) {
    display.setCursor(80, 17 + 9*i);
    display.print(char(88+i)); //X, Y, Z
    display.print(":");
    display.print(0);
  }
  
  display.setCursor(80, 44);
  display.print("EMG:");
  display.print(0);
  
  display.setCursor(80, 53);
  display.print("HR:");
  display.print(0);

  display.display();
}

void espNow()
{
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);

  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}

void gyroscope()
{
  // Try to initialize the Gyroscope
  Serial.println("Adafruit MPU6050 test!");
  if(!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while(1) {delay(10);}
  }
  Serial.println("MPU6050 Found!");

  // Full scale range of ±2g, ±4g, ±8g and ±16g
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: +-8G");

  // Full-scale range of ±250, ±500, ±1000, and ±2000dps
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to +- 500 deg/s ");

  // Full-scale range of 5Hz, 10Hz, 21Hz, 44Hz, 94Hz, 184Hz, 260Hz
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to 5Hz");
}

void testMotors()
{
  for(int i = 0; i < number_of_pins; i++) {
    digitalWrite(motor_pins[i], HIGH); //turn motor i on
    delay(1000);
    digitalWrite(motor_pins[i], LOW); //turn motor i off
    delay(1000);
  }
}

void setup() 
{
  // Set up Serial Monitor
  Serial.begin(9600);

  // Initialize used pins
  for (int i = 0; i < number_of_pins; i++) {
    pinMode(motor_pins[i], OUTPUT);
    pinMode(sensor_pins[i], INPUT);
  }

  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  gyroscope(); //start the gyroscope
  
  espNow(); //start wireless communication with peer
}

void loop() {

  // Get new sensor events with the readings
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Format structured data
  strcpy(outData.a, "WG-WROOM");
  outData.sensors[0] = values[0];
  outData.sensors[1] = values[1];
  outData.sensors[2] = values[2];
  outData.sensors[3] = values[3];

  outData.acceleration[0] = a.acceleration.x;
  outData.acceleration[1] = a.acceleration.y;
  outData.acceleration[2] = a.acceleration.z;

  outData.rotation[0] = g.gyro.x;
  outData.rotation[1] = g.gyro.y;
  outData.rotation[2] = g.gyro.z;

  outData.temperature = temp.temperature;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outData, sizeof(outData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  
  printSerial();
  delay(1000);
}