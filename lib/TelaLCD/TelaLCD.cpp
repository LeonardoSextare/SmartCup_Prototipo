#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TelaLCD.h>

DisplayLCD::DisplayLCD(uint8_t endereco, uint8_t colunas, uint8_t linhas)
    : lcd(endereco, colunas, linhas) {}

void DisplayLCD::Inicializar()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void DisplayLCD::Escrever(String texto, bool quebra_linha, bool limpar_tela)
{
    if (limpar_tela)
        lcd.clear();

    size_t c = 0, l = 0;
    while (c < texto.length())
    {
        lcd.setCursor(c % 16, l);
        lcd.write(texto[c]);

        if ((c + 1) % 16 == 0 && quebra_linha)
            l++;

        c++;
    }
}

void DisplayLCD::LimparTela()
{
    lcd.clear();
}