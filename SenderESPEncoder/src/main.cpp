
// MAC Address: 94:B9:7E:C0:57:64
// MAC Address: 7C:9E:BD:45:36:C0
// MAC Address: C8:F0:9E:F4:AC:8C
// MAC Address: 48:E7:29:97:47:54

// Including Libraries
#include <esp_now.h>
#include <WiFi.h>


#define BUFFER_SIZE 10    //Tamanho dos dados enviados
#define led 2
// Variables to serial data
char receivedChar;
boolean newData = false;
bool state = false;
String data;

char * ptr;
int values_speed[6] = {0,0,0,0,0,0};

// MAC Address of Drone ESP32
uint8_t broadcastAddress1[] = {0x08, 0xD1, 0xF9, 0x99, 0xE5, 0xC4}; // Primeiro
uint8_t broadcastAddress2[] = {0x7C, 0x9E, 0xBD, 0x45, 0x36, 0xC0}; // Segundo
uint8_t broadcastAddress3[] = {0x48, 0xE7, 0x29, 0x9A, 0x2D, 0xA0}; // Terceiro



// Define a data structure
typedef struct struct_message {
  int speed_left, speed_right;
} struct_message;


// Create a structured object
struct_message speed_data_r1;
struct_message speed_data_r2;
struct_message speed_data_r3;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {

  // Set up Serial Monitor
  Serial.begin(115200);

  pinMode(led, OUTPUT);

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);

  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // register second peer  
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // register third peer  
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


}


void loop() {
  // Function to Receive Data
  if (Serial.available() > 0) {

        int i = 0;
        data = Serial.readStringUntil('\n');
        char arr[data.length() + 1];
        strcpy(arr, data.c_str());
        ptr = strtok(arr, ",");
        while (ptr != NULL) {
          values_speed[i] = atoi(ptr);
          ptr = strtok(NULL, ",");
          i++;
        }
        
        speed_data_r1.speed_left = values_speed[0];
        speed_data_r1.speed_right = values_speed[1];
        speed_data_r2.speed_left = values_speed[2];
        speed_data_r2.speed_right = values_speed[3];
        speed_data_r3.speed_left = values_speed[4];
        speed_data_r3.speed_right = values_speed[5];

        Serial.println(values_speed[5]);



        

        // Send message by ESP-NOW
        esp_err_t result_1 = esp_now_send(broadcastAddress1, (uint8_t *) &speed_data_r1, sizeof(speed_data_r1));

        // Send message by ESP-NOW
        esp_err_t result_2 = esp_now_send(broadcastAddress2, (uint8_t *) &speed_data_r2, sizeof(speed_data_r2));

        // Send message by ESP-NOW
        esp_err_t result_3 = esp_now_send(broadcastAddress3, (uint8_t *) &speed_data_r3, sizeof(speed_data_r3));

        }

        delay(50);
    }








