#include <SoftwareSerial.h>
#include <TM1637Display.h>
#include <EEPROM.h>

#define ESP_TX 2
#define ESP_RX 3
#define BUTTON_MODE 4
#define BUTTON_ADJUST 5
#define DIO 6
#define CLK 7

#define EEPROM_MIN 0
#define EEPROM_HOUR 1

#define MODE_CLOCK 0
#define MODE_ADJUST_MINUTE 1
#define MODE_ADJUST_HOUR 2

#define COLON 0b01000000
#define LEADING_ZEROS true
#define NO_LEADING_ZEROS false
#define SHOW_TWO_DIGITS 2
#define ALIGN_HOUR 0
#define ALIGN_MINUTE 2

const String WIFI_SSID = "TP-Link_69420";
const String WIFI_PASS = "WifiTPLINK1250@";

const uint8_t SEG_TEMPERATURE[] = {
  SEG_A | SEG_B | SEG_G | SEG_F,  // symbol degree
  SEG_A | SEG_D | SEG_E | SEG_F   // C celsius
};

SoftwareSerial esp8266(ESP_TX, ESP_RX);  // arduino RX - TX
TM1637Display display(CLK, DIO);

int mode = 0;   // default mode
int count = 0;  // number of button presses

bool isPressed = false;
bool isClear;

int min;
int hour;
unsigned long previousMillis = 0;   // last time clock display was updated
const long UPDATE_INTERVAL = 1000;  // amount of time between each display update

void setup() {
  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_ADJUST, INPUT_PULLUP);

  Serial.begin(9600);
  esp8266.begin(9600);

  display.setBrightness(4);
  display.clear();
  delay(1000);
  initWifiModule();
}

void loop() {
  min = EEPROM.read(EEPROM_MIN);
  hour = EEPROM.read(EEPROM_HOUR);
  int time = hour * 100 + min;

  int buttonState = digitalRead(BUTTON_MODE);
  // press and release button logic

  if (buttonState == 0) {
    isPressed = true;
    delay(10);
  }
  if (buttonState == 1 && isPressed) {
    isPressed = false;
    count++;
  }

  if (count % 3 == 0) {
    mode = MODE_CLOCK;
  } else if ((count - 1) % 3 == 0) {
    mode = MODE_ADJUST_MINUTE;
  } else {
    mode = MODE_ADJUST_HOUR;
  }
  Serial.print("Mode: ");
  Serial.println(mode);

  if (mode == MODE_CLOCK) {
    runClock(time);
  }

  int readValue = digitalRead(BUTTON_ADJUST);
  switch (mode) {
    case MODE_ADJUST_MINUTE:
      adjustMinute(readValue);
      break;
    case MODE_ADJUST_HOUR:
      adjustHour(readValue);
      break;
  }
}

void runClock(int time) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= UPDATE_INTERVAL) {
    previousMillis = currentMillis;
    display.showNumberDecEx(time, COLON, LEADING_ZEROS);
    min++;

    if (min >= 60) {
      min = 0;
      hour++;
    }

    if (hour >= 24) {
      hour = 0;
    }

    EEPROM.write(EEPROM_MIN, min);
    EEPROM.write(EEPROM_HOUR, hour);
  }
}

void adjustMinute(int readValue) {
  display.clear();
  display.showNumberDec(min, LEADING_ZEROS, SHOW_TWO_DIGITS, ALIGN_MINUTE);

  if (readValue == 0) {
    Serial.print("readValue: ");
    Serial.println(readValue);

    min++;
    if (min >= 60) {
      min = 0;
    }

    delay(250);
    EEPROM.write(EEPROM_MIN, min);
    display.clear();
  }
}

void adjustHour(int readValue) {
  display.clear();
  display.showNumberDec(hour, LEADING_ZEROS, SHOW_TWO_DIGITS, ALIGN_HOUR);

  if (readValue == 0) {
    Serial.print("readValue: ");
    Serial.println(readValue);

    hour++;
    if (min >= 24) {
      hour = 0;
    }

    delay(250);
    EEPROM.write(EEPROM_HOUR, hour);
    display.clear();
  }
}

String sendData(String command, const int timeout) {
  Serial.print("Sending command: ");
  Serial.println(command);
  esp8266.print(command + "\r\n");

  String response = "";
  long int time = millis();

  while ((time + timeout) > millis()) {
    while (esp8266.available()) {
      char c = esp8266.read();
      response += c;
    }
  }

  Serial.println(response);
  return response;
}

void initWifiModule() {
  sendData("AT+RST", 2000);  // reset the ESP8266-01 module
  delay(1000);
  sendData("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + WIFI_PASS + "\"", 5000);  // connect to the WiFi network
  delay(3000);
  sendData("AT+CWMODE=1", 1500);  // set the ESP8266-01 WiFi mode to station mode
  delay(1000);
  sendData("AT+CIFSR", 1500);  // show IP address and MAC address of the WiFi module.
  delay(1000);
  sendData("AT+CIPMUX=1", 1500);  // allow multiple (concurrent) connections
  delay(1000);
  sendData("AT+CIPSERVER=1,80", 1500);  // open the server on port 80 (HTTP)
  delay(1000);
}
