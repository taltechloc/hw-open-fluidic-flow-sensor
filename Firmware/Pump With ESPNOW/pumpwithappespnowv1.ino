// Import required libraries
#ifdef ESP32
  #include <ArduinoJson.h>
  #include <FirebaseESP32.h>
  #include <esp_now.h>
  #include <WiFi.h>
  #include <WiFiUdp.h>
  #include "time.h"
#endif

// Replace with your network credentials (STATION)
const char* ssid = "TendaRozsa"; // Set your WiFi name here
const char* password = "Rozsa2020"; // Set your WiFi password here

// Set your firebase parameters here .
#define FIREBASE_HOST "flowiot-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "iPj4dcRWiuh0nlZ3al0kfLhldYtnJfZdmfgNIePG"

FirebaseData firebaseData;
FirebaseJson json;

//void printResult (FirebaseData &data);
#include "PumpI2C.h"
uint8_t ShapeByte=0; // sine
uint8_t WaveByte=0; // full wave
uint8_t DampByte=0; // disabled
uint8_t ShapeByte2=0;
uint8_t WaveByte2=0;
uint8_t DampByte2=0;
uint8_t Pump1Address =0x78;
uint8_t Pump2Address =0x7B;
#define DEBUG true
#define LED               16
#define I2C_SDA 2
#define I2C_SCL 4

//Pump1 parameters
int VoltagePeak1=255;
int pumpfreq1 = 50;
int settime1=60;
int pumpState1 = 0;
bool mrunning1 = true;

//Pump 2 parameters
int VoltagePeak2=255;
int pumpfreq2 = 50;
int settime2=60;
int pumpState2 = 0;
bool mrunning2 = true;

float average = 0;
float velocity = 0;

const char* ntpServer = "de.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    char a[32];
    float b;
    float c;
    float d;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("Velocity: ");
  Serial.println(myData.b);
  Serial.print("Flowrate: ");
  Serial.println(myData.c);
  Serial.print("Deltat: ");
  Serial.println(myData.d);
  Serial.println();
}


void setup() {

    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(115200);
    
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
   WiFi.mode(WIFI_MODE_APSTA);
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.setInt(firebaseData, "pump1", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "voltage1", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "frequency1", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "damping1", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "wave1", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "shape1", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "pump2", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "voltage2", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "frequency2", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "damping2", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "wave2", 0);                                          //send initial string of led status
Firebase.setInt(firebaseData, "shape2", 0);                                          //send initial string of led status
Firebase.setString(firebaseData, "time", "00:00:00");                                          //send initial string of led status
Firebase.setString(firebaseData, "ratestring", "0.0");                                          //send initial string of led status
Firebase.setString(firebaseData, "velocitystring", "0.0");                                          //send initial string of led status
Wire.begin(I2C_SDA, I2C_SCL);
  Driver_setup(Pump1Address);
  Driver_setup(Pump2Address);
}





void loop() {

//pump1

if (Firebase.getInt(firebaseData, "/pump1")){
    if (firebaseData.dataType() == "int"){
      pumpState1 = firebaseData.intData();
      if (pumpState1 == 1){
        Serial.println("PUMP 1 ON");
        driver_start(Pump1Address);
      }
      else if (pumpState1 == 0){
        Serial.println("PUMP 1 OFF");
        driver_stop(Pump1Address);
      }
    }
  }

  if (Firebase.getInt(firebaseData, "/voltage1")){
    if (firebaseData.dataType() == "int"){
      VoltagePeak1=firebaseData.intData(); 
        driver_setvoltage(VoltagePeak1, Pump1Address);  
        Serial.println("PUMP 1 Voltage change");
    }
  }

  if (Firebase.getInt(firebaseData, "/frequency1")){
    if (firebaseData.dataType() == "int"){
      pumpfreq1=firebaseData.intData(); 
      driver_setfrequency(pumpfreq1, Pump1Address);
     Serial.println("PUMP 1 Frequency change");
    }
  }

  if (Firebase.getInt(firebaseData, "/damping1")){
    if (firebaseData.dataType() == "int"){
        DampByte=firebaseData.intData();
      Serial.println("PUMP 1 damping change");
        driver_shape(ShapeByte,WaveByte,DampByte, Pump1Address);   
    }
  }

if (Firebase.getInt(firebaseData, "/wave1")){
    if (firebaseData.dataType() == "int"){
        WaveByte=firebaseData.intData();
      Serial.println("PUMP 1 damping change");
        driver_shape(ShapeByte,WaveByte,DampByte, Pump1Address);   
    }
  }
if (Firebase.getInt(firebaseData, "/shape1")){
    if (firebaseData.dataType() == "int"){
       ShapeByte=firebaseData.intData();
      Serial.println("PUMP 1 shape change");
        driver_shape(ShapeByte,WaveByte,DampByte, Pump1Address);  
    }
  }
//end pump1

//pump2
if (Firebase.getInt(firebaseData, "/pump2")){

    if (firebaseData.dataType() == "int"){
      pumpState2 = firebaseData.intData();
      if (pumpState2 == 1){
        Serial.println("PUMP 2 ON");
        driver_start(Pump2Address);
      }

      else if (pumpState2 == 0){
        Serial.println("PUMP 2 OFF");
        driver_stop(Pump2Address);
      }
    }
  }



  if (Firebase.getInt(firebaseData, "/voltage2")){

    if (firebaseData.dataType() == "int"){
      VoltagePeak2=firebaseData.intData(); 
        driver_setvoltage(VoltagePeak2, Pump2Address);  
        Serial.println("PUMP 2 Voltage change");
    }
  }



  if (Firebase.getInt(firebaseData, "/frequency2")){

    if (firebaseData.dataType() == "int"){
      pumpfreq2=firebaseData.intData(); 
      driver_setfrequency(pumpfreq2, Pump2Address);
     Serial.println("PUMP 2 Frequency change");
    }
  }



  if (Firebase.getInt(firebaseData, "/damping2")){

    if (firebaseData.dataType() == "int"){
        DampByte2=firebaseData.intData();
      Serial.println("PUMP 2 damping change");
        driver_shape(ShapeByte2,WaveByte2,DampByte2, Pump2Address);   
    }
  }



if (Firebase.getInt(firebaseData, "/wave2")){

    if (firebaseData.dataType() == "int"){
        WaveByte2=firebaseData.intData();
      Serial.println("PUMP 2 damping change");
        driver_shape(ShapeByte2,WaveByte2,DampByte2, Pump2Address); 
    }
  }



if (Firebase.getInt(firebaseData, "/shape2")){

    if (firebaseData.dataType() == "int"){
       ShapeByte2=firebaseData.intData();
      Serial.println("PUMP 2 shape change");
        driver_shape(ShapeByte2,WaveByte2,DampByte2, Pump2Address); 

    }
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  velocity = myData.b;
  average = myData.c;
  
  char ratesbuff[3]; //50 chars should be enough
  dtostrf(average, sizeof(ratesbuff), 2, ratesbuff);
  //Optional: Construct String object 
  String srate(ratesbuff);

  char velocitybuff[3]; //50 chars should be enough
  dtostrf(velocity, sizeof(velocitybuff), 2, velocitybuff);
  //Optional: Construct String object 
  String svelocity(velocitybuff);

    struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%H:%M:%S");
  char timeStringBuff[50]; //50 chars should be enough
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
  String asString(timeStringBuff);

  //update data for the Application
  Firebase.setString(firebaseData, "ratestring", srate);
  Firebase.setString(firebaseData, "velocitystring", svelocity);
  Firebase.setString(firebaseData, "time", asString);

  //update data for website graph and table
  // Preparation and sending of data
  json.set("rate", srate);
  json.set("velocity", svelocity);
  json.set("time", asString);
  Firebase.pushJSON(firebaseData, "/Data", json);
}
