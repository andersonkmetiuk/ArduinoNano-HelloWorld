# Initial development for Arduino Nano

 <div><img src= assets/arduino-nano.svg width=300 align="left" /></div>

|          **Board**          |          Arduino Nano            |
|:---------------------------:|:--------------------------------:|
|        **Microcontroller**  |          ATmega328               |
|          **USB connector**  |         Mini-B USB               |
|     **Built-in LED Pin**    |                13                |
|     **Digital I/O Pins**    |                14                |
|    **Analog input pins**    |                 8                |
|         **PWM pins**        |                 6                |
|           **UART**          |                RX/TX             |
|           **I2C**           |                A4(SDA), A5(SCL)  |
|           **SPI**           | D11 (COPI), D12 (CIPO), D13 (SCK)|
|       **I/O Voltage**       |                5V                |
| **Input voltage (nominal)** |               7-12V              |
|  **DC Current per I/O Pin** |               20 mA              |
|      **Processor**          |         ATmega328 16 MHz         |
|        **Memory**           | 2KB SRAM, 32KB FLASH, 1KB EEPROM |

---

 <div><img src= assets/Arduino-Nano-Complete-Pinout.png /></div> 

---
## Arduino Nano

Documentation &rarr; [Arduino Nano](https://docs.arduino.cc/hardware/nano)

Tutorials &rarr; [Built-in Examples](https://docs.arduino.cc/built-in-examples/)

Configuring Visual Studio with [PlatformIO Extension](https://platformio.org/install/ide?install=vscode). Remember to set the **platform.ini** file with:
```
[env:nanoatmega328]
platform = atmelavr
board = nanoatmega328new
framework = arduino
```
## Simple Projects
### Branch: Serial-Relay-Leds-Button
This program tests commands from the Serial and Button to control 2 LED and a 5V Relay Module (jqc3f-5vdc-c).

----
<img src=https://www.usinainfo.com.br/1016975-thickbox_default/modulo-rele-5v-10a-1-canal-jqc3f-para-arduino-e-esp32.jpg width="300"/>

```
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
```

### Branch: Watchdog
The Watchdog Timer is a useful feature to help the system recover from scenarios where the system hangs or freezes due to errors in the code written or due to conditions that may arise due to hardware issues.

When enabled, it starts counting from 0 to a value selected by the user. If the watchdog timer is not reset by the time it reaches the user selected value, the watchdog resets the microcontroller.

The ATmega328P watchdog timer can be configured for 10 different time settings (the time after which the watchdog timer overflows, thus causing a reset). The various times are : 16ms, 32ms, 64ms, 0.125s, 0.25s, 0.5s, 1s, 2s, 4s and 8s.

Here, we will use a simple example of LED blinking.

The LEDs are blinked for a certain time before entering a while(1) loop. The while(1) loop is used as a substitute for a system in the hanged state.

Since the watchdog timer is not reset when in the while(1) loop, the watchdog causes a system reset and the LEDs start blinking again before the system hangs and restarts again. This continues in a loop.

### Word of caution :
---
The Watchdog timer is disabled at the start of the code. A delay of 3 seconds is used before enabling the Watchdog.

This delay is important in order to let the bootloader in Arduino to check if a new code is being uploaded and to give it time to burn the code into the flash.

This is important as a precaution. A situation may occur, wherein due to faulty coding, or improper considerations; the code written resets the microcontroller at very short durations infinitely. 

This will damage the Arduino board and lead to sketches not being uploaded to the board.

This may not be the case for the new Optiboot loader that comes with the newer version of the Arduino, but it will definitely happen to the older ones.

In case if you break the Arduino in this manner, you will have to burn the bootloader using a different Arduino as an ISP into the bricked Arduino.

```
#include <Arduino.h>
#include <avr/wdt.h> /* Header for watchdog timers in AVR */

#define TESTPIN 7 //choose the pin you want to test it

void setup() {
  Serial.begin(9600); /* Define baud rate for serial communication */
  Serial.println("Watchdog Demo Starting");
  pinMode(TESTPIN, OUTPUT);
  wdt_disable();  /* Disable the watchdog and wait for more than 2 seconds */
  delay(3000);  /* Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration */
  wdt_enable(WDTO_2S);  /* Enable the watchdog with a timeout of 2 seconds */
}

void loop() {
  for(int i = 0; i<20; i++) /* Blink LED for some time */
  {
    digitalWrite(TESTPIN, HIGH);
    delay(100);
    digitalWrite(TESTPIN, LOW);
    delay(100);
    wdt_reset();  /* Reset the watchdog */
  }
  while(1); /* Infinite loop. Will cause watchdog timeout and system reset. */
}
```
