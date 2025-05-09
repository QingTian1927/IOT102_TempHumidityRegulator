#include <SoftwareSerial.h>
#include <TM1637Display.h>
#include <EEPROM.h>
#include <DHT11.h>

#define ESP_TX 2
#define ESP_RX 3
#define BUTTON_MODE 4
#define BUTTON_BUZZ 5
#define DIO 6
#define CLK 7
#define DHT 8
#define BUZ 9
#define WARNING_LED 10

#define MODE_INTERCHANGE 0
#define MODE_TEMPERATURE 1
#define MODE_HUMIDITY 2

#define TEMP_HARD_LOWER_BOUND 0
#define TEMP_HARD_UPPER_BOUND 50
#define HUMD_HARD_LOWER_BOUND 20
#define HUMD_HARD_UPPER_BOUND 80

#define EEPROM_TEMP_LOWER 0
#define EEPROM_TEMP_UPPER 1
#define EEPROM_HUMD_LOWER 2
#define EEPROM_HUMD_UPPER 3

#define COLON 0b01000000
#define LEADING_ZEROS true
#define NO_LEADING_ZEROS false
#define SHOW_TWO_DIGITS 2
#define ALIGN_LEFT 0
#define ALIGN_RIGHT 2
#define FIRST_DIGIT 1
#define SECOND_DIGIT 2

#define JSON_STATUS_OK "ok"
#define JSON_STATUS_ERROR "error"
#define JSON_STATUS_OUTBOUND "outbound"

const String WIFI_SSID = "TP-Link_69420";
const String WIFI_PASS = "WifiTPLINK1250@";
//const String WIFI_SSID = "NoteX";
//const String WIFI_PASS = "LmaoUrGay1257@";
//const String WIFI_SSID = "WIFI_NIGA";
//const String WIFI_PASS = "linhnga123";
String IP = "";

int mode = 0;   // default mode
int count = 0;  // number of button presses

bool isModeButtonPressed = false;
bool isOn = true;
bool isBuzzButtonPressed = false;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;   // time of the last system update
const long UPDATE_INTERVAL = 1000;  // amount of time between each update

SoftwareSerial esp8266(ESP_TX, ESP_RX);  // arduino RX - TX
TM1637Display display(CLK, DIO);
DHT11 dht11(DHT);

int temperature = 0;
int humidity = 0;

int showTemp = true;
String dht11Error = "";
unsigned long lastReadTime = 0;

int tempUpperBound;
int tempLowerBound;
int humidUpperBound;
int humidLowerBound;

const long BEEP_INTERVAL = 500;
bool buzzerState = false;

const uint8_t SEG_TEMP[] = {
  SEG_A | SEG_B | SEG_G | SEG_F,
  SEG_A | SEG_D | SEG_E | SEG_F
};

const uint8_t SEG_HUMID[] = {
  SEG_C | SEG_D | SEG_E | SEG_G
};

#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587

const int MELODY_NOTES[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_A4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_F4
};
const int MELODY_NUM = sizeof(MELODY_NOTES) / sizeof(int);

const int NOTE_DURATIONS[] = {
  150, 150, 300, 300, 300, 500,
  150, 150, 300, 300, 300, 500,
  150, 150, 300, 300, 300, 300, 500,
  150, 150, 300, 300, 300, 500
};

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);

  display.setBrightness(4);
  display.clear();
  delay(1000);
  initWifiModule();

  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUZ, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(WARNING_LED, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  getBound();

  int buttonState = digitalRead(BUTTON_MODE);
  if (buttonState == LOW) {
    isModeButtonPressed = true;
  }
  if (buttonState == HIGH && isModeButtonPressed) {
    isModeButtonPressed = false;
    count++;
  }
  mode = count % 3;

  int buzzerValue = digitalRead(BUTTON_BUZZ);
  if (buzzerValue == LOW) {
    isBuzzButtonPressed = true;
  }
  if (buzzerValue == HIGH && isBuzzButtonPressed) {
    isBuzzButtonPressed = false;
    isOn = !isOn;
  }

  currentMillis = millis();

  int result;
  if (currentMillis - lastReadTime >= UPDATE_INTERVAL) {  // Ensure at least 1s between reads
    lastReadTime = currentMillis;
    result = dht11.readTemperatureHumidity(temperature, humidity);
    dht11Error = (result == 0) ? "" : DHT11::getErrorString(result);
  }

  bool tempOutOfBounds = (temperature < tempLowerBound || temperature > tempUpperBound);
  bool humidOutOfBounds = (humidity < humidLowerBound || humidity > humidUpperBound);

  // if (isOn && (tempOutOfBounds || humidOutOfBounds)) {
  //   playMelody();
  // }

  switch (mode) {
    case MODE_INTERCHANGE:
      displayInterchange();
      break;
    case MODE_TEMPERATURE:
      displayTemperature();
      break;
    case MODE_HUMIDITY:
      displayHumidity();
      break;
  }

  if (esp8266.available()) {
    if (esp8266.find("+IPD,")) {
      delay(500);
      char c = esp8266.read();
      Serial.println((int)c);

      if (c < '0' || c > '9') {
        c = esp8266.read();
        Serial.println((int)c);
      }
      int connectionId = c - '0';

      String response = esp8266.readStringUntil('H');  // Read string until the H in HTTP/1.1
      esp8266.flush();
      Serial.println(response);

      if (response.indexOf("bounds=") != -1) {
        int index = 0;
        String value = "";

        String params = response.substring(response.indexOf("="));
        Serial.println(params);

        for (int i = 1; i <= response.length(); i++) {
          char c = params.charAt(i);
          Serial.println(value);

          if (c == ',' || c == ' ') {
            setBound(value, index++);
            value = "";
            continue;
          }

          if (c >= '0' && c <= '9') {
            value += c;
          }
        }

        Serial.print(tempLowerBound);
        Serial.print(" ");
        Serial.print(tempUpperBound);
        Serial.print(" - ");
        Serial.print(humidLowerBound);
        Serial.print(" ");
        Serial.println(humidUpperBound);

        sendStatusJSON(connectionId, "ok");
      } else if (response.indexOf("mode=") != -1) {
        String params = response.substring(response.indexOf("="));
        char c = params.charAt(1);

        if (c >= '0' && c <= '9') {
          int modeValue = c - '0';
          if (modeValue >= 0 && modeValue <= 2) {
            count = modeValue;
            Serial.print("Count: ");
            Serial.println(count);

            sendStatusJSON(connectionId, "ok");
          } else {
            sendStatusJSON(connectionId, "error");
          }
        } else {
          sendStatusJSON(connectionId, "error");
        }
      } else {
        sendReadingJSON(connectionId);
      }

      esp8266.flush();
    }
  }
}

void getBound() {
  tempLowerBound = EEPROM.read(EEPROM_TEMP_LOWER);
  if (tempLowerBound == 255) {  // uninitialized memory
    tempLowerBound = TEMP_HARD_LOWER_BOUND;
    EEPROM.write(EEPROM_TEMP_LOWER, tempLowerBound);
  }

  tempUpperBound = EEPROM.read(EEPROM_TEMP_UPPER);
  if (tempUpperBound == 255) {
    tempUpperBound = TEMP_HARD_UPPER_BOUND;
    EEPROM.write(EEPROM_TEMP_UPPER, tempUpperBound);
  }

  humidLowerBound = EEPROM.read(EEPROM_HUMD_LOWER);
  if (humidLowerBound == 255) {
    humidLowerBound = HUMD_HARD_LOWER_BOUND;
    EEPROM.write(EEPROM_HUMD_LOWER, humidLowerBound);
  }

  humidUpperBound = EEPROM.read(EEPROM_HUMD_UPPER);
  if (humidUpperBound == 255) {
    humidUpperBound = HUMD_HARD_UPPER_BOUND;
    EEPROM.write(EEPROM_HUMD_UPPER, humidUpperBound);
  }
}

void setBound(String valueStr, int index) {
  int valueNum = valueStr.toInt();

  Serial.print(valueNum);
  Serial.print(" - ");
  Serial.println(index);

  switch (index) {
    case EEPROM_TEMP_LOWER:
      tempLowerBound = valueNum;
      Serial.println(tempLowerBound);
      EEPROM.write(EEPROM_TEMP_LOWER, tempLowerBound);
      return;
    case EEPROM_TEMP_UPPER:
      tempUpperBound = valueNum;
      Serial.println(tempUpperBound);
      EEPROM.write(EEPROM_TEMP_UPPER, tempUpperBound);
      return;
    case EEPROM_HUMD_LOWER:
      humidLowerBound = valueNum;
      Serial.println(humidLowerBound);
      EEPROM.write(EEPROM_HUMD_LOWER, humidLowerBound);
      return;
    case EEPROM_HUMD_UPPER:
      humidUpperBound = valueNum;
      Serial.println(humidUpperBound);
      EEPROM.write(EEPROM_HUMD_UPPER, humidUpperBound);
      return;
  }
}

void displayTemperature() {
  bool tempOutOfBounds = (temperature < tempLowerBound || temperature > tempUpperBound);
  display.showNumberDec(temperature, NO_LEADING_ZEROS, SHOW_TWO_DIGITS, ALIGN_LEFT);
  display.setSegments(SEG_TEMP, SECOND_DIGIT, 2);
  if ((tempOutOfBounds)) {
    digitalWrite(WARNING_LED, HIGH);
    if (isOn) {
      playMelody();
    }
  } else {
    digitalWrite(WARNING_LED, LOW);
  }
}

void displayHumidity() {
  bool humidOutOfBounds = (humidity < humidLowerBound || humidity > humidUpperBound);
  display.showNumberDec(humidity, NO_LEADING_ZEROS, SHOW_TWO_DIGITS, ALIGN_LEFT);
  display.setSegments(SEG_HUMID, FIRST_DIGIT, 3);
  if ((humidOutOfBounds)) {
    digitalWrite(WARNING_LED, HIGH);
    if (isOn) {
      playMelody();
    }
  } else {
    digitalWrite(WARNING_LED, LOW);
  }
}

void displayInterchange() {
  if (currentMillis - previousMillis >= UPDATE_INTERVAL) {
    previousMillis = currentMillis;
    showTemp = !showTemp;
  }

  if (showTemp) {
    displayTemperature();
  } else {
    displayHumidity();
  }
}

void playMelody() {
  int pressed = false;

  for (int i = 0; i < MELODY_NUM; i++) {
    if (digitalRead(BUTTON_BUZZ) == LOW) {
      pressed = true;
    }

    if (pressed && digitalRead(BUTTON_BUZZ) == HIGH) {
      pressed = false;
      isOn = false;
      return;
    }

    int noteDuration = NOTE_DURATIONS[i];
    tone(BUZ, MELODY_NOTES[i], noteDuration);
    delay(noteDuration * 1.3);
  }
  noTone(BUTTON_BUZZ);
}

String sendCommand(String command, const int timeout) {
  Serial.print("Sending command: ");
  Serial.println(command);
  esp8266.print(command);

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

void sendStatusJSON(int connectionId, String status) {
  digitalWrite(LED_BUILTIN, HIGH);

  String json;
  json.reserve(30);

  json = "{\"status\":\"";
  json += status;
  json += "\"}";

  String httpResponse;
  httpResponse.reserve(150 + json.length());

  httpResponse = "HTTP/1.1 200 OK\r\n";
  httpResponse += "Access-Control-Allow-Origin: *\r\n";                                 // Allow all origins
  httpResponse += "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";  // Allow specific HTTP methods
  httpResponse += "Access-Control-Allow-Headers: Content-Type\r\n";                     // Allow certain headers
  httpResponse += "Connection: close\r\n";
  httpResponse += "Content-Type: application/json\r\n";
  httpResponse += "Content-Length: ";
  httpResponse += String(json.length());
  httpResponse += "\r\n\r\n";
  httpResponse += json;
  httpResponse += "\r\n";

  int contentLength = httpResponse.length();
  String sendCmd = "AT+CIPSEND=";
  sendCmd += String(connectionId);
  sendCmd += ",";
  sendCmd += String(contentLength);
  sendCmd += "\r\n";

  String closeCmd = "AT+CIPCLOSE=";
  closeCmd += String(connectionId);
  closeCmd += "\r\n";

  String sendResult = sendCommand(sendCmd, 1000);
  delay(500);

  if (sendResult.indexOf(">") != 1) {
    esp8266.println(httpResponse);
    delay(2000);
  }

  sendCommand(closeCmd, 1000);
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);
}

void sendReadingJSON(int connectionId) {
  digitalWrite(LED_BUILTIN, HIGH);

  String json;
  json.reserve(100);

  json = "{\"status\":\"";

  if (dht11Error == "") {
    json += String(JSON_STATUS_OK);
    json += "\"";
  } else {
    json += String(JSON_STATUS_ERROR);
    json += "\",\"message\":\"";
    json += dht11Error;
    json += "\"";
  }

  json += ",\"temperature\":";
  json += String(temperature);
  json += ",\"humidity\":";
  json += String(humidity);
  json += "}";

  String httpResponse;
  httpResponse.reserve(150 + json.length());

  httpResponse = "HTTP/1.1 200 OK\r\n";
  httpResponse += "Access-Control-Allow-Origin: *\r\n";                                 // Allow all origins
  httpResponse += "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";  // Allow specific HTTP methods
  httpResponse += "Access-Control-Allow-Headers: Content-Type\r\n";                     // Allow certain headers
  httpResponse += "Connection: close\r\n";
  httpResponse += "Content-Type: application/json\r\n";
  httpResponse += "Content-Length: ";
  httpResponse += String(json.length());
  httpResponse += "\r\n\r\n";
  httpResponse += json;
  httpResponse += "\r\n";

  int contentLength = httpResponse.length();
  String sendCmd = "AT+CIPSEND=";
  sendCmd += String(connectionId);
  sendCmd += ",";
  sendCmd += String(contentLength);
  sendCmd += "\r\n";

  String closeCmd = "AT+CIPCLOSE=";
  closeCmd += String(connectionId);
  closeCmd += "\r\n";

  // Serial.println(httpResponse);
  // Serial.println(sendCmd);
  // Serial.println(closeCmd);

  String sendResult = sendCommand(sendCmd, 1000);
  delay(500);

  if (sendResult.indexOf(">") != 1) {
    esp8266.print(httpResponse);
    delay(2000);
  }

  sendCommand(closeCmd, 1000);
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);
}

String getIP(String espResponseIPString) {
  String ip = espResponseIPString;
  ip.trim();
  ip = ip.substring(ip.indexOf("\"") + 1);
  ip = ip.substring(0, ip.indexOf("\"") - 1);
  return ip;
}

void initWifiModule() {
  sendCommand("AT+RST\r\n", 2000);  // reset the ESP8266-01 module
  delay(1000);

  sendCommand("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + WIFI_PASS + "\"\r\n", 5000);  // connect to the WiFi network
  delay(3000);

  sendCommand("AT+CWMODE=1\r\n", 1500);  // set the ESP8266-01 WiFi mode to station mode
  delay(1000);

  String wifiIP = sendCommand("AT+CIFSR\r\n", 1500);  // show IP address and MAC address of the WiFi module.
  IP = getIP(wifiIP);
  delay(1000);

  sendCommand("AT+CIPMUX=1\r\n", 1500);  // allow multiple (concurrent) connections
  delay(1000);

  sendCommand("AT+CIPSERVER=1,80\r\n", 1500);  // open the server on port 80 (HTTP)
  delay(1000);
}