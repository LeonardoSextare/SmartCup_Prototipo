#include "Arduino.h"
#include <MFRC522.h>
#include <SPI.h>
#include "LeitorRFID.h"

#define DEBUG
RFID_RC522::RFID_RC522(int pinoSDA, int pinoRST)
    : rfid(pinoSDA, pinoRST) {}

bool RFID_RC522::Inicializar()
{
    SPI.begin();
    rfid.PCD_Init();
    if (rfid.PCD_PerformSelfTest() != true)
    {
        Serial.println("Falha no Leitor NFC");
        delay(1000);
        return false;
    }
    Serial.println("Leitor NFC Iniciado com Sucesso!");
    return true;
}

void RFID_RC522::FinalizarOperacao()
{
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

bool RFID_RC522::Autenticar(byte bloco)
{
    MFRC522::MIFARE_Key chave = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

    MFRC522::StatusCode retorno = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &chave, &(rfid.uid));
    if (retorno != MFRC522::STATUS_OK)
    {
#ifdef DEBUG
        Serial.println("Autenticação falhou no bloco: " + bloco);
        Serial.println("Erro: ");
        Serial.print(rfid.GetStatusCodeName(retorno));
#endif // DEBUG
        return false;
    }
    return true;
}

// Função não utilizada por enquanto
// bool RFID_RC522::AutenticarNTAG213(byte bloco)
// {
//     byte senha_buffer[] = {0xFF, 0xFF, 0xFF, 0xFF};
//     byte senha_ACK[] = {0, 0};

//     MFRC522::StatusCode retorno = rfid.PCD_NTAG216_AUTH(&senha_buffer[0], senha_ACK);

//     if (retorno != MFRC522::STATUS_OK)
//     {
//         Serial.print("Autenticação falhou: ");
//         Serial.println(rfid.GetStatusCodeName(retorno));
//         Serial.println("Bloco: " + bloco);
//         return false;
//     }
//     Serial.print("Auth: ");
//     Serial.print(senha_ACK[0], HEX);
//     Serial.println(senha_ACK[1], HEX);
//     return true;
// }

String RFID_RC522::LerUID()
{
    String uid = "";
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
    {
        for (uint8_t i = 0; i < rfid.uid.size; i++)
        {
            uid += (rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
            uid += String(rfid.uid.uidByte[i], HEX);
        }
        uid.toUpperCase();
    }

    return uid;
}

bool RFID_RC522::GravarDados(String &dados)
{
    uint16_t tamanho = dados.length();
    if (tamanho > 721)
    {
#ifdef DEBUG
        Serial.println("Não há espaço suficiente no cartão para todos os dados.");
#endif // DEBUG
        return false;
    }

    uint16_t index = 0;
    byte buffer[16];

    for (byte setor = 1; setor < 16; setor++)
    {
        for (byte bloco = 0; bloco < 3; bloco++)
        {
            byte enderecoBloco = setor * 4 + bloco;
            memset(buffer, 0, 16);

            for (byte i = 0; i < 16 && index < tamanho; i++, index++)
                buffer[i] = dados[index];

            if (!Autenticar(enderecoBloco))
                return false;

            MFRC522::StatusCode retorno = rfid.MIFARE_Write(enderecoBloco, buffer, 16);
            if (retorno != MFRC522::STATUS_OK)
            {
#ifdef DEBUG
                Serial.println("Falha na escrita\nSetor: " + setor);
                Serial.println("Bloco: " + bloco);
                Serial.println("Endereco Bloco: " + enderecoBloco);
                Serial.println("Erro: ");
                Serial.print(rfid.GetStatusCodeName(retorno));
#endif // DEBUG
                return false;
            }

            if (index >= tamanho)
            {
#ifdef DEBUG
                Serial.println("Todos os blocos foram gravados corretamente.");
#endif // DEBUG
                return true;
            }
        }
    }
    return true; // Necessario para retirada de warning, o codigo não deve chegar nessa linha.
}

String RFID_RC522::LerDados()
{
    String dados = "";
    byte buffer[18];
    byte tamanhoBuffer = 18;

    for (byte setor = 1; setor < 16; setor++)
    {
        for (byte bloco = 0; bloco < 3; bloco++)
        {
            byte enderecoBloco = setor * 4 + bloco;

            if (!Autenticar(enderecoBloco))
                return "Falha na leitura";

            MFRC522::StatusCode retorno = rfid.MIFARE_Read(enderecoBloco, buffer, &tamanhoBuffer);
            if (retorno != MFRC522::STATUS_OK)
            {
#ifdef DEBUG
                Serial.println("Falha na leitura\nSetor: " + setor);
                Serial.println("Bloco: " + bloco);
                Serial.println("Endereco Bloco: " + enderecoBloco);
                Serial.println("Erro: ");
                Serial.print(rfid.GetStatusCodeName(retorno));
#endif // DEBUG
                return "Falha na leitura";
            }

            for (byte i = 0; i < 16; i++)
            {
                if (buffer[i] == '\0')
                    return dados;

                dados += (char)buffer[i];
            }
        }
    }
    return dados;
}

String RFID_RC522::LerDadosTAG213()
{
    String dados = "";
    byte buffer[18];
    byte tamanhoBuffer = 18;

    for (byte bloco = 4; bloco < 37; bloco++)
    {
        MFRC522::StatusCode retorno = rfid.MIFARE_Read(bloco, buffer, &tamanhoBuffer);
        if (retorno != MFRC522::STATUS_OK)
        {
#ifdef DEBUG
            Serial.println("Falha na Leitura da TAG");
            Serial.println("Bloco: " + bloco);
            Serial.println("Erro: ");
            Serial.print(rfid.GetStatusCodeName(retorno));
#endif // DEBUG
            return "Falha na Leitura";
        }

        for (byte i = 0; i < 4; i++)
        {
            if (buffer[i] == '\0')
                return dados;

            dados += (char)buffer[i];
        }
    }
    return dados;
}

bool RFID_RC522::GravarDadosTAG213(String &dados)
{
    uint16_t tamanho = dados.length();
    if (tamanho > 128)
    {
#ifdef DEBUG
        Serial.println("Não há espaço suficiente na TAG para todos os dados.");
#endif // DEBUG
        return false;
    }

    uint16_t index = 0;
    byte buffer[4];

    for (byte bloco = 4; bloco < 37; bloco++)
    {
        memset(buffer, 0, 4);

        for (byte i = 0; i < 4 && index < tamanho; i++, index++)
            buffer[i] = dados[index];

        MFRC522::StatusCode retorno = rfid.MIFARE_Ultralight_Write(bloco, buffer, 4);
        if (retorno != MFRC522::STATUS_OK)
        {
#ifdef DEBUG
            Serial.println("Falha na escrita\nbloco: " + bloco);
            Serial.println("Erro: ");
            Serial.print(rfid.GetStatusCodeName(retorno));
#endif // DEBUG
            return false;
        }

        if (index >= tamanho)
        {
#ifdef DEBUG
            Serial.println("Todos os blocos foram gravados corretamente.");
#endif // DEBUG
            return true;
        }
    }
    return true; // Necessario para retirada de warning, o codigo não deve chegar nessa linha.
}