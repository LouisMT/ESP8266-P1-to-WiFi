#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

// Version identifier
const char* VERSION = "2021.01.26+1";

// Network settings
const char* HOSTNAME = "p1";
const char* SSID = "SomeSSID";
const char* PASSWORD = "SomePassword";
const unsigned int MAX_CLIENTS = 4;
const unsigned int PORT = 1337;

// Serial settings
const long DEBUG_BAUD_RATE = 9600;
const long P1_BAUD_RATE = 115200;
const SerialConfig P1_MODE = SERIAL_8N1;
const bool P1_INVERT = true;

// General settings
const unsigned int BUFFER_SIZE = 128;

// Globals
WiFiServer server(PORT);
WiFiClient *clients[MAX_CLIENTS] = { NULL };

void printFirmwareInfo() {
  Serial1.printf("* VERSION         %s\r\n", VERSION);
  Serial1.printf("Network Settings:\r\n");
  Serial1.printf("* HOSTNAME        %s\r\n", HOSTNAME);
  Serial1.printf("* SSID            %s\r\n", SSID);
  Serial1.printf("* PASSWORD        %s\r\n", "REDACTED");
  Serial1.printf("* MAX_CLIENTS     %d\r\n", MAX_CLIENTS);
  Serial1.printf("* PORT            %d\r\n", PORT);
  Serial1.printf("Serial Settings:\r\n");
  Serial1.printf("* DEBUG_BAUD_RATE %d\r\n", DEBUG_BAUD_RATE);
  Serial1.printf("* P1_BAUD_RATE    %d\r\n", P1_BAUD_RATE);
  Serial1.printf("* P1_MODE         %d\r\n", P1_MODE);
  Serial1.printf("* P1_INVERT       %d\r\n", P1_INVERT);
  Serial1.printf("General Settings:\r\n");
  Serial1.printf("BUFFER_SIZE       %d\r\n", BUFFER_SIZE);
}

void setup() {
  // Wait 5 seconds to allow debug serial to be connected
  delay(5000);

  Serial.begin(P1_BAUD_RATE, P1_MODE, SERIAL_RX_ONLY, 1, P1_INVERT);
  Serial1.begin(DEBUG_BAUD_RATE);
  printFirmwareInfo();

  Serial1.printf("Starting WiFi connection\r\n");
  WiFi.hostname(HOSTNAME);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial1.printf("Waiting for WiFi connection...\r\n");
    delay(500);
  }

  Serial1.printf("WiFi connected at %s\r\n", WiFi.localIP().toString().c_str());

  Serial1.printf("Starting server\r\n");
  server.begin();

  Serial1.printf("Starting OTA server\r\n");
  ArduinoOTA.begin();
}

void addClient() {
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  for (unsigned int i = 0; i < MAX_CLIENTS; i++) {
    if (!clients[i]) {
      Serial1.printf("Client %s accepted\r\n", client.remoteIP().toString().c_str());
      clients[i] = new WiFiClient(client);
      return;
    }
  }

  Serial1.printf("Client %s rejected (maximum clients reached)\r\n", client.remoteIP().toString().c_str());
  client.stop();
}

void readWriteSerial() {
  byte buffer[BUFFER_SIZE];
  int size = Serial.readBytes(buffer, BUFFER_SIZE);

  if (size == 0) {
    return;
  }

  for (unsigned int i = 0; i < MAX_CLIENTS; i++) {
    WiFiClient *client = clients[i];

    if (!client) {
      continue;
    } else if (!client->connected()) {
      Serial1.printf("Client %s disconnected\r\n", client->remoteIP().toString().c_str());
      clients[i] = NULL;
      delete client;
    } else {
      client->write(buffer, size);
    }
  }
}

void loop() {
  addClient();
  readWriteSerial();
  ArduinoOTA.handle();
}
