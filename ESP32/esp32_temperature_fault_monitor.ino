/*
 * ESP32 Temperature & Connection Fault Monitor
 * Project: AI-Assisted Embedded Fault Detection
 *
 * Reads an analog temperature sensor (LM35-style 10 mV/C output),
 * checks for open/short/out-of-range conditions, and sends structured
 * telemetry over Serial for the Python AI diagnostic layer.
 *
 * Default sensor input: GPIO34 (ADC1_CH6)
 * Change TEMP_SENSOR_PIN if your hardware uses another ADC pin.
 */

#include <Arduino.h>

// ESP32 analog temperature/fault monitor.
// Default LM35-style sensor input: GPIO34 (ADC1_CH6) on classic ESP32.
const uint8_t TEMP_SENSOR_PIN = 34;
const uint16_t ADC_MAX = 4095;
const float SENSOR_MV_PER_C = 10.0f; // LM35: 10 mV per degree C.

// Prototype incubator limits. Calibrate these experimentally.
const float MIN_VALID_TEMP_C = 0.0f;
const float MAX_VALID_TEMP_C = 80.0f;
const float LOW_TEMP_ALARM_C = 35.0f;
const float HIGH_TEMP_ALARM_C = 40.0f;

const uint8_t SAMPLE_COUNT = 10;
const uint16_t SAMPLE_DELAY_MS = 5;

struct TemperatureReading {
  uint16_t raw;
  uint32_t millivolts;
  float temperatureC;
};

TemperatureReading readTemperature() {
  uint32_t rawTotal = 0;
  uint32_t mvTotal = 0;

  for (uint8_t i = 0; i < SAMPLE_COUNT; ++i) {
    rawTotal += analogRead(TEMP_SENSOR_PIN);
    mvTotal += analogReadMilliVolts(TEMP_SENSOR_PIN);
    delay(SAMPLE_DELAY_MS);
  }

  TemperatureReading reading{};
  reading.raw = static_cast<uint16_t>(rawTotal / SAMPLE_COUNT);
  reading.millivolts = mvTotal / SAMPLE_COUNT;
  reading.temperatureC = reading.millivolts / SENSOR_MV_PER_C;
  return reading;
}

const char* classifyFault(const TemperatureReading& reading) {
  // These raw-value checks are prototype indicators only. A robust
  // open-sensor diagnostic needs appropriate hardware validation.
  if (reading.raw <= 5) {
    return "SENSOR_LOW_OR_DISCONNECTED";
  }

  if (reading.raw >= ADC_MAX - 5) {
    return "SENSOR_HIGH_OR_SHORT";
  }

  if (reading.temperatureC < MIN_VALID_TEMP_C ||
      reading.temperatureC > MAX_VALID_TEMP_C) {
    return "TEMPERATURE_OUT_OF_RANGE";
  }

  if (reading.temperatureC < LOW_TEMP_ALARM_C) {
    return "LOW_TEMPERATURE";
  }

  if (reading.temperatureC > HIGH_TEMP_ALARM_C) {
    return "HIGH_TEMPERATURE";
  }

  return "NORMAL";
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(TEMP_SENSOR_PIN, INPUT);

  // GPIO34 is ADC1 on the classic ESP32. 0 dB is suitable for the
  // low-voltage LM35 range used by this prototype; change this setting
  // if your sensor/interface produces a higher voltage.
  analogSetPinAttenuation(TEMP_SENSOR_PIN, ADC_0db);

  delay(1000);
  Serial.println("ESP32_FAULT_MONITOR_READY");
}

void loop() {
  const TemperatureReading reading = readTemperature();
  const char* fault = classifyFault(reading);

  Serial.printf(
      "TEMP,%.2f,RAW,%u,FAULT,%s\n",
      reading.temperatureC,
      reading.raw,
      fault);

  delay(1000);
}
