#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  Serial.println("Begin...");
}

void loop() {
 Serial.println("A"); //Sends data to the ESP32 through UART
 delay(5000);
}