// Include Libraries
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

int number_of_sensors = 4;
int pins[4] = {32, 33, 34, 35};
int values[4] = {0, 0, 0, 0};
 
// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0x0c, 0xb8, 0x15, 0x78, 0x76, 0xb8};
 
// Define a data structure
typedef struct struct_message {
  char a[32];
  int sensors[4];
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

  for (int i = 0; i < number_of_sensors; i++) { // check all sensors and show their value
    values[i] = analogRead(pins[i]);
  }

  // Format structured data
  strcpy(myData.a, "WG-WROOM");
  myData.sensors[0] = values[0];
  myData.sensors[1] = values[1];
  myData.sensors[2] = values[2];
  myData.sensors[3] = values[3];
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  delay(1000);
}
