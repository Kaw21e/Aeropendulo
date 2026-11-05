#include "HX711.h"

const int DT_PIN   = 16;
const int SCK_PIN  = 4;
const int touchPin = 32;
const int limiar   = 500;

HX711 balanca;
float fatores[15];
float fatorCalibracao = 1.0;
int caso = 1;  // global: mantém o valor entre as voltas do loop

// espera tocar e soltar o sensor
void esperarToque() {
  while (touchRead(touchPin) > limiar) {  // espera tocar
    delay(20);
  }
  Serial.println("Toque reconhecido");
  while (touchRead(touchPin) < limiar) {  // espera soltar
    delay(20);
  }
  delay(200);
}

float media(float v[], int qtd) {
  if (qtd == 0) return 0;
  float soma = 0;
  for (int i = 0; i < qtd; i++) {
    soma += v[i];
  }
  return soma / qtd;
}

void setup() {
  Serial.begin(115200);
  balanca.begin(DT_PIN, SCK_PIN);
  balanca.set_scale(1.0);  // sem fator: get_units devolve (bruto - tara)
}

void loop() {
  switch (caso) {
    case 1:
      Serial.println("Retire todos os pesos e toque para iniciar.");
      esperarToque();
      balanca.tare(30);
      Serial.println("Tara feita.");
      caso = 2;
      break;

    case 2:
      Serial.println("Coloque o peso de 100 g e toque.");
      esperarToque();
      for (int i = 0; i < 5; i++) {
        fatores[i] = balanca.get_units(10) / 100.0;
      }
      caso = 3;
      break;

    case 3:
      Serial.println("Coloque o peso de 300 g e toque.");
      esperarToque();
      for (int i = 5; i < 10; i++) {
        fatores[i] = balanca.get_units(10) / 300.0;
      }
      caso = 4;
      break;

    case 4:
      Serial.println("Coloque o peso de 400 g e toque.");
      esperarToque();
      for (int i = 10; i < 15; i++) {
        fatores[i] = balanca.get_units(10) / 400.0;
      }
      caso = 5;
      break;

    case 5:
      fatorCalibracao = media(fatores, 15);
      balanca.set_scale(fatorCalibracao);
      Serial.print("O fator da balança é: ");
      Serial.println(fatorCalibracao, 4);
      Serial.println("Calibração concluída! Toque para zerar a balança.");
      caso = 6;
      break;

    case 6:  // modo balança normal
      if (touchRead(touchPin) < limiar) {
        Serial.println("Toque reconhecido");
        balanca.tare(20);
        Serial.println("Zerado!");
        delay(500);
      }
      if (balanca.is_ready()) {
        Serial.print(balanca.get_units(10), 1);
        Serial.println(" g");
      }
      delay(500);
      break;
  }
}
