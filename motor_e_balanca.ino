#include <ESP32Servo.h>
#include "HX711.h"

// motor
Servo esc;
const int PINO_ESC = 23;
int pulsoAtual = 1000;

// balança
HX711 balanca;
const int DT_PIN  = 16;
const int SCK_PIN = 4;
const float fator = -108.5926;

void setup() {
  Serial.begin(115200);

  esc.setPeriodHertz(50);
  esc.attach(PINO_ESC, 1000, 2000);
  esc.writeMicroseconds(1000);       // armar
  Serial.println("Armando ESC...");

  balanca.begin(DT_PIN, SCK_PIN);
  balanca.set_scale(fator);
  delay(3000);                       // espera o arming e a célula estabilizar
  balanca.tare();                    // zera com o motor parado

  Serial.println("Pronto! Digite 1000 a 2000, ou 's' para parar.");
}

void loop() {
  if (Serial.available()) {
    if (Serial.peek() == 's') {      // parada de emergência
      Serial.read();
      pulsoAtual = 1000;
      esc.writeMicroseconds(pulsoAtual);
      Serial.println("PARADO");
    } else {
      int valor = Serial.parseInt();
      if (valor >= 1000 && valor <= 2000) {
        pulsoAtual = valor;
        esc.writeMicroseconds(pulsoAtual);
      }
    }
  }

  if (balanca.is_ready()) {
    Serial.print(pulsoAtual);
    Serial.print(", ");
    Serial.println(balanca.get_units(3), 1);
  }
}
