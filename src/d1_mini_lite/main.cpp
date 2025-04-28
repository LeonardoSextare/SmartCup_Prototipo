#include <Arduino.h>
#include "ArduinoComm.h"

ArduinoComm comm(Serial, 9600); // Instancia a biblioteca com o objeto Serial

void setup() {
    Serial.begin(9600); // Inicializa a comunicação serial
    Serial.println("ESP8266 pronto!");
}

void loop() {
    // Envia uma mensagem para o Arduino Mega
    comm.sendString("Olá, Arduino Mega!");

    // Aguarda uma resposta
    unsigned long start = millis();
    while (!comm.processIncomingData() && millis() - start < 1000) {
        // Aguarda até 1 segundo por uma resposta
    }

    if (comm.processIncomingData()) {
        String response = comm.readMessage();
        Serial.print("Recebido do Mega: ");
        Serial.println(response);
    }

    delay(1000); // Aguarda antes de enviar novamente
}