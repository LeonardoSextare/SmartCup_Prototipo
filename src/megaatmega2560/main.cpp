#include <PonteSerial.h>
#include <TelaLCD.h>
#include <LeitorRFID.h>

#define ID_LCD 0x27
#define PINO_RST 48
#define PINO_SDA 53
#define PINO_RELE 7

PonteSerial ponte;
RFID_RC522 LeitorNFC(PINO_SDA, PINO_RST);
DisplayLCD Tela(ID_LCD, 2, 16);

void processar_resposta(const String &resposta);
void liberar_liquido(int quantidade_ml);

void enviar_comando_esp(const String &mensagem_envio, const String &mensagem_esperada, const String &mensagem_erro);

void setup()
{
    pinMode(PINO_RELE, OUTPUT);
    digitalWrite(PINO_RELE, HIGH);

    Tela.Inicializar();
    Tela.Escrever("Iniciando Sistema");
    delay(5000);

    ponte.iniciar(9600);

    enviar_comando_esp("OI ESP", "OI MEGA", "Aguardando ESP");
    enviar_comando_esp("WIFI", "WIFI OK", "Conectando Wifi");
    enviar_comando_esp("API", "API OK", "Conectando API");

    Tela.Escrever("Iniciando Leitor NFC");
    delay(2000);
    while (!LeitorNFC.Inicializar())
    {
        Tela.Escrever("Falha no Leitor NFC");
        delay(1000);
    }

    Tela.Escrever("Sistema Iniciado");
    delay(2000);
}

void loop()
{
    Tela.Escrever("Aproxime o Copo");
    String UID = "";

    while ((UID = LeitorNFC.LerUID()) == "")
        ;

    Tela.Escrever("Validando Copo...");
    UID.replace(" ", "");

    String resposta = ponte.enviarPedido(UID, 10000);
    processar_resposta(resposta);

    delay(10000);
}

void processar_resposta(const String &resposta)
{
    if (resposta == "OK")
    {
        Tela.Escrever("Liberando Liquido...");
        String qntd = ponte.enviarPedido("QUANTIDADE", 10000);
        int qntd_int = qntd.toInt();
        liberar_liquido(qntd_int);
        delay(5000);
        Tela.Escrever("Retire o Copo");
    }
    else if (resposta == "SALDO")
        Tela.Escrever("Saldo Insuficiente.");
    else if (resposta == "ALCOOL")
        Tela.Escrever("Copo Nao Permite Alcool.");
    else if (resposta == "CLIENTE")
        Tela.Escrever("Copo Sem Cliente.");
    else if (resposta == "VAZIO")
        Tela.Escrever("Maquina sem Liquido");
    else if (resposta == "COPO")
        Tela.Escrever("Copo nao cadastrado!");
    else
        Tela.Escrever("Erro Desconhecido");
}

void liberar_liquido(int quantidade_ml)
{
    Tela.Escrever("Liberando Liquido...");
    int vazao_por_segundo = 18;

    double tempo_ms = (quantidade_ml / vazao_por_segundo) * 1000;
    digitalWrite(7, LOW);
    delay((tempo_ms));
    digitalWrite(7, HIGH);

    Tela.Escrever("Retire o Copo");
}

void enviar_comando_esp(const String &mensagem_envio, const String &mensagem_esperada, const String &mensagem_erro)
{
    while (true)
    {
        String resposta = ponte.enviarPedido(mensagem_envio);
        if (resposta == mensagem_esperada)
            break;
        Tela.Escrever(mensagem_erro);
        delay(1000);
    }
}