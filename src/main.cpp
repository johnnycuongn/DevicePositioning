#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <WiFi.h>
#include "ThingSpeak.h"

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

const String targetName = "iPhoneDuc";
const String anchor1 = "bleAnchor1";
const String anchor2 = "bleAnchor2";

const char* anchor1_ssid = "wifiAnchor1";
const char* anchor2_ssid = "wifiAnchor2";



unsigned long myChannelNumber = 2;
const char * myWriteAPIKey = "SGXEV1URZ8AY77SX";

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      // Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());

      String detectName = advertisedDevice.getName().c_str();
      if (detectName.equals(targetName)) {
        Serial.print("Target device BLE: ");
        Serial.println(advertisedDevice.getRSSI());
        ThingSpeak.setField(1, advertisedDevice.getRSSI());
      }

      if (detectName.equals(anchor1)) {
        Serial.print("Anchor 1 BLE: ");
        Serial.println(advertisedDevice.getRSSI());
        ThingSpeak.setField(2, advertisedDevice.getRSSI());
      }

      if (detectName.equals(anchor2)) {
        Serial.print("Anchor 2 BLE: ");
        Serial.println(advertisedDevice.getRSSI());
        ThingSpeak.setField(3, advertisedDevice.getRSSI());
      }

      // int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      // if (status == 200) {
      //   Serial.println("Channel update successful.");
      //   Serial.println('-------------------------------');
      // }
      // else {
      //   Serial.println("Problem updating channel. HTTP error code " + String(status));
      // }
    }
};

WiFiClient client;
const char* ssid = "SaeForWork";   // your network SSID (name) 
const char* password = "08102001";   // your network password



void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  WiFi.mode(WIFI_STA);  
  ThingSpeak.begin(client);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

}

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 15000;

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password); 
      delay(5000);
      Serial.print(".");    
    }
    Serial.println("\nConnected.");
  }

  if ((millis() - lastTime) > timerDelay) {
    // Connect to wifi

    // put your main code here, to run repeatedly:
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    pBLEScan->clearResults();  

    int n = WiFi.scanNetworks();
    if (n == 0) {
      Serial.println("No networks found.");
    } else {
      ThingSpeak.setField(4, WiFi.RSSI());
      for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        if (ssid == anchor1_ssid || ssid == anchor2_ssid) {
          if (ssid == anchor1_ssid) {
            Serial.print("Anchor 1 RSSI: ");
            Serial.println(WiFi.RSSI(i));
            ThingSpeak.setField(5, WiFi.RSSI(i));
          } 
          
          if (ssid == anchor2_ssid) {
            Serial.print("Anchor 2 RSSI: ");
            Serial.println(WiFi.RSSI(i));
            ThingSpeak.setField(6, WiFi.RSSI(i));
          }
        }
      }

      int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      if (status == 200) {
        Serial.println("Channel update successful.");
        Serial.println('-------------------------------');
      }
      else {
        Serial.println("Problem updating channel. HTTP error code " + String(status));
      }
      Serial.println('-------------------------------');
      lastTime = millis();
    }
  }
}