#include "PonteSerial.h"

String PonteSerial::enviarPedido(String mensagem, int tempo_limite)
{
  Serial.println(mensagem);

  unsigned long tempoInicio = millis(); // Marca o tempo de envio
  String resposta = "";

  // Enquanto não receber nada e não estourar o tempo limite
  while ((millis() - tempoInicio) < tempo_limite)
  { // 2000 ms = 2 segundos
    if (Serial.available() > 0)
    {
      resposta = Serial.readStringUntil('\n');
      resposta.trim();
      return resposta; // Resposta recebida dentro do tempo
    }
    delay(1); // Espera um pouco para evitar travar o microcontrolador
  }

  // Se saiu do while sem receber nada
  return "ERRO_TIMEOUT";
}

void PonteSerial::iniciar(long velocidade)
{
  Serial.begin(velocidade);
  while (!Serial)
  {
    ; // Espera até a porta serial estar pronta (necessário em alguns Arduinos como Leonardo)
  }
}

void PonteSerial::enviar(String mensagem)
{
  Serial.println(mensagem);
}

String PonteSerial::receber()
{
  String mensagemRecebida = "";
  if (Serial.available() > 0)
  {
    mensagemRecebida = Serial.readStringUntil('\n'); // Lê até encontrar o caractere de nova linha
  }
  return mensagemRecebida;
}
