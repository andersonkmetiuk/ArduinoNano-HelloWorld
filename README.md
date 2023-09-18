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
For Linux you might need to run this command
```
sudo apt-get install python3-venv
```
[Here's why](https://github.com/platformio/platformio-core-installer/issues/1774)

---

## Simple Projects
### Branch: Serial-Relay-Leds-Button
This program tests commands from the Serial and Button to control 2 LED and a 5V Relay Module (jqc3f-5vdc-c).

----
<img src=assets/relay-module-jqc3f-5vdc-c.jpg width="30%"/>

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

### Branch: Ethernet

In PlatformIO to add an external library we need to add it like this:
```
[env:nanoatmega328]
platform = atmelavr
board = nanoatmega328new
framework = arduino
lib_deps = uipethernet/UIPEthernet@^2.0.12
```

For this example we will use the **UIPEthernet.h**
For this example you will also need the **ENC28J60** ethernet module.

<img src=assets/ENC28J60.jpg width="30%" />

```
#include <Arduino.h>
#include <SPI.h>
#include <UIPEthernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //ATRIBUIÇÃO DE ENDEREÇO MAC AO ENC28J60
byte ip[] = { 192, 168, 0, 175 }; //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110  **** ISSO VARIA, NO MEU CASO É: 192.168.0.175
EthernetServer server(80); //PORTA EM QUE A CONEXÃO SERÁ FEITA

int ledPin = 8; //PINO DIGITAL UTILIZADO PELO LED
String readString = String(30); //VARIÁVEL PARA BUSCAR DADOS NO ENDEREÇO (URL)
int status = 0; //DECLARAÇÃO DE VARIÁVEL DO TIPO INTEIRA(SERÁ RESPONSÁVEL POR VERIFICAR O STATUS ATUAL DO LED)

void setup(){
  Ethernet.begin(mac, ip); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI FAZER A CONEXÃO COM A REDE
  server.begin(); //INICIA O SERVIDOR PARA RECEBER DADOS NA PORTA 80
  pinMode(ledPin, OUTPUT); //DEFINE O PINO COMO SAÍDA
  digitalWrite(ledPin, LOW); //LED INICIA DESLIGADO
  }
void loop(){
EthernetClient client = server.available(); //CRIA UMA CONEXÃO COM O CLIENTE
  if (client) { // SE EXISTE CLIENTE FAZ
    while (client.connected()) {//ENQUANTO EXISTIR CLIENTE CONECTADO, FAZ
   if (client.available()) { //SE O CLIENTE ESTÁ HABILITADO, FAZ
    char c = client.read(); //LÊ CARACTER A CARACTER DA REQUISIÇÃO HTTP
    if (readString.length() < 100) //SE O ARRAY FOR MENOR QUE 100, FAZ
      {
        readString += c; // "readstring" VAI RECEBER OS CARACTERES LIDO
      }
        if (c == '\n') { //SE ENCONTRAR "\n" É O FINAL DO CABEÇALHO DA REQUISIÇÃO HTTP, FAZ
          if (readString.indexOf("?") <0){ //SE ENCONTRAR O CARACTER "?", FAZ
          }
          else //SENÃO, FAZ
        if(readString.indexOf("ledParam=1") >0){ //SE ENCONTRAR O PARÂMETRO "ledParam=1", FAZ
             digitalWrite(ledPin, HIGH); //LIGA O LED
             status = 1; //VARIÁVEL RECEBE VALOR 1(SIGNIFICA QUE O LED ESTÁ LIGADO)
           }else{ //SENÃO, FAZ
             digitalWrite(ledPin, LOW); //DESLIGA O LED
             status = 0; //VARIÁVEL RECEBE VALOR 0(SIGNIFICA QUE O LED ESTÁ DESLIGADO)
           }
          client.println("HTTP/1.1 200 OK"); //ESCREVE PARA O CLIENTE A VERSÃO DO HTTP
          client.println("Content-Type: text/html"); //ESCREVE PARA O CLIENTE O TIPO DE CONTEÚDO(texto/html)
          client.println();
          //AS LINHAS ABAIXO CRIAM A PÁGINA HTML
          client.println("<body style=background-color:#ADD8E6>"); //DEFINE A COR DE FUNDO DA PÁGINA
          client.println("<center><font color='blue'><h1>MASTERWALKER SHOP</font></center></h1>"); //ESCREVE "MASTERWALKER SHOP" NA PÁGINA
          client.println("<h1><center>CONTROLE DE LED</center></h1>"); //ESCREVE "CONTROLE DE LED" NA PÁGINA
          if (status == 1){ //SE VARIÁVEL FOR IGUAL A 1, FAZ
          //A LINHA ABAIXO CRIA UM FORMULÁRIO CONTENDO UMA ENTRADA INVISÍVEL(hidden) COM O PARÂMETRO DA URL E CRIA UM BOTÃO APAGAR (CASO O LED ESTEJA LIGADO)
          client.println("<center><form method=get name=LED><input type=hidden name=ledParam value=0 /><input type=submit value=APAGAR></form></center>");
          }else{ //SENÃO, FAZ
          //A LINHA ABAIXO CRIA UM FORMULÁRIO CONTENDO UMA ENTRADA INVISÍVEL(hidden) COM O PARÂMETRO DA URL E CRIA UM BOTÃO ACENDER (CASO O LED ESTEJA DESLIGADO)
          client.println("<center><form method=get name=LED><input type=hidden name=ledParam value=1 /><input type=submit value=ACENDER></form></center>");
          }
          client.println("<center><font size='5'>Status atual do LED: </center>"); //ESCREVE "Status atual do LED:" NA PÁGINA
          if (status == 1){ //SE VARIÁVEL FOR IGUAL A 1, FAZ
              client.println("<center><font color='green' size='5'>LIGADO</center>"); //ESCREVE "LIGADO" EM COR VERDE NA PÁGINA
          }else{ //SENÃO, FAZ
              client.println("<center><font color='red' size='5'>DESLIGADO</center>"); //ESCREVE "DESLIGADO" EM COR VERMELHA NA PÁGINA
          }
          client.println("<hr />"); //TAG HTML QUE CRIA UMA LINHA HORIZONTAL NA PÁGINA
          client.println("<hr />"); //TAG HTML QUE CRIA UMA LINHA HORIZONTAL NA PÁGINA
          client.println("</body></html>"); //FINALIZA A TAG "body" E "html"
          readString=""; //A VARIÁVEL É REINICIALIZADA
          client.stop(); //FINALIZA A REQUISIÇÃO HTTP E DESCONECTA O CLIENTE
            }
          }
        }
      }
 }
```
[Source](https://blogmasterwalkershop.com.br/arduino/como-usar-com-arduino-modulo-ethernet-enc28j60-web-server) of this example.

### Branch: Ethercard-BackSoon
Ethercard backSoon example
```
// Present a "Will be back soon web page", as stand-in webserver.
// 2011-01-30 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
#include <Arduino.h>
#include <EtherCard.h>

#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 192,168,0,200 };
// gateway ip address
static byte gwip[] = { 192,168,0,1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

const char page[] PROGMEM =
"HTTP/1.0 503 Service Unavailable\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
  "<head><title>"
    "Service Temporarily Unavailable"
  "</title></head>"
  "<body>"
    "<h3>This service is currently unavailable</h3>"
    "<p><em>"
      "The main server is currently off-line.<br />"
      "Please try again later."
    "</em></p>"
  "</body>"
"</html>"
;

void setup(){
  Serial.begin(57600);
  Serial.println("\n[backSoon]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
}

void loop(){
  // wait for an incoming TCP packet, but ignore its contents
  if (ether.packetLoop(ether.packetReceive())) {
    memcpy_P(ether.tcpOffset(), page, sizeof page);
    ether.httpServerReply(sizeof page - 1);
  }
}

```