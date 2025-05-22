#include <PonteSerial.h>
#include <Wifi.h>
#include <SmartCupAPI.h>

const String SSID = "SEXTAREG15";
const String SENHA = "a1b2c3d4";

SmartCupApi api;
PonteSerial ponte;

void aguardar_mega();
void verificar_wifi();
void verificar_api();
void processar_pedido(const String &pedido);

void setup()
{
    ponte.iniciar(9600);
    delay(6000);

    aguardar_mega();
    verificar_wifi();
    verificar_api();
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

    delay(10);
}

void aguardar_mega()
{
    while (true)
    {
        String recebido = ponte.receber();
        recebido.trim();

        if (recebido == "OI ESP")
        {
            ponte.enviar("OI MEGA");
            break;
        }
        delay(10);
    }
}

void verificar_wifi()
{
    while (true)
    {
        String recebido = ponte.receber();
        recebido.trim();

        if (recebido == "WIFI")
        {
            conectar_wifi(SSID, SENHA);
            ponte.enviar("WIFI OK");
            break;
        }
        delay(10);
    }
}

void verificar_api()
{
    while (true)
    {
        String recebido = ponte.receber();
        recebido.trim();

        if (recebido == "API")
        {
            while (true)
            {
                JsonDocument doc = api.obter_json("/");
                if (!doc.isNull())
                {
                    String mensagem = doc["mensagem"].as<String>();
                    if (mensagem == "Olá, mundo!")
                        break;
                }
                delay(1000);
            }

            ponte.enviar("API OK");
            break;
        }
        delay(10);
    }
}

void processar_pedido(const String &codigo_copo)
{
    // Prepara o JSON para o endpoint /operacao
    JsonDocument json;
    json["maquina_id"] = 1; // ID da máquina
    json["codigo_nfc"] = codigo_copo; // UID do copo enviado pelo Mega

    // Envia o JSON para o endpoint /operacao
    JsonDocument doc = api.post_json("/operacao", json);
    if (!doc.isNull())
    {
        // Verifica se a resposta contém o campo "detail" (erro)
        if (doc["detail"].is<String>())
        {
            String motivo = doc["detail"].as<String>();

            // Mapeia os erros da API para mensagens esperadas pelo Mega
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
                ponte.enviar("ERRO"); // Erro genérico
        }
        else
        {
            // Pedido bem-sucedido
            ponte.enviar("OK"); // Envia "OK" para o Mega

            // Aguarda o pedido de quantidade do Mega
            while (true)
            {
                String novo_pedido = ponte.receber();
                novo_pedido.trim();

                if (novo_pedido == "QUANTIDADE")
                {
                    // Envia a quantidade do copo
                    int capacidade_copo = doc["copo"]["capacidade"].as<int>();
                    ponte.enviar(String(capacidade_copo));
                    break;
                }

                delay(10);
            }
        }
    }
    else
    {
        ponte.enviar("ERRO_API"); // Envia erro genérico se a API não responder
    }
}