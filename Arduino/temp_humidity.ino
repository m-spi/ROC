/*
HTS221 - Temp_Humidity with BLE Gesture Control

This example reads data from the on-board HTS221 sensor of the
Nano 33 BLE Sense and prints the temperature and humidity sensor
values to the Serial Monitor when they change significantly.
Additionally, it handles gesture control over BLE.

The circuit:
- Arduino Nano 33 BLE Sense
*/

#include <Arduino_HTS221.h>
#include <ArduinoBLE.h>

// BLE UUIDs
const char* deviceServiceUuid = "185A";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

// Global variables
BLEService Temp_service(deviceServiceUuid);
BLEFloatCharacteristic Temp_charac(deviceServiceCharacteristicUuid, BLERead);

byte manufacturerData[6] = { 1, 2, 3, 4, 5, 6 };

float old_temp = -1000.0;  // Initialize with a large difference
float old_hum = -1000.0;   // Initialize with a large difference

void setup() {
  Serial.begin(9600);
  // Initialize HTS221 sensor
  if (!HTS.begin()) {
    while (1);
  }

  // Initialize BLE
  if (!BLE.begin()) {
    while (1);
  }

  // Set up BLE device
  BLE.setLocalName("ROC0");

  // Data to recognise the device
  BLE.setManufacturerData(manufacturerData, 6);

  // Add characteristic
  Temp_service.addCharacteristic(Temp_charac);
  BLE.addService(Temp_service);
  //BLE.setAdvertisedService(Temp_service);
  //Temp_charac.writeValue(33);
  //BLE.setConnectionInterval(0x0006, 0x0c80);

  BLE.setConnectable(true);
  BLE.advertise();
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    while (central.connected()) {
      // Read sensor values
      float temperature = HTS.readTemperature();
      float humidity = HTS.readHumidity();
      // Check for significant changes
      if (abs(old_temp - temperature) >= 0.5 || abs(old_hum - humidity) >= 1) {
        old_temp = temperature;
        old_hum = humidity;

        Temp_charac.writeValue(old_temp);
      }
    }
  }
}
