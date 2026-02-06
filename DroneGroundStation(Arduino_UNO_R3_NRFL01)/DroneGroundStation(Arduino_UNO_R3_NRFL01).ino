#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 6);
const byte address[6] = "00001";

int16_t data[3];// [0]=throttle, [1]=RightElevon, [2]=LeftElevon
char rxBuf[32]; //buffer for the data received via serial
uint8_t rxPos = 0;

void setup() {
  Serial.begin(115200);

  if (!radio.begin()) {
    Serial.println("Radio Error!");
    while (1);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(15, 15);
  radio.stopListening();

  data[0] = 0;
  data[1] = 76;
  data[2] = 76;

  Serial.println("Nadajnik gotowy.");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      rxBuf[rxPos] = 0;   // zakończ string
      parseFrame(rxBuf);
      rxPos = 0;
    } 
    else if (rxPos < sizeof(rxBuf) - 1) {
      rxBuf[rxPos++] = c;
    }
  }
}

void parseFrame(char *buf) {
  int t, l, r;
  if (sscanf(buf, "T:%d;L:%d;R:%d", &t, &l, &r) == 3) {
    data[0] = constrain(t, 0, 180);
    data[2] = constrain(l, 0, 180);
    data[1] = constrain(r, 0, 180);

    if (radio.write(&data, sizeof(data))) {
      //Serial.print("DataSent:");
      //Serial.print("Throttle:");
      //Serial.print(data[0]);
      //Serial.print(" LeftElevon:");
      //Serial.print(data[2]);
      //Serial.print(" RightElevon:");
      //Serial.println(data[1]);
    } else {
      //Serial.println("packets not sent");
    }
  }
}
