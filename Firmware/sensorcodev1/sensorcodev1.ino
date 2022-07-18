#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <Ewma.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xA4, 0xE5, 0x7C, 0xE0, 0xF6, 0x84};

extern volatile unsigned long timer0_millis;

// Insert your SSID
constexpr char WIFI_SSID[] = "TendaRozsa";

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}

const int APin1 = 39;// Analog pin flow sensor1
const int APin2 = 34;// Analog pin flow sensor2

int ADCValue1;// do not change
int ADCValue2;// do not change
float voltage1 =0;// Voltage value from sensor1
float voltage2 =0;// Voltage value from sensor2
unsigned long timeone = 0;
unsigned long timetwo = 0;
const int threshold = 1.00;
float Deltat = 0;
float Velocity = 0;
float vsecond = 0;
float flowrate = 0;

//Ewma adcFilter1(0.1);   // Less smoothing - faster to detect changes, but more prone to noise
Ewma adcFilter2(0.01);  // More smoothing - less prone to noise, but slower to detect changes

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  float b;
  float c;
  float d;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  int32_t channel = getWiFiChannel(WIFI_SSID);

  WiFi.printDiag(Serial); // Uncomment to verify channel number before
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  WiFi.printDiag(Serial); // Uncomment to verify channel number before

    // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
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
}


void loop() {
  // read the input on analog pin potPin:
  ADCValue1 = analogRead(APin1);
  voltage1 = (3.3/4095.0) * ADCValue1;
  ADCValue2 = analogRead(APin2);
  voltage2 = (3.3/4095.0) * ADCValue2;

  if(voltage2>=threshold){
    Deltat = 0;
    //vsecond = 0;
    //flowrate = 0;
    timeone=millis();
    }

    if(voltage1>=threshold){
      //Deltat = 0;
      timetwo=millis();
      Deltat = timetwo - timeone;
      if(Deltat != 0){
      Velocity = 3.0/Deltat;
      vsecond = Velocity * 10.0; 
      flowrate = 3.14 * 0.5 * 0.5 * vsecond;
      }
      }

  
  //float average = adcFilter1.filter(raw);
  float average = adcFilter2.filter(flowrate);
  // send it to the computer as ASCII digits
  Serial.println(average);
  delay(1);        // delay in between reads for stability

  // Set values to send
  strcpy(myData.a, "FLOW SENSOR DATA");
  myData.b = vsecond;
  myData.c = average;
  myData.d = Deltat;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  
  
  

  
  Serial.print("ADCValue1:");
  Serial.print(ADCValue1);
   
  Serial.print(" Voltage1:");
  Serial.print(voltage1);
  Serial.println("V"); 
  
  Serial.print("ADCValue2:");
  Serial.print(ADCValue2);
   
  Serial.print(" Voltage2:");
  Serial.print(voltage2);
  Serial.println("V");   
  Serial.println(Deltat);
  Serial.println(vsecond);
  Serial.println(flowrate);
  Serial.println(average);
  delay(18); // delay in between reads for stability
}
