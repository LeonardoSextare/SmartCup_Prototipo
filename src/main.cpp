#include <Arduino.h>
#include "LeitorRFID.h"

#define PINO_RST 49
#define PINO_SDA 53

RFID_RC522 LeitorNFC(PINO_SDA, PINO_RST);
String teste = "Teste de Gravação";
String testeTAG = "Teste de Gravação TAGG3";

void menu()
{
  Serial.println("");
  Serial.println("1. GravarDados");
  Serial.println("2. LerDados");
  Serial.println("3. GravarTAG");
  Serial.println("4. LerTag");
  Serial.println("5. Outros Testes");
  Serial.println("");
}

void setup()
{
  Serial.begin(9600);
  LeitorNFC.Inicializar();
  Serial.println("Arduino Iniciado");
  menu();
}

void loop()
{
  String retorno = "";

  if (Serial.available() > 0)
  {
    char input = Serial.read();

    switch (input)
    {
    case '1':
      Serial.println("1. Coloque o Cartão:");
      while (LeitorNFC.LerUID() == "")
        continue;

      LeitorNFC.GravarDados(teste);
      LeitorNFC.FinalizarOperacao();

      menu();
      break;

    case '2':
      Serial.println("2. Coloque o Cartão:");

      while (LeitorNFC.LerUID() == "")
        continue;

      Serial.println("Dados Lidos: ");
      Serial.println(LeitorNFC.LerDados());
      LeitorNFC.FinalizarOperacao();

      menu();
      break;

    case '3':
      Serial.println("3. Coloque a TAG:");

      while (LeitorNFC.LerUID() == "")
        continue;

      LeitorNFC.GravarDadosTAG213(testeTAG);
      LeitorNFC.FinalizarOperacao();

      menu();
      break;

    case '4':
      Serial.println("4. Coloque a TAG:");

      while (LeitorNFC.LerUID() == "")
        continue;

      Serial.println("Dados Lidos: ");
      Serial.println(LeitorNFC.LerDadosTAG213());
      LeitorNFC.FinalizarOperacao();

      menu();
      break;

    case '5':
      Serial.println("5. Coloque o Cartão/TAG:");

      while (LeitorNFC.LerUID() == "")
        continue;

      LeitorNFC.GravarDadosTAG213(testeTAG);
      // if (LeitorNFC.AutenticarNTAG213(4)){
      // Serial.println(LeitorNFC.LerDadosTAG213());

      // } else {
      //   Serial.println("Deu Ruim");
      // }

      // Serial.println("Nada ainda");

      menu();
      break;
    }
  }
  delay(2000);
}
