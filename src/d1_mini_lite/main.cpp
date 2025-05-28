#include <PonteSerial.h>
#include <Wifi.h>
#include <SmartCupAPI.h>

const String SSID = "SEXTAREG15";
const String SENHA = "a1b2c3d4";

SmartCupApi api;
PonteSerial ponte;

void processar_comando_mega(const String &esperada, const String &resposta, void (*acao)() = nullptr);
void processar_pedido(const String &pedido);

void setup()
{
    ponte.iniciar(9600);
    delay(6000);

    processar_comando_mega("OI ESP", "OI MEGA");
    processar_comando_mega("WIFI", "WIFI OK", []()
                           { conectar_wifi(SSID, SENHA); });
    processar_comando_mega("API", "API OK", []()
                           { api.conectar_api(); });
    delay(5000);
}

void loop()
{
    // Recebe o pedido do Mega
    String pedido = ponte.receber();
    pedido.trim();

    if (!pedido.isEmpty())
    {
        processar_pedido(pedido);
    }

    delay(100);
}
void processar_pedido(const String &codigo_copo)
{
    JsonDocument json;
    json["maquina_id"] = 1;
    json["codigo_nfc"] = codigo_copo;

    JsonDocument doc = api.post_json("/operacao", json);
    if (!doc.isNull())
    {
        if (doc["detail"].is<String>())
        {
            String motivo = doc["detail"].as<String>();

            if (motivo == "Copo não encontrado.")
                ponte.enviar("COPO");
            else if (motivo == "Copo não está associado a um cliente.")
                ponte.enviar("CLIENTE");
            else if (motivo == "Copo não permite bebidas alcoólicas.")
                ponte.enviar("ALCOOL");
            else if (motivo == "Máquina sem bebida suficiente.")
                ponte.enviar("VAZIO");
            else if (motivo == "Saldo insuficiente do cliente.")
                ponte.enviar("SALDO");
            else
                ponte.enviar("ERRO");
        }
        else
        {
            ponte.enviar("OK");
            String qntd_copo = doc["copo"]["capacidade"];
            processar_comando_mega("QUANTIDADE", qntd_copo);
        }
    }
    else
    {
        ponte.enviar("ERRO_API");
    }
}

void processar_comando_mega(const String &esperada, const String &resposta, void (*acao)())
{
    while (true)
    {
        String recebido = ponte.receber();
        recebido.trim();

        if (recebido == esperada)
        {
            if (acao != nullptr) // Verifica se o ponteiro não é nulo
                acao();          
            ponte.enviar(resposta);
            break;
        }
        delay(10);
    }
}