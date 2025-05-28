#include "SmartCupApi.h"

SmartCupApi::SmartCupApi()
{
    _client.setInsecure();        // TLS sem validação (teste)
    _http.setTimeout(TIMEOUT_MS); // timeout único para cada requisição
}

JsonDocument SmartCupApi::obter_json(const char *rota)
{
    String payload = _fazer_get(rota);
    return _parsear_json(payload);
}

JsonDocument SmartCupApi::post_json(const char *rota, const JsonDocument &corpo)
{
    // Serializa o corpo JSON para uma string
    String payload;
    serializeJson(corpo, payload);

    // Faz o POST e obtém o payload de resposta
    String resposta = _fazer_post(rota, payload);
    return _parsear_json(resposta);
}

String SmartCupApi::_fazer_get(const char *rota)
{
    String url = String(URL_BASE) + rota;
    if (!_http.begin(_client, url))
    {
        // Serial.println("SmartCupApi: falha ao iniciar GET");
        return "";
    }
    int code = _http.GET();
    if (code != HTTP_CODE_OK)
    {
        // Serial.printf("SmartCupApi: erro HTTP GET %d\n", code);
        _http.end();
        return "";
    }
    String body = _http.getString();
    _http.end();
    return body;
}

String SmartCupApi::_fazer_post(const char *rota, const String &payload)
{
    String url = String(URL_BASE) + rota;
    if (!_http.begin(_client, url))
    {
        // Serial.println("SmartCupApi: falha ao iniciar POST");
        return "";
    }

    // Define o cabeçalho do conteúdo como JSON
    _http.addHeader("Content-Type", "application/json");

    // Envia o POST com o payload
    int code = _http.POST(payload);
    if (code != HTTP_CODE_OK && code != HTTP_CODE_CREATED)
    {
        // Serial.printf("SmartCupApi: erro HTTP POST %d\n", code);
        String body = _http.getString();
        _http.end();
        return body;
    }

    String body = _http.getString();
    _http.end();
    return body;
}

JsonDocument SmartCupApi::_parsear_json(const String &payload)
{
    JsonDocument doc;
    if (payload.isEmpty())
    {
        // Serial.println("SmartCupApi: payload vazio");
        return doc; // doc.isNull()==true
    }

    DeserializationError err = deserializeJson(doc, payload);
    if (err)
    {
        // Serial.print("SmartCupApi: JSON inválido (");
        // Serial.print(err.c_str());
        // Serial.println(")");
        return JsonDocument(); // doc nulo
    }
    return doc;
}

void SmartCupApi::conectar_api()
{
    while (true)
    {
        String json = _fazer_get("/");
        JsonDocument doc = _parsear_json(json);

        if (!doc.isNull())
        {
            String mensagem = doc["mensagem"].as<String>();
            if (mensagem == "Olá, mundo!")
                break;
        }
        delay(1000);
    }
}