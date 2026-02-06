#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DigitalIO.h>
#include <PsxControllerHwSpi.h>

bool c = 0;
void check();
const byte PIN_PS2_ATT = 10;
PsxControllerHwSpi<PIN_PS2_ATT> psx;
boolean connected = false;
byte lxOld, lyOld, rxOld, ryOld;
PsxButtons oldButtons;
RF24 radio(7, 6); // CE, CSN
const byte address[6] = "00001";
int16_t data[3];


void callback1() { Serial.println("Select"); }
void callback2() { Serial.println("L3"); }
void callback3() { Serial.println("R3"); }
void callback4() { Serial.println("Start"); }
void callback5() { Serial.println("Up"); }
void callback6() { Serial.println("Right"); }
void callback7() { Serial.println("Down"); }
void callback8() { Serial.println("Left"); }
void callback9() { Serial.println("L2"); }
void callback10() { Serial.println("R2"); }
void callback11() { Serial.println("L1"); }
void callback12() { Serial.println("R1"); }
void callback13() { Serial.println("Triangle"); }
void callback14() { Serial.println("Circle"); }
void callback15() { Serial.println("Cross"); }
void callback16() { Serial.println("Square"); }

void stickCallback0(byte x, byte y) {
  Serial.print("Left Stick: ");
  Serial.print(x);
  Serial.print(",");
  Serial.println(y);
}

void stickCallback1(byte x, byte y) {
  Serial.print("Right Stick: ");
  Serial.print(x);
  Serial.print(",");
  Serial.println(y);
}

void (*buttonCallbacks[16])() = {
  callback1, callback2, callback3, callback4,
  callback5, callback6, callback7, callback8,
  callback9, callback10, callback11, callback12,
  callback13, callback14, callback15, callback16
};

void (*stickCallbacks[2])(byte x, byte y) = {
  stickCallback0,
  stickCallback1
};
void setup() {
  delay(100);

  Serial.begin(115200);

  c = radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(15, 15);
  radio.stopListening();

  data[0] = 0;
  data[1] = 80;
  data[2] = 80;

  Serial.println("Nadajnik gotowy. Sterowanie: Q-Y (ESC), A/Z (Serwa)");
}
void loop() {
  if (!connected) {
    config();
  } else {
    if (!psx.read()) {
      Serial.println("Disconnected");
      connected = false;
    } else {
      check();
      readController();
    }
  }
  delay(16);
}
void readController() {
  PsxButtons buttons = psx.getButtonWord();

  if (oldButtons != buttons) {
    oldButtons = buttons;
    for (int i = 0; i < 16; i++) {
      if (buttons & 1) {
        (*buttonCallbacks[i])();
      }
      buttons >>= 1;
    }
  }
  byte lx, ly;
  psx.getLeftAnalog(lx, ly);
  if (lx != lxOld || ly != lyOld) {
    stickCallback0(lx, ly);
    lxOld = lx;
    lyOld = ly;
    data[0] = lyOld;
  }
  byte rx, ry;
  psx.getRightAnalog(rx, ry);
  if (rx != rxOld || ry != ryOld) {
    stickCallback1(rx, ry);
    rxOld = rx;
    ryOld = ry;
    data[1] = rxOld;
    data[2] = ryOld;
  }
  delay(20);
  bool success = radio.write(&data, sizeof(data));
  if (success) {
    Serial.print("Wyslano -> ESC: ");
    Serial.print(data[0]);
    Serial.print(" | R: ");
    Serial.print(data[1]);
    Serial.print(" | L: ");
    Serial.println(data[2]);
  }
}
void config() {
  if (psx.begin()) {
    Serial.println("Connected");
    delay(300);
    if (!psx.enterConfigMode()) {
      Serial.println("Cannot enter config mode");
    } else {
      psx.enableAnalogSticks();
      psx.enableAnalogButtons();
      psx.exitConfigMode();
    }
    connected = true;
  }
}
void check() {
  if (!c) {
    Serial.println("radio.begin zawiodlo");
  }
  if (!radio.isChipConnected()) {
    Serial.println("blad w connected");
  }
}
