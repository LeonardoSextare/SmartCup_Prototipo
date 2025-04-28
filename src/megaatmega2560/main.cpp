#include <Arduino.h>
#include "ArduinoComm.h"

ArduinoComm comm(Serial, 9600); // Instancia a biblioteca com o objeto Serial

void setup() {
    Serial.begin(9600); // Inicializa a comunicação serial
    Serial.println("Arduino Mega pronto!");
}

void loop() {
    // Verifica se há uma mensagem recebida
    if (comm.processIncomingData()) {
        String message = comm.readMessage();
        Serial.print("Recebido do ESP8266: ");
        Serial.println(message);

        // Responde ao ESP8266
        comm.sendString("Olá, ESP8266!");
    }

    delay(1000); // Pequeno atraso para evitar sobreposição
}