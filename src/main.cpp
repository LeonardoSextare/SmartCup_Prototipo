#include <Arduino.h>
#include <Wire.h>
#include <TelaLCD.h>
#include <LeitorRFID.h>

#define PINO_RST 48
#define PINO_SDA 53

#define ID_LCD 0x27

RFID_RC522 LeitorNFC(PINO_SDA, PINO_RST);
DisplayLCD Tela(ID_LCD, 2, 16);

void setup()
{
    Serial.begin(9600);
    Tela.Inicializar();
    while (LeitorNFC.Inicializar() != true)
        Tela.Escrever("Falha no Leitor NFC", true, false);

    Tela.Escrever("Arduino Iniciado");
    delay(2000);
}

void loop()
{
    String UID;
    Tela.Escrever("Aproxime a TAG");
    while((UID = LeitorNFC.LerUID()) == "");
    Serial.print(UID);
    if(UID == " 23 27 00 9A")
        Tela.Escrever("  Vai se fuder       Pedro");
    else
        Tela.Escrever("  Vai se fuder      Ribero");
    delay(5000);
}
