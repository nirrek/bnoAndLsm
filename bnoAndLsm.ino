// Core Arduino Libraries
#include <Wire.h>
#include <SPI.h>

// 3rd Party Libraries
#include <I2C.h>
#include <Adafruit_LSM9DS0_Mod.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_Simple_AHRS.h>

// Delay between samplings in milliseconds
#define SAMPLE_PERIOD_MS 250

// Decimal precision of the samples
#define SAMPLE_PRECISION 4

// Instantiate a model of the lsm9ds0 chip with the provided id.
int32_t lsmId = 1;
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(lsmId);

// Instantiate an ahrs from the lsm chip
Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());

// Instantiate a model of the bno055 chip with the provided id.
// Can actually pass (sensorId, address) for this one (unlike previous ones)
int32_t bnoId = 2;
Adafruit_BNO055 bno = Adafruit_BNO055(bnoId);

void setup() {
  Serial.begin(115200);

  // Ensure that we are able to communicate with the lsm chip
  if (!lsm.begin()) {
    Serial.println(F("Couldn't initialize lsm9ds0. Wiring issues probably"));
    delay(500); // allow serial data to drain
    exit(1);
  }

  if (!bno.begin()) {
    Serial.println(F("Couldn't initialize bno055. Wiring issues probably"));
    delay(500); // allow serial data to drain
    exit(1);
  }

  bno.setExtCrystalUse(true);

}

sensors_event_t sampleBno;
sensors_vec_t sampleLsm;
unsigned long sampleTimestamp;

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
 * Serializes the sensor sample according to the API, and sends it over the
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

void sendLsmSampleOverSerial(sensors_vec_t* orientation, unsigned long timestamp) {
  Serial.print("id ");
  Serial.print(lsmId);
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
