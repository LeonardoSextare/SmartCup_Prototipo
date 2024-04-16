#ifndef LeitorRFID
#define LeitorRFID

#include "Arduino.h"
#include <MFRC522.h>
#include <SPI.h>

class RFID_RC522
{
public:
    RFID_RC522(int pinoSDA, int pinoRST);

    const String CARTAO = " D3 D5 6A 0E";
    const String CHAVEIRO = " 23 27 00 9A";

    bool Inicializar();
    bool Autenticar(byte bloco);
    // bool AutenticarNTAG213(byte bloco); //Função não utilizada por enquanto
    String LerUID();
    bool GravarDados(String &dados);
    bool GravarDadosTAG213(String &dados);
    String LerDados();
    String LerDadosTAG213();
    void FinalizarOperacao();

private:
    MFRC522 rfid;
};

#endif // LeitorNFC