#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(7, 10); // CE, CSN
const byte address[6] = "00001";

Servo ESC;
Servo rightServo;
Servo leftServo;



int16_t data[3];

void setup() {
  Serial.begin(115200);
  
  ESC.attach(2, 1000, 2000);
  rightServo.attach(3, 1000, 2000);
  leftServo.attach(4, 1000, 2000);

  if (!radio.begin()) {
    Serial.println("Radio nRF24L01 nie odpowiada!");
    while (1); 
  }
  
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS); // Niższa prędkość = większy zasięg i stabilność
  radio.setRetries(15, 15);
  radio.startListening();
  data[0] = 0;
  data[1] = 76;
  data[2] = 76;
  delay(1000);
  Serial.println("Odbiornik aktywny...");
  rightServo.write(76);
  leftServo.write(76);
  ESC.write(0);
}

void loop() {
  if (radio.available()) {
    // Odczytujemy całą strukturę naraz
    radio.read(&data, sizeof(data));
    Serial.print("XXX -> ESC: "); Serial.print(data[0]);
    Serial.print(" | ServoR: "); Serial.print(data[1]);
    Serial.print(" | ServoL: "); Serial.println(data[2]);
    // 1. Sterowanie ESC (0-180)
    data[0] = constrain(data[0], 0, 180);
    ESC.write(data[0]);
    //ESC.write(0);

    data[1] = constrain(data[1], 20, 120); // Zabezpieczenie przed wyjściem poza zakres
    rightServo.write(data[1]);

    data[2] = constrain(data[2], 20, 12);
    leftServo.write(data[2]);

    // Debugowanie
    Serial.print("XXX -> ESC: "); Serial.print(data[0]);
    Serial.print(" | ServoR: "); Serial.print(data[1]);
    Serial.print(" | ServoL: "); Serial.println(data[2]);
    //delay(10);
  }
}