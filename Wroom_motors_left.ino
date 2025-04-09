// Include Libraries
#include <esp_now.h>
#include <WiFi.h>

// int number_of_sensors = 4;
// //int pins[4] = {25, 26, 34, 35};

// Define a data structure
typedef struct struct_message {
  char a[32];
  int sensors[5];
} struct_message;

// Create a structured object
struct_message myData;

// Callback function executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.print("Sensor ");
  
  // for (int i = 0; i < number_of_sensors; i++) { // check all sensors and show their value
  //     char letter = char(i + 65);
  //     Serial.print(letter);
  //     Serial.print(":");
  //     Serial.print(myData.sensors[i]);
  //     Serial.print(" ");
    
  //   if (myData.sensors[i] <= 100) {
  //     digitalWrite(pins[i], HIGH); //turn motor 1 on
  //   }
  //   else {
  //     digitalWrite(pins[i], LOW); //turn motor 1 off
  //   }
  // }
  // Serial.println();

  Serial.print("A:");
  Serial.print(myData.sensors[0]);
  Serial.print(" ");
    
  if (myData.sensors[0] <= 100) {
    digitalWrite(25, HIGH); //turn motor 1 on
  }
  else {
    digitalWrite(25, LOW); //turn motor 1 off
  }

  Serial.print("B:");
  Serial.print(myData.sensors[1]);
  Serial.print(" ");
    
  if (myData.sensors[1] <= 100) {
    digitalWrite(26, HIGH); //turn motor 1 on
  }
  else {
    digitalWrite(26, LOW); //turn motor 1 off
  }

  Serial.print("C:");
  Serial.print(myData.sensors[2]);
  Serial.print(" ");
    
  if (myData.sensors[2] <= 100) {
    digitalWrite(32, HIGH); //turn motor 1 on
  }
  else {
    digitalWrite(32, LOW); //turn motor 1 off
  }

  Serial.print("D:");
  Serial.print(myData.sensors[3]);
  Serial.print(" ");
    
  if (myData.sensors[3] <= 100) {
    digitalWrite(33, HIGH); //turn motor 1 on
  }
  else {
    digitalWrite(33, LOW); //turn motor 1 off
  }

  Serial.println();
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(9600);

  // for (int i = 0; i < number_of_sensors; i++) {
  //   pinMode(pins[i], OUTPUT);
  // }

  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // for (int i = 0; i < number_of_sensors; i++) { // check all sensors and show their value
  //   char letter = char(i + 65);
  //   Serial.print("Motor ");
  //   Serial.println(letter);
  
  //   digitalWrite(pins[i], HIGH); //turn motor i on
  //   delay(1000);
  //   digitalWrite(pins[i], LOW); //turn motor i off
  //   delay(1000);
  // }

  // digitalWrite(25, HIGH); //turn motor i on
  // delay(1000);
  // digitalWrite(25, LOW); //turn motor i off
  // delay(1000);

  // digitalWrite(26, HIGH); //turn motor i on
  // delay(1000);
  // digitalWrite(26, LOW); //turn motor i off
  // delay(1000);

  // digitalWrite(32, HIGH); //turn motor i on
  // delay(1000);
  // digitalWrite(32, LOW); //turn motor i off
  // delay(1000);

  // digitalWrite(33, HIGH); //turn motor i on
  // delay(1000);
  // digitalWrite(33, LOW); //turn motor i off
  // delay(1000);
}
