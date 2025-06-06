#include <Arduino.h>
#include <SensirionI2cScd4x.h>
#include <Wire.h>

// macro definitions
// make sure that we use the proper definition of NO_ERROR
#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

SensirionI2cScd4x sensor;

static char errorMessage[64];
static int16_t error;

void PrintUint64(uint64_t& value) {
    Serial.print("0x");
    Serial.print((uint32_t)(value >> 32), HEX);
    Serial.print((uint32_t)(value & 0xFFFFFFFF), HEX);
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    
    Serial.println("=== ESP32 SCD41 CO2 Sensor ===");
    
    // Initialize I2C with your wiring: SDA=21, SCL=22
    Wire.begin(21, 22);
    sensor.begin(Wire, SCD41_I2C_ADDR_62);

    uint64_t serialNumber = 0;
    delay(30);
    
    // Ensure sensor is in clean state
    error = sensor.wakeUp();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute wakeUp(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
    }
    
    error = sensor.stopPeriodicMeasurement();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
    }
    
    error = sensor.reinit();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute reinit(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
    }
    
    // Read out information about the sensor
    error = sensor.getSerialNumber(serialNumber);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        Serial.println("SCD41 not found! Check wiring:");
        Serial.println("- SDA to GPIO 21");
        Serial.println("- SCL to GPIO 22");
        Serial.println("- VDD to 3.3V");
        Serial.println("- GND to GND");
        return;
    }
    
    Serial.print("SCD41 serial number: ");
    PrintUint64(serialNumber);
    Serial.println();
    Serial.println("Sensor initialized successfully!");
    Serial.println();
}

void loop() {
    uint16_t co2Concentration = 0;
    float temperature = 0.0;
    float relativeHumidity = 0.0;
    
    // Wake the sensor up from sleep mode
    error = sensor.wakeUp();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute wakeUp(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    
    // Ignore first measurement after wake up
    error = sensor.measureSingleShot();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureSingleShot(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    
    // Perform single shot measurement and read data
    error = sensor.measureAndReadSingleShot(co2Concentration, temperature, relativeHumidity);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureAndReadSingleShot(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    
    // Print results
    Serial.println("=== SCD41 Reading ===");
    Serial.print("CO2: ");
    Serial.print(co2Concentration);
    Serial.println(" ppm");
    
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    
    Serial.print("Humidity: ");
    Serial.print(relativeHumidity);
    Serial.println(" %RH");
    
    // Air quality assessment
    Serial.print("Air Quality: ");
    if (co2Concentration < 400) {
        Serial.println("Outdoor air");
    } else if (co2Concentration < 1000) {
        Serial.println("Good");
    } else if (co2Concentration < 2000) {
        Serial.println("Acceptable");
    } else if (co2Concentration < 5000) {
        Serial.println("Poor - Open windows!");
    } else {
        Serial.println("Very Poor - Ventilate immediately!");
    }
    
    Serial.print("Uptime: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
    Serial.println();
    
    // Wait 30 seconds instead of 5 minutes for more frequent readings
    Serial.println("Waiting 30 seconds for next reading...");
    delay(30000);
}