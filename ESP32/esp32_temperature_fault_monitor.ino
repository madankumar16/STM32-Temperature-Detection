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

const int TEMP_SENSOR_PIN = 34;
const float ADC_REFERENCE_V = 3.3f;
const int ADC_MAX = 4095;
const float SENSOR_MV_PER_C = 10.0f;

// Incubator operating limits; tune these after real measurements.
const float MIN_VALID_TEMP_C = 0.0f;
const float MAX_VALID_TEMP_C = 80.0f;
const float LOW_TEMP_ALARM_C = 35.0f;
const float HIGH_TEMP_ALARM_C = 40.0f;

float readTemperatureC() {
  const int samples = 10;
  long total = 0;
  for (int i = 0; i < samples; ++i) {
    total += analogRead(TEMP_SENSOR_PIN);
    delay(5);
  }

  float raw = total / (float)samples;
  float voltage = (raw / ADC_MAX) * ADC_REFERENCE_V;
  return (voltage * 1000.0f) / SENSOR_MV_PER_C;
}

String classifyFault(float temperatureC, int raw) {
  if (raw <= 5) return "SENSOR_LOW_OR_DISCONNECTED";
  if (raw >= ADC_MAX - 5) return "SENSOR_HIGH_OR_SHORT";
  if (temperatureC < MIN_VALID_TEMP_C || temperatureC > MAX_VALID_TEMP_C)
    return "TEMPERATURE_OUT_OF_RANGE";
  if (temperatureC < LOW_TEMP_ALARM_C) return "LOW_TEMPERATURE";
  if (temperatureC > HIGH_TEMP_ALARM_C) return "HIGH_TEMPERATURE";
  return "NORMAL";
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(TEMP_SENSOR_PIN, INPUT);
  delay(1000);
  Serial.println("ESP32_FAULT_MONITOR_READY");
}

void loop() {
  int raw = analogRead(TEMP_SENSOR_PIN);
  float temperatureC = readTemperatureC();
  String fault = classifyFault(temperatureC, raw);

  // Machine-readable CSV telemetry for Python.
  Serial.printf("TEMP,%.2f,RAW,%d,FAULT,%s\n", temperatureC, raw, fault.c_str());

  delay(1000);
}
