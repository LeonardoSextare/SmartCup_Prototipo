#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

class SmartCupApi
{
public:
    SmartCupApi();

    /**
     * GET na rota e retorna JsonDocument desserializado
     * (já imprime erro e devolve doc.isNull()==true em falha).
     */
    JsonDocument obter_json(const char *rota);

    /**
     * POST na rota com um corpo JSON e retorna JsonDocument desserializado
     * (já imprime erro e devolve doc.isNull()==true em falha).
     */
    JsonDocument post_json(const char *rota, const JsonDocument &corpo);

private:
    // faz o GET e retorna o payload bruto
    String _fazer_get(const char *rota);

    // faz o POST e retorna o payload bruto
    String _fazer_post(const char *rota, const String &payload);

    // parseia um payload JSON em JsonDocument (e imprime erro se inválido)
    JsonDocument _parsear_json(const String &payload);

    static constexpr const char *URL_BASE = "https://smartcupapi.onrender.com";
    static constexpr uint32_t TIMEOUT_MS = 5000;

    WiFiClientSecure _client;
    HTTPClient _http;
};