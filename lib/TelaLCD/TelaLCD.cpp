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

void DisplayLCD::Escrever(const String texto, bool limpar_tela)
{
    if (limpar_tela)
    {
        lcd.clear();
    }

    const uint8_t linhas = 2;
    const uint8_t colunas = 16;
    uint16_t idx = 0;
    uint16_t len = texto.length();

    for (uint8_t linha = 0; linha < linhas && idx < len; linha++)
    {
        while (idx < len && texto[idx] == ' ')
            idx++;

        uint16_t maxLen = (len - idx > colunas) ? colunas : (len - idx);
        uint16_t endIdx = idx + maxLen;

        if (endIdx < len && texto[endIdx] != ' ')
        {
            int16_t lastSpace = texto.lastIndexOf(' ', endIdx - 1);
            if (lastSpace >= (int16_t)idx)
            {
                endIdx = lastSpace;
            }
        }

        String linha_texto = texto.substring(idx, endIdx);
        idx = endIdx;
        while (idx < len && texto[idx] == ' ')
            idx++;

        uint8_t espacos = (colunas - linha_texto.length()) / 2;
        lcd.setCursor(espacos, linha);
        lcd.print(linha_texto);
    }
}

void DisplayLCD::LimparTela()
{
    lcd.clear();
}