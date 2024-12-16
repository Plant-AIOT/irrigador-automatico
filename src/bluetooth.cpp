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

// Close bluetooth
void closeBluetooth() {
  SerialBT.end();
  Serial.println("Bluetooth encerrado");
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

// Handle bluetooth commands
void handleBluetooth() {
    if (SerialBT.available()) {
        String command = SerialBT.readStringUntil('\n');
        command.trim();

        static String selectedSSID;

        if (command == "scan") {
            scanWifiNetworks();
        } else if (command.startsWith("ssid:")) {
            selectedSSID = command.substring(5);
            SerialBT.println("SSID recebido: " + selectedSSID);
        } else if (command.startsWith("pass:")) {
            String wifiPassword = command.substring(5);
            SerialBT.println("Senha recebida. Tentando conectar... " + wifiPassword);

            WiFi.begin(selectedSSID.c_str(), wifiPassword.c_str());
            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 10) {
                delay(1000);
                SerialBT.println("Tentando conectar...");
                attempts++;
            }

            if (WiFi.status() == WL_CONNECTED) {
                SerialBT.println("Conectado com sucesso!");
                saveWifiCredentials(selectedSSID.c_str(), wifiPassword.c_str());
            } else {
                SerialBT.println("Falha ao conectar à rede.");
            }
        } 
    }
}


