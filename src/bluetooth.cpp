#include "bluetooth.hpp"

// Bluetooth object
BluetoothSerial SerialBT;
// NVS memory object
Preferences preferences;

// Initialize bluetooth
void setupBluetooth() {
  SerialBT.begin(BLUETOOTH_NAME);
  Serial.println("Bluetooth iniciado");
}

// Save wifi credentials to NVS memory
void saveWifiCredentials(const char* ssid, const char* password) {
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
}

// Load wifi credentials from NVS memory
bool loadWifiCredentials(char* ssid, char* password) {
  preferences.begin("wifi", true);
  String storedSSID = preferences.getString("ssid", "");
  String storedPassword = preferences.getString("password", "");
  preferences.end();

  if (storedSSID.length() > 0 && storedPassword.length() > 0) {
    strcpy(ssid, storedSSID.c_str());
    strcpy(password, storedPassword.c_str());
    return true;
  }
  return false;
}

// Scan wifi networks
void scanWifiNetworks() {
    int numNetworks = WiFi.scanNetworks();
    if (numNetworks == 0) {
        SerialBT.println("Nenhuma rede wifi encontrada.");
    } else {
        SerialBT.println("Redes disponiveis:");
        for (int i = 0; i < numNetworks; i++) {
            SerialBT.println(WiFi.SSID(i));
        }
    }
}


