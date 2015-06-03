// Core Arduino Libraries
#include <SPI.h>

// User libraries
#include "I2C.h"
#include "Adafruit_BNO055_Mod.h"
#include "Adafruit_LSM9DS0_Mod.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_Simple_AHRS.h"
#include "imumaths.h"

// Delay between samplings in milliseconds
#define SAMPLE_PERIOD_MS 250

// Decimal precision of the samples
#define SAMPLE_PRECISION 4

// IDs for the chips
#define LSM_ID 1
#define BNO_ID 2

// Instantiate a model of the lsm9ds0 chip and then wrap it in an AHRS instance
// to allow for easy querying of roll/pitch/yaw
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(LSM_ID);
Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());

// Instantiate a model of the bno055 chip with the provided id.
// Can actually pass (sensorId, address) for this one (unlike previous ones)
Adafruit_BNO055 bno = Adafruit_BNO055(BNO_ID);

// Main loops variables for holding sensor samplings.
sensors_event_t sampleBno;
sensors_vec_t sampleLsm;
unsigned long sampleTimestamp;

/**
 * Initialization function of the firmware.
 */
void setup() {
  Serial.begin(115200);

  // Ensure that we are able to communicate with the lsm chip
  if (!lsm.begin()) {
    Serial.println(F("Couldn't initialize lsm9ds0. Wiring issues probably"));
    delay(500); // allow serial data to drain
    exit(1);
  }

  // Ensure that we are able to communicate with the bno chip
  if (!bno.begin()) {
    Serial.println(F("Couldn't initialize bno055. Wiring issues probably"));
    delay(500); // allow serial data to drain
    exit(1);
  }

  // Use the external crystal for improved accuracy
  bno.setExtCrystalUse(true);
}

/**
 * Main loop of the Arduino firmware.
 */
void loop() {
  // Take sensor readings
  sampleTimestamp = millis();
  bno.getEvent(&sampleBno);
  ahrs.getOrientation(&sampleLsm);

  // Send sensor readings over serial
  sendBnoSampleOverSerial(&sampleBno, sampleTimestamp);
  sendLsmSampleOverSerial(&sampleLsm, sampleTimestamp);

  // Wait until next time slice to take readings again.
  delay(SAMPLE_PERIOD_MS);
}

/**
 * Serializes the BNO sensor sample according to the API, and sends it over the
 * Serial connection.
 */
void sendBnoSampleOverSerial(sensors_event_t* event, unsigned long timestamp) {
  Serial.print("id ");
  Serial.print(event->sensor_id);
  Serial.print(" ");

  Serial.print("time ");
  Serial.print(event->timestamp);
  Serial.print(" ");

  Serial.print("x ");
  Serial.print(event->orientation.x, SAMPLE_PRECISION);
  Serial.print(" ");

  Serial.print("y ");
  Serial.print(event->orientation.y, SAMPLE_PRECISION);
  Serial.print(" ");

  Serial.print("z ");
  Serial.print(event->orientation.z, SAMPLE_PRECISION);
  Serial.print(" ");

  Serial.println("");
  Serial.println("$"); // message-end sentinel ("$\n")
}

/**
 * Serializes the LSM sensor sample according to the API, and sends it over the
 * Serial connection.
 */
void sendLsmSampleOverSerial(sensors_vec_t* orientation, unsigned long timestamp) {
  Serial.print("id ");
  Serial.print(LSM_ID);
  Serial.print(" ");

  Serial.print("time ");
  Serial.print(timestamp);
  Serial.print(" ");

  Serial.print("x ");
  Serial.print(orientation->roll, SAMPLE_PRECISION);
  Serial.print(" ");

  Serial.print("y ");
  Serial.print(orientation->pitch, SAMPLE_PRECISION);
  Serial.print(" ");

  Serial.print("z ");
  Serial.print(orientation->heading, SAMPLE_PRECISION);
  Serial.print(" ");

  Serial.println("");
  Serial.println("$"); // message-end sentinel ("$\n")
}
