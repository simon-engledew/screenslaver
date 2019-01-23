// Library
#include <SPI.h>
#include <ESP8266WiFi.h>
#include "epd5in83.h"

// WiFi settings
const char* ssid = "";
const char* password = "";

// Host
const char* host = "";

#define ROW EPD_WIDTH/8

unsigned char imageBuffer[ROW];

void setup()
{

  // Serial
  Serial.begin(115200);
  Serial.println("ESP8266 in normal mode");


  Epd epd;
  if (epd.Init() != 0) {
      Serial.println("e-Paper init failed");
      return;
  }
  Serial.println("e-Paper init succeeded");

  // epd.Clear();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Logging data to cloud
  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 8000;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  Serial.println("Available:");
  Serial.println(client.available(), DEC);

  epd.SendCommand(DATA_START_TRANSMISSION_1);

  unsigned char pixels;
  bool left, right;

  for (int y = 0; y < EPD_HEIGHT; y++) {
    if (client.readBytes(imageBuffer, sizeof(imageBuffer)) > 0) {
      for (int x = 0; x < ROW; x++) {
        pixels = imageBuffer[x];

        for (short shift = 0; shift < 8; shift++) {
          left = (pixels & (1 << shift)) > 0;
          shift++;
          right = (pixels & (1 << shift)) > 0;

          if (left) {
            if (right) {
              epd.SendData(0x33);
            } else {
              epd.SendData(0x30);
            }
          } else {
            if (right) {
              epd.SendData(0x03);
            } else {
              epd.SendData(0x00);
            }
          }
        }
      }
    } else {
      Serial.println("Short read");
      break;
    }
  }

  epd.SendCommand(DISPLAY_REFRESH);
  delay(100);
  epd.WaitUntilIdle();

  client.stop();

  epd.Sleep();

  Serial.println();

  client.stop();

  Serial.println();
  Serial.println("closing connection");

  // Sleep
  Serial.println("ESP8266 in sleep mode");
  ESP.deepSleep(5 * 1000000);
}

void loop()
{

}
