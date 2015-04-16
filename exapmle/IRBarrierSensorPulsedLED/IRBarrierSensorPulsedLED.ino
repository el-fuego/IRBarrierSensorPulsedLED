#include <IRBarrierSensorPulsedLED.h>

// setup pins
barrierSensorPin = A3;
barrierLEDPin = 9;

void setup() {
  // declare the LED pins as OUTPUTs:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(barrierLEDPin, OUTPUT);

  // setup serial output
  Serial.begin(9600);
}

IRBarrierSensorPulsedLED barrierSensor({barrierSensorPin, barrierLEDPin});

void loop() {
  // turn ON builtin LED if barrier is detected
  digitalWrite(LED_BUILTIN, barrierSensor.hasBarrier() ? HIGH : LOW);
  
  // print current deviation
  Serial.writeln(barrierSensor.getRaw());
  delay(100);
}
