#include <Arduino.h>
// defines
#define LED1 7 //LED Digital Port 7
#define BLINK 250

unsigned long blink_timer;
unsigned int led_state;
void setup() {
  //Pins Setup
  pinMode(LED1,OUTPUT);
  digitalWrite(LED1, LOW);
  blink_timer = millis() + BLINK;
  led_state = 1;
}

void loop() {
  //HELLO WORLD
  if(millis() > blink_timer)
  {
    digitalWrite(LED1, led_state);
    led_state = !led_state;
    blink_timer = millis() + BLINK;
  }

}
