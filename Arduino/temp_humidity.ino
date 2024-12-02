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

  float old_temp = -1000.0; // Initialize with a large difference
  float old_hum = -1000.0;  // Initialize with a large difference

  void setup() {
    Serial.begin(9600);
    while (!Serial); // Wait for Serial Monitor to open

    // Initialize HTS221 sensor
    if (!HTS.begin()) {
      Serial.println("Failed to initialize humidity and temperature sensor!");
      while (1);
    }

    // Initialize BLE
    if (!BLE.begin()) {
      Serial.println("Failed to start Bluetooth® Low Energy module!");
      while (1);
    }

    // Set up BLE device
    BLE.setLocalName("Arduino Nano 33 BLE (Peripheral)");
    BLE.setAdvertisedService(Temp_service);
    Temp_service.addCharacteristic(Temp_charac);
    BLE.addService(Temp_service);
    Temp_charac.writeValue(33);
    BLE.advertise();

    Serial.println("Nano 33 BLE (Peripheral Device) initialized");
    Serial.println("Waiting for central connection...");
  }

  void loop() {
    BLEDevice central = BLE.central();
    if(central){
      while(central.connected()){
        // Read sensor values
        float temperature = HTS.readTemperature();
        float humidity = HTS.readHumidity();
        // Check for significant changes
        if (abs(old_temp - temperature) >= 0.5 || abs(old_hum - humidity) >= 1) {
          old_temp = temperature;
          old_hum = humidity;

          // Print updated sensor values
          Serial.print("Temperature = ");
          Serial.print(temperature);
          Serial.println(" °C");
          Serial.print("Humidity    = ");
          Serial.print(humidity);
          Serial.println(" %");
          Serial.println();

          
              // Loop while central is connected
            Temp_charac.writeValue(old_temp);
            Serial.print(old_temp);
        }

      }
    }
  }




