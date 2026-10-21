#include <Arduino.h>
#include "driver/uart.h"

const int BUTTON_PIN = 4;
const int LED_PIN = 5;
const int DMX_TX = 16;
const int DMX_RX = 15;

const uart_port_t DMX_UART = UART_NUM_2;

// 000 - 005 = Off
// 006 - 255 = On (velocidad de lento a rápido)
const uint8_t SPEED = 255;

uint8_t dmxData[33];
bool lastPressed = false;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  memset(dmxData, 0, sizeof(dmxData));
  Serial2.begin(250000, SERIAL_8N2, DMX_RX, DMX_TX);

  Serial.println("Controlador DMX Eurolite SD-201 iniciado.");
}

void sendDMXPacket() {
  Serial2.flush();

  uart_set_line_inverse(DMX_UART, UART_SIGNAL_TXD_INV);
  delayMicroseconds(120);

  uart_set_line_inverse(DMX_UART, UART_SIGNAL_INV_DISABLE);
  delayMicroseconds(12);

  Serial2.write(dmxData, sizeof(dmxData));
  Serial2.flush();
}

void loop() {
  bool pressed = (digitalRead(BUTTON_PIN) == HIGH);

  if (pressed != lastPressed) {
    // DEBUG!
    // Serial.println(pressed ? "Final de carrera accionado" : "Final de carrera en reposo");
    lastPressed = pressed;
  }

  // Cada elemento del array es una dirección DMX
  // Las Eurolite funcionan sólo en modo 6 canales, y sus direcciones son del 1 al 5 (es decir, máx 5 máquinas)
  dmxData[1] = pressed ? SPEED : 0;
  dmxData[2] = pressed ? SPEED : 0;
  digitalWrite(LED_PIN, pressed ? HIGH : LOW);

  sendDMXPacket();
  delay(25);
}