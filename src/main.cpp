#include <Arduino.h>
/* Commands
  a: change LED1
  s: change LED2
  r: change relay
  d: change LEDs and Relay
*/


// defines
#define LED1 7 //LED Digital Port 7
#define LED2 8 // LED Digital Port 8
#define BUTTON1 9 // Button Digital Port 9
#define RELAY 10

// Variables will change:
int led1State = LOW;        // the current state of the output pin
int led2State = HIGH;
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin
int relayState = LOW;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  //output
  pinMode(LED1,OUTPUT);
  digitalWrite(LED1, LOW);
  pinMode(LED2,OUTPUT);
  digitalWrite(LED2, LOW);
  pinMode(RELAY,OUTPUT);
  digitalWrite(RELAY, LOW);
  //input
  pinMode(BUTTON1, INPUT);


  //serial
  Serial.begin(9600);
  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }
  Serial.println("Setup...");

}

void loop() {
  //SERIAL
  char incomingByte=0;
  // reply only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    if (incomingByte == 'a') // a = change LED1 state
    {
      led1State = !led1State;
      Serial.println("LED1 state changed");
    }
    else if (incomingByte == 's') // s = change LED2 state
    {
      led2State = !led2State;
      Serial.println("LED2 state changed");
    }
    else if (incomingByte == 'r') // relay control
    {
      relayState = !relayState;
      Serial.println("Relay state changed");
    }
       else if (incomingByte == 'd') // change 3 states
    {
      relayState = !relayState;
      led1State = !led1State;
      led2State = !led2State;
      Serial.println("Changed LED1 LED2 RELAY");
    }
    else
      Serial.println("Do Nothing");

  }
  // DEBOUNCE
  int reading = digitalRead(BUTTON1);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        led1State = !led1State;
        led2State = !led2State;
        relayState = !relayState;
        Serial.println("Button pressed");
      }
    }
  }

  // set the LED:
  digitalWrite(LED1, led1State);
  digitalWrite(LED2, led2State);
  digitalWrite(RELAY,relayState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
