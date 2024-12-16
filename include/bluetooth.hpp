#ifndef BLUETOOTH_HPP
#define BLUETOOTH_HPP

#include <BluetoothSerial.h>
#include <Preferences.h>
#include <WiFi.h>


#define BLUETOOTH_NAME "PlantAI"

void setupBluetooth();
void closeBluetooth();
void handleBluetooth();
void scanWifiNetworks();
void saveWifiCredentials(const char* ssid, const char* password);
bool loadWifiCredentials(char* ssid, char* password);

#endif