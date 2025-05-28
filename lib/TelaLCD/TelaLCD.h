#ifndef TelaLCD
#define TelaLCD

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class DisplayLCD
{
public:
    DisplayLCD(uint8_t endereco, uint8_t colunas, uint8_t linhas);
    void Inicializar();
    void Escrever(String texto, bool limpar_tela = true);
    void LimparTela();

private:
    LiquidCrystal_I2C lcd;
};

#endif // TelaLCD