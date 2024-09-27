#include <WiFiNINA.h>

#include <Firebase.h>
#include <Config.h>
#include <Firebase_Arduino_WiFiNINA.h>
#include <Firebase_TCP_Client.h>
#include <WCS.h>
#include <SPI.h>

#include "arduino_secrets.h" 

//credentials for the host and authentication for firebase
#define DATABASE_URL "iot-traffic-light-system-default-rtdb.asia-southeast1.firebasedatabase.app"
#define DATABASE_SECRET "AIzaSyDBhDeTt1d3qwmIP5G7suzDxEf4qTtCVx8"

#define LED1_PIN 2
#define LED2_PIN 3
#define LED3_PIN 4

//varibale to check upon the ledStatus 
bool ledStatus = false;

char ssid[] = SECRET_SSID;      
char pass[] = SECRET_PASS; 

//firebaseData object will handle change on data on specific database notepad (or to carry out the communication form firebase to arduino)
FirebaseData fbdo;

void setup() {
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  pinMode(LED1_PIN,OUTPUT);
  pinMode(LED2_PIN,OUTPUT);
  pinMode(LED3_PIN,OUTPUT);
  
  while (!Serial) 
  {
    ; // wait for serial port to connect. 
  }

  // check for the WiFi module to connect
  ConnectToWifi();
  
  delay(10000);
  Serial.println("Connected Successfully");

  //connecting the firebase
  Firebase.begin(DATABASE_URL,DATABASE_SECRET,ssid,pass);
  Firebase.reconnectWiFi(true);
    
}

void loop() {

  Change_LED_status("/traffic_light/red","red");
  Change_LED_status("/traffic_light/green","green");
  Change_LED_status("/traffic_light/blue","blue");
  delay(100);
  Serial.println(" ");
  CheckWiFiSignal();
}

//function that will take the path or key that is defined in the firebase along with LED color to get the value
void Change_LED_status(String path, String color)
{
  // Check and toggle for LED
  if(Firebase.getBool(fbdo,path)) {
    //if the datatype in the firebase is same as the Bool 
    if(fbdo.dataType() == "boolean") {
      //with the object of FirebaseData I am accessing the value of the path that is mentioned in the parameters
      ledStatus = fbdo.boolData();
      Serial.println("Successful READ for "+color+" : " + ledStatus);
      toggleLED(color, ledStatus);
    }
  } else {
    Serial.println("Failed to read " +color+" LED: " + fbdo.errorReason());
  }
}

//checking the wifi signal strength
void CheckWiFiSignal() {
  long rssi = WiFi.RSSI();  // Get the signal strength (Received Signal Strength Indicator (RSSI), which measures the strength of the Wi-Fi signal in dBm (decibels relative to one milliwatt)).
  Serial.print("WiFi signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
  if (rssi < -70) {
  Serial.println("Warning: Weak WiFi signal, try moving closer to the router.");
  }
}

// Function to toggle LED based on string input
void toggleLED(String color, bool state) {
  if (color == "red") {
    digitalWrite(LED1_PIN, state);  // Red LED
  } 
  else if (color == "green") {
    digitalWrite(LED2_PIN, state);  // Green LED
  } 
  else if (color == "blue") {
    digitalWrite(LED3_PIN, state);  // Blue LED
  }
}

//function to connect to wifi
void ConnectToWifi()
{
   Serial.print("Connecting to Wi-Fi");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(ssid, pass);
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.print("Connected with ssid: " + String(ssid));
  
  Serial.println();
}
