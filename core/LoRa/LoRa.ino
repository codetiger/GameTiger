#include "lora.h"
#include <SPI.h>

#define LED_GPIO PIN_LED
Lora *lora;

void setup() {
  pinMode(LED_GPIO, OUTPUT);
  Serial.begin(9600);
  delay(2000);
  Serial.println("[Main] Setting up Lora Chip");
  SPI.begin();
  
  lora = new Lora(20);
  lora->SetRxEnable();
  Serial.println("[Main] Done");
}

void loop() {
  digitalWrite(LED_GPIO, LOW);

  char payload[] = "Hi Lora, GameTiger";
  lora->SendData(payload, strlen(payload));
  uint16_t delta = 0;
  do {
      delta += 50;
      delay(50);
      lora->ProcessIrq();
  } while(delta <= 5000);

  lora->CheckDeviceStatus();

  digitalWrite(LED_GPIO, HIGH);
  delay(2000);
}