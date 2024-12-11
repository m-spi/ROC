/*
HTS221 - Temp_Humidity with BLE Gesture Control

This example reads data from the on-board HTS221 sensor of the
Nano 33 BLE Sense and prints the temperature and humidity sensor
values to the Serial Monitor when they change significantly.
Additionally, it handles gesture control over BLE.

The circuit:
- Arduino Nano 33 BLE Sense
*/

/*** Includes ***/
#include <ArduinoBLE.h>
#include <Wire.h>

#include <Arduino_HTS221.h>
#include <I2CSoilMoistureSensor.h>


/*** Functions prototypes ***/
void disconnectHandler(BLEDevice central);

void connectHandler(BLEDevice central);


/*** Global variables ***/
// Moisture sensor
// NOTE: HTS address is 0xbe
I2CSoilMoistureSensor moistureSensor(0x20);

// BLE UUIDs
const char* device_service_uuid = "185A";
const char* air_temp_characteristic_uuid = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* air_hum_characteristic_uuid = "ff7b36c6-46fd-4577-af82-7bc30c876221";
const char* soil_moisture_characteristic_uuid = "6041f41e-cfee-4a0f-96e0-ce7ba222482e";

// Global variables
BLEService service(device_service_uuid);
BLEFloatCharacteristic airTempCharac(air_temp_characteristic_uuid, BLERead);
BLEFloatCharacteristic airHumCharac(air_hum_characteristic_uuid, BLERead);
BLEFloatCharacteristic soilMoistureCharac(soil_moisture_characteristic_uuid, BLERead);

byte manufacturer_data[6] = { 1, 2, 3, 4, 5, 6 };

// Initialize with a large difference
float old_air_temp = -1000.0;
float old_air_hum = -1000.0;
float old_soil_moisture = -1000.0;
float old_soil_temp = -1000.0;


/*** Setup function ***/
void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize HTS221 sensor
  if (!HTS.begin()) {
    while (1);
  }

  // Will wait for 1 second to start the sensor
  moistureSensor.begin(true);
  // Start measurements so that next reads will be immediate
  moistureSensor.startMeasureLight();
  moistureSensor.getCapacitance();


  // Initialize BLE
  if (!BLE.begin()) {
    while (1);
  }

  /* Set up BLE device */
  BLE.setEventHandler(BLEConnected, connectHandler);
  BLE.setEventHandler(BLEDisconnected, disconnectHandler);
  BLE.setLocalName("ROC");
  // Data to recognise the device
  BLE.setManufacturerData(manufacturer_data, 6);

  // Add characteristic
  service.addCharacteristic(airTempCharac);
  service.addCharacteristic(airHumCharac);
  service.addCharacteristic(soilMoistureCharac);
  BLE.addService(service);
  //BLE.setAdvertisedService(Temp_service);
  //Temp_charac.writeValue(33);

  BLE.setConnectable(true);
  BLE.advertise();

  Serial.println("Setup finished");
}


/*** Loop function ***/
void loop() {
  BLEDevice central = BLE.central();
  double air_temperature;
  double air_humidity;
  double soil_moisture;

  if (central) {
    while (central.connected()) {
      // Read sensor values
      air_temperature = HTS.readTemperature();
      air_humidity = HTS.readHumidity();

      // Check for significant changes
      if (abs(old_air_temp - air_temperature) >= 0.1) {
        Serial.print("Air temperature : ");
        Serial.println(air_temperature);
        old_air_temp = air_temperature;

        airTempCharac.writeValue(air_temperature);
      }

      if (abs(old_air_hum - air_humidity) >= 0.5) {
        Serial.print("Air humidity : ");
        Serial.println(air_humidity);
        old_air_hum = air_humidity;

        airHumCharac.writeValue(air_humidity);
      }

      if (!moistureSensor.isBusy()) {
        // Moisture formula is based on https://github.com/Miceuz/i2c-moisture-sensor/blob/master/Soil%20Moisture%20Sensor%20Calibration.pdf
        soil_moisture = (double)moistureSensor.getCapacitance();
        soil_moisture = (0.0001007 * soil_moisture * soil_moisture) + (0.0024885 * soil_moisture) - 6.2508722;

        if (abs(old_soil_moisture - soil_moisture) >= 0.5) {
          Serial.print("Soil moisture : ");
          Serial.println(soil_moisture);
          old_soil_moisture = soil_moisture;

          soilMoistureCharac.writeValue(soil_moisture);
        }
      }
    }
  }
}


/*** Other functions ***/
void disconnectHandler(BLEDevice central) {
  BLE.advertise();
  Serial.println("Disconnected");
}

void connectHandler(BLEDevice central) {
  BLE.stopAdvertise();
  Serial.println("Connected");
}
