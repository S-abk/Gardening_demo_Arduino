#include <EEPROM.h>
#include "DHT.h"
#include "SI114X.h"

// Define statuses
enum Status {
    Standby  = 0,
    Warning  = 1,
    Watering = 3,
};
typedef enum Status Systemstatus;
Systemstatus WorkingStatus;

// Define warning statuses
enum WarningStatus {
    NoWarning          = 0,
    AirHumidityWarning = 1,
    AirTemperWarning   = 2,
    UVIndexWarning     = 3,
    NoWaterWarning     = 4,
};
typedef enum WarningStatus WarningStatusType;
WarningStatusType SystemWarning;

// Define limits
struct Limens {
    unsigned char UVIndex_Limen       = 9;
    unsigned char DHTHumidity_Hi      = 60;
    unsigned char DHTHumidity_Low     = 0;
    unsigned char DHTTemperature_Hi   = 30;
    unsigned char DHTTemperature_Low  = 0;
    unsigned char MoisHumidity_Limen  = 0;
    float         WaterVolume         = 0.2;
};
typedef struct Limens WorkingLimens;
WorkingLimens SystemLimens;

// Pin definitions
#define DHTPIN          A0     
#define MoisturePin     A1
#define ButtonPin       2
#define WaterflowPin    5
#define RelayPin        6

#define OneSecond       1000
#define DataUpdateInterval 10000  // 10 seconds
#define RelayOn         HIGH
#define RelayOff        LOW

#define NoWaterTimeOut  3        // 10 seconds

unsigned int  uiWaterVolume = 0;
unsigned long StartTime = 0;
DHT dht(DHTPIN, DHT11);

// Declare the SI1145 sensor object
SI114X uvSensor;

void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    dht.begin();
    
    // Initialize sensors and relay
    pinMode(ButtonPin, INPUT);
    pinMode(WaterflowPin, INPUT);
    pinMode(RelayPin, OUTPUT);
    
    // Initialize UV sensor
    while (!uvSensor.Begin()) {
        delay(1000);
        Serial.println("Initializing SI1145...");
    }

    // Initialize system status
    WorkingStatus = Standby;
}

void loop() {
    // Listen for incoming serial data
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();  // Remove any whitespace or newline characters

        if (command == "WATER") {
            // Trigger watering process
            Serial.println("Executing Watering Command");
            digitalWrite(RelayPin, RelayOn);
            delay(NoWaterTimeOut * OneSecond);
            digitalWrite(RelayPin, RelayOff);
            Serial.println("Watering Complete");
        }
    }

    switch (WorkingStatus) {
        case Standby:
            if (millis() - StartTime > DataUpdateInterval) {
                StartTime = millis();
                
                // Read sensor data
                float DHTHumidity = dht.readHumidity();
                float DHTTemperature = dht.readTemperature();
                int MoisHumidity = analogRead(MoisturePin) / 7;
                int scaledUVIndex = (int)((uvSensor.ReadUV() / 100.0 + 0.5) * 10); // Scale to avoid floating-point

                // Adjust moisture humidity value
                if (MoisHumidity > 100) {
                    MoisHumidity = 100;
                }
                
                // Send compact data over serial
                Serial.print(DHTHumidity);
                Serial.print(",");
                Serial.print(DHTTemperature);
                Serial.print(",");
                Serial.print(MoisHumidity);
                Serial.print(",");
                Serial.print(scaledUVIndex);
                Serial.print(",");
                Serial.println(SystemWarning);
                
                // Check for warnings
                if (MoisHumidity < SystemLimens.MoisHumidity_Limen) {
                    SystemWarning = NoWaterWarning;
                    Serial.println("Status: Watering");
                    digitalWrite(RelayPin, RelayOn);
                    delay(NoWaterTimeOut * OneSecond);
                    digitalWrite(RelayPin, RelayOff);
                }
                if (DHTHumidity < SystemLimens.DHTHumidity_Low || DHTHumidity > SystemLimens.DHTHumidity_Hi) {
                    SystemWarning = AirHumidityWarning;
                }
                if (DHTTemperature < SystemLimens.DHTTemperature_Low || DHTTemperature > SystemLimens.DHTTemperature_Hi) {
                    SystemWarning = AirTemperWarning;
                }
                if (scaledUVIndex > SystemLimens.UVIndex_Limen) {
                    SystemWarning = UVIndexWarning;
                }
            }
            break;
    }
}
