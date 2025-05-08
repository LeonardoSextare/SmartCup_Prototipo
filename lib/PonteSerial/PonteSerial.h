#ifndef PONTE_SERIAL_H
#define PONTE_SERIAL_H

#include <Arduino.h>

class PonteSerial
{
public:
    String enviarPedido(String mensagem, int tempo_limite = 5000);

    void iniciar(long velocidade);
    void enviar(String mensagem);
    String receber();
};

#endif
