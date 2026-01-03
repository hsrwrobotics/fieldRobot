#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t receiverAddress[] = {0xc8, 0x2e, 0x18, 0x45, 0xd7, 0x04};
// uint8_t receiverAddress[] = {0x08, 0xf9, 0xe0, 0xd3, 0xd7, 0xe4};
// Structure example to send data
typedef struct struct_message {
  int x;
  int y;
  // int flash;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

#define JoyStick_X_PIN 36
#define JoyStick_Y_PIN 33

int valueX = 0; // to store the X-axis value
int valueY = 0; // to store the Y-axis value

void setup() {
  pinMode(JoyStick_X_PIN, INPUT);
  pinMode(JoyStick_Y_PIN, INPUT);

  // Init Serial Monitor  
  Serial.begin(9600);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Set values to send
  int valueX = analogRead(JoyStick_X_PIN); // to store the X-axis value
  int valueY = analogRead(JoyStick_Y_PIN); // to store the Y-axis value

  myData.x = valueX;
  myData.y = valueY;

  Serial.print(myData.x);
  Serial.print(" | ");
  Serial.print(myData.y);
  Serial.println();

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&myData, sizeof(myData));

  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // } else {
  //   Serial.println("Error sending the data");
  // }
  delay(10);
}// i found that using 5v to power both joysticks causes issues. but using 3v3 fixed them.
