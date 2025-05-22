#include <PonteSerial.h>
#include <TelaLCD.h>
#include <LeitorRFID.h>

#define ID_LCD 0x27
#define PINO_RST 48
#define PINO_SDA 53

PonteSerial ponte;
RFID_RC522 LeitorNFC(PINO_SDA, PINO_RST);
DisplayLCD Tela(ID_LCD, 2, 16);

void conectar_esp();
void verificar_wifi();
void verificar_api();
void processar_resposta(const String &resposta);
void liberar_liquido(int pino_rele, int tempo_ms);
void liberar_liquido(int quantidade_ml);

void setup()
{
    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);
    // digitalWrite(7, LOW);
    Serial.begin(9600);
    Tela.Inicializar();
    Tela.Escrever("Iniciando Sistema");
    delay(5000);

    ponte.iniciar(9600);
    conectar_esp();
    verificar_wifi();
    verificar_api();

    Tela.Escrever("Iniciando Leitor NFC...");
    while (!LeitorNFC.Inicializar())
    {
        Tela.Escrever("Falha no Leitor NFC");
        delay(1000);
    }
    Tela.Escrever("Leitor NFC Iniciado!");
    Tela.Escrever("Sistema Iniciado");
    delay(2000);
}

void loop()
{
    Tela.Escrever("Aproxime o Copo");
    String UID;

    // Aguarda até que um UID seja lido
    while ((UID = LeitorNFC.LerUID()) == "")
        ;

    Tela.Escrever("Validando Copo...");
    UID.replace(" ", "");

    String resposta = ponte.enviarPedido(UID, 10000);
    processar_resposta(resposta);

    delay(10000);
}

void conectar_esp()
{
    Tela.Escrever("Aguardando ESP...");
    while (ponte.enviarPedido("OI ESP") != "OI MEGA")
        delay(1000);
    Tela.Escrever("ESP Iniciado!");
}

void verificar_wifi()
{
    Tela.Escrever("Conectando Wifi...");
    while (ponte.enviarPedido("WIFI") != "WIFI OK")
        delay(1000);
    Tela.Escrever("Wifi Conectado!");
}

void verificar_api()
{
    Tela.Escrever("Conectando API...");
    while (ponte.enviarPedido("API", 10000) != "API OK")
        delay(1000);
    Tela.Escrever("API Online");
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
        Tela.Escrever("Maquina sem Liquido");
    else
        Tela.Escrever("Erro Desconhecido");
}

void liberar_liquido(int quantidade_ml)
{
    Tela.Escrever("Liberando Liquido...");
    int vazao_por_segundo = 18;

    double tempo_ms = (quantidade_ml / vazao_por_segundo) * 1000;
    
    Serial.println("vazao");
    Serial.println(tempo_ms);
    // Liga o relé pelo tempo calculado
    digitalWrite(7, LOW);
    delay((tempo_ms));
    digitalWrite(7, HIGH);

    Tela.Escrever("Retire o Copo");
}