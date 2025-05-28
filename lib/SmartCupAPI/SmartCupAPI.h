#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

class SmartCupApi
{
public:
    SmartCupApi();

    JsonDocument obter_json(const char *rota);
    JsonDocument post_json(const char *rota, const JsonDocument &corpo);
    void conectar_api();

private:
   
    String _fazer_get(const char *rota);
    String _fazer_post(const char *rota, const String &payload);
    JsonDocument _parsear_json(const String &payload);

    static constexpr const char *URL_BASE = "https://smartcupapi.onrender.com";
    static constexpr uint32_t TIMEOUT_MS = 5000;

    WiFiClientSecure _client;
    HTTPClient _http;
};