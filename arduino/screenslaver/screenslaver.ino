// Library
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "epd5in83.h"

extern const char* ssid;
extern const char* password;

#include "settings.h"

void die(const char* message) {
  Serial.println(message);
  Serial.flush();
  Serial.end();
  ESP.deepSleep(0);
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);

  while (!Serial) {
     delay(100);
  }

  Serial.println("Booted");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  HTTPClient http;

  if (!http.begin("http://screenslaver.surge.sh/data.bin")) {
    die("Failed to connect to host");
    return;
  }
  if (http.GET() != HTTP_CODE_OK) {
    die("Did not return 200 OK");
    return;
  }

  Epd epd;
  if (http.getSize() != epd.Size()) {
    die("Invalid Content-Length");
    return;
  }

  if (epd.Init() != 0) {
      die("Failed to init ePaper display");
      return;
  }

  Serial.print("Updating...");
  Serial.flush();

  epd.DisplayStream(http.getStreamPtr());
  epd.Sleep();

  http.end();

  Serial.println("...done");
  Serial.flush();
  Serial.end();

  ESP.deepSleep(10 * 1000000);
  // hardware will reset if xpd and dtr are connected
}

void loop()
{
  // N/A
}
