#include <SoftwareSerial.h>
#include <TM1637Display.h>
#include <EEPROM.h>
#include <DHT11.h>

#define ESP_TX 2
#define ESP_RX 3
#define DIO 6
#define CLK 7
#define DHT 8

#define JSON_STATUS_OK "ok"
#define JSON_STATUS_ERROR "error"
#define JSON_STATUS_OUTBOUND "outbound"

const String WIFI_SSID = "TP-Link_69420";
const String WIFI_PASS = "WifiTPLINK1250@";
String IP = "";

SoftwareSerial esp8266(ESP_TX, ESP_RX);  // arduino RX - TX
TM1637Display display(CLK, DIO);
DHT11 dht11(DHT);

int temperature = 0;
int humidity = 0;
String dht11Error = "";
unsigned long lastReadTime = 0;

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);

  display.setBrightness(4);
  display.clear();
  delay(1000);
  initWifiModule();
}

void loop() {
  int result;
  if (millis() - lastReadTime >= 1000) {  // Ensure at least 1s between reads
    lastReadTime = millis();

    Serial.println(result);
    result = dht11.readTemperatureHumidity(temperature, humidity);
    dht11Error = (result == 0) ? "" : DHT11::getErrorString(result);
  }

  if (esp8266.available()) {
    if (esp8266.find("+IPD,")) {
      delay(1000);
      int connectionId = esp8266.read() - '0';
      Serial.println(connectionId);

      sendJSON(connectionId);
    }
  }
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
  httpResponse += "Content-Type: application/json\r\n";                                 // Your content type
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

  if (sendCommand(sendCmd, 2000).indexOf(">") != 1) {
    esp8266.println(httpResponse);
    delay(1000);
  }
  sendCommand(closeCmd, 1000);
  delay(500);
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