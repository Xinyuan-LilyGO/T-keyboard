#include <BLEDevice.h>
#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include "TKeyboardClient.h"


BLEClient* pClient = nullptr;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  MyAdvertisedDeviceCallbacks callbackDev = MyAdvertisedDeviceCallbacks();
  BLEDevice::init("T-Keyboard client");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  BLEScanResults res =pBLEScan->start(5, false);
  int count = res.getCount();
  Serial.printf("count %1d \n", count);
  for(int i=0; i < count; i++){
    Serial.print(i);
    Serial.println(" checking...");
    callbackDev.onResult(res.getDevice(i));
  }
  if(connectToServer()){
    Serial.println("We are now connected to the BLE Server.");
  }
  delay(5000);
}

void loop() {

}
