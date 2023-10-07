#include <Arduino.h>
//Blinky - Hello World

// defines
#define LED1 7 //LED Digital Port 7
#define LED2 8 // LED Digital Port 8

void setup() {
  //Pins Setup
  pinMode(LED1,OUTPUT);
  digitalWrite(LED1, LOW);
  pinMode(LED2,OUTPUT);
  digitalWrite(LED2, LOW);
}

void loop() {
    //HELLO WORLD
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,LOW);
    delay(100);
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,HIGH);
    delay(100);

}
