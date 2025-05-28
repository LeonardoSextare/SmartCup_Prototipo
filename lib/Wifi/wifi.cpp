#include <Arduino.h>
#include <Wifi.h>
#include <ESP8266WiFi.h>

void conectar_wifi(String ssid, String senha)
{
  // Serial.print("Conectando-se a ");
  // Serial.println(ssid);

  WiFi.begin(ssid, senha);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    // Serial.print(".");
  }

  // Serial.println("\nWiFi conectado!");
  // Serial.print("IP: ");
  // Serial.println(WiFi.localIP());
}