// Include Libraries

#include <TinyPICO.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

TinyPICO tp = TinyPICO();
Adafruit_MPU6050 mpu;

int number_of_sensors = 5;
int pins[5] = {4, 32, 33, 14, 15};
int values[5] = {0, 0, 0, 0, 0};
 
// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0x98, 0xCD, 0xAC, 0xD3, 0x63, 0x38};
 
// Define a data structure
typedef struct struct_message {
  char a[32];
  int sensors[5];
  int acceleration[3];
  int rotation[3];
  int temperature;
} struct_message;
 
// Create a structured object
struct_message myData;
 
// Peer info
esp_now_peer_info_t peerInfo;
 
// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  
  // Set up Serial Monitor
  Serial.begin(9600); 
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  //full scale range of ±2g, ±4g, ±8g and ±16g
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: +-8G");

  //full-scale range of ±250, ±500, ±1000, and ±2000dps
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to +- 500 deg/s ");

  //full-scale range of 5Hz, 10Hz, 21Hz, 44Hz, 94Hz, 184Hz, 260Hz
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to 5Hz");

  Serial.println("");
  delay(100);
 
  //Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA); //PROBLEM WITH SENSORS
 
  //Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  for (int i = 0; i < number_of_sensors; i++) { // initialize all the sensors
    pinMode(pins[i], INPUT);
  }
}
 
void loop() {

  tp.DotStar_CycleColor(25);
 
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  for (int i = 0; i < number_of_sensors; i++) { // check all sensors and show their value
    values[i] = analogRead(pins[i]);
    Serial.print(analogRead(pins[i]));
    Serial.print(" ");
  }

  Serial.print(analogRead(4));

  delay(500);
  Serial.println();

  // Format structured data
  strcpy(myData.a, "Welcome to the Workshop!");
  myData.sensors[0] = values[0];
  myData.sensors[1] = values[1];
  myData.sensors[2] = values[2];
  myData.sensors[3] = values[3];
  myData.sensors[4] = values[4];

  myData.acceleration[0] = a.acceleration.x;
  myData.acceleration[1] = a.acceleration.y;
  myData.acceleration[2] = a.acceleration.z;

  myData.rotation[0] = g.gyro.x;
  myData.rotation[1] = g.gyro.y;
  myData.rotation[2] = g.gyro.z;

  myData.temperature = temp.temperature;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  delay(2000);
}