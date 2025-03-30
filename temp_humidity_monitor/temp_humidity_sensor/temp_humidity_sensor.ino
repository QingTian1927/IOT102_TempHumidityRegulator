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

#define MODE_INTERCHANGE 0
#define MODE_TEMPERATURE 1
#define MODE_HUMIDITY 2

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

int tempUpperBound = 90;
int tempLowerBound = 80;
int humidUpperBound = 90;
int humidLowerBound = 80;

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

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
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

  if (isOn && (tempOutOfBounds || humidOutOfBounds)) {
    playMelody();
  }

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
      char c = esp8266.read();
      Serial.println((int)c);

      if (c < '0' || c > '9') {
        c = esp8266.read();
        Serial.println((int)c);
      }

      int connectionId = c - '0';
      sendJSON(connectionId);
    }
  }
}

void displayTemperature() {
  display.showNumberDec(temperature, NO_LEADING_ZEROS, SHOW_TWO_DIGITS, ALIGN_LEFT);
  display.setSegments(SEG_TEMP, SECOND_DIGIT, 2);
}

void displayHumidity() {
  display.showNumberDec(humidity, NO_LEADING_ZEROS, SHOW_TWO_DIGITS, ALIGN_LEFT);
  display.setSegments(SEG_HUMID, FIRST_DIGIT, 3);
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

void sendJSON(int connectionId) {
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
  httpResponse += "Content-Type: application/json\r\n";  // Your content type
  httpResponse += "Content-Length: ";
  httpResponse += String(json.length());
  httpResponse += "\r\n\r\n";
  httpResponse += json;

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

  String sendResult = sendCommand(sendCmd, 2000);
  delay(500);

  if (sendResult.indexOf(">") != 1) {
    esp8266.println(httpResponse);
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