
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "IRBarrierSensorPulsedLED.h"

IRBarrierSensorPulsedLED::IRBarrierSensorPulsedLED(Pins _pins, float _sensivityLevel, int _measurementTimeMicroSeconds, float _calibrationLevel) : pins(_pins), sensivityLevel(_sensivityLevel), measurementTimeMicroSeconds(_measurementTimeMicroSeconds), calibrationLevel(_calibrationLevel) {}

// Sensor values with LED turned off and on
IRBarrierSensorPulsedLED::SensorValues IRBarrierSensorPulsedLED::readValues() {
  SensorValues values;
  // Add delay for short calls to turn off led (it is not high-speed)
  if ( lastLedHighlightAt + measurementTimeMicroSeconds > micros()) {
    delayMicroseconds(measurementTimeMicroSeconds);
  }
  values.dark = analogRead(pins.sensor);
  digitalWrite(pins.led, HIGH);
  delayMicroseconds(measurementTimeMicroSeconds);
  values.light = analogRead(pins.sensor);
  digitalWrite(pins.led, LOW);
  
  lastLedHighlightAt = millis();
  
  return values;
}

// Deviation of lightness to determine barrier
float IRBarrierSensorPulsedLED::lightValuesDeviation(SensorValues values) {
  float correction = (defaultValues.dark - values.dark) * defaultValues.light / defaultValues.dark;
  return 1 - (values.light / (defaultValues.light - correction));
}

// Is sensor already calibrated? For first run
bool IRBarrierSensorPulsedLED::isCalibrated () {
  return defaultValues.dark != 0;
}

// Is need runtime calibration
// true if default and current sensor values is differs more than calibrationLevel
bool IRBarrierSensorPulsedLED::isNeedCalibration (SensorValues values) {
  return abs(defaultValues.dark - values.dark) / defaultValues.dark > calibrationLevel;
}

// Set defaultValues
void IRBarrierSensorPulsedLED::calibrateSensor (SensorValues values) {
  defaultValues = values;
  Serial.println("Calibrated");
}

// Calibrate sensor to current lightness
void IRBarrierSensorPulsedLED::calibrate () {
  calibrateSensor(readValues());
}

// Deviation of lightness to determine barrier.
// Calibrate sensors automatically if calibrationLevel between values with offed LED is reached
float IRBarrierSensorPulsedLED::getRaw () {
  SensorValues values = readValues();
    
  // calibrate sensor on power on
  if (!isCalibrated()) {
    calibrateSensor(values);
  }
  float deviation = lightValuesDeviation(values);
  
  Serial.print("light: "); Serial.print(values.light);Serial.print(" / "); Serial.print(defaultValues.light);
  Serial.print(" ; dark: "); Serial.print(values.dark);Serial.print(" / "); Serial.print(defaultValues.dark);
  Serial.print(" ; deviation: "); Serial.println(deviation);
  
  bool hasBarrier = deviation > sensivityLevel;
  
  // calibrate sensor on lightness changes
  // do not calibrate near to barrier 
  if (!hasBarrier && isNeedCalibration(values)) {
    calibrateSensor(values);
  }
  
  return deviation;
}

// Is barrier determined by sensivityLevel? Use getRaw()
bool IRBarrierSensorPulsedLED::hasBarrier () {
  return getRaw() > sensivityLevel;
}
