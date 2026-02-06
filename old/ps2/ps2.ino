#include <DigitalIO.h>
#include <PsxControllerHwSpi.h>

const byte PIN_PS2_ATT=10;
PsxControllerHwSpi<PIN_PS2_ATT> psx;

boolean connected=false;
byte lxOld,lyOld,rxOld,ryOld;
PsxButtons oldButtons;

void callback1(){Serial.println("Select");}
void callback2(){Serial.println("L3");}
void callback3(){Serial.println("R3");}
void callback4(){Serial.println("Start");}
void callback5(){Serial.println("Up");}
void callback6(){Serial.println("Right");}
void callback7(){Serial.println("Down");}
void callback8(){Serial.println("Left");}
void callback9(){Serial.println("L2");}
void callback10(){Serial.println("R2");}
void callback11(){Serial.println("L1");}
void callback12(){Serial.println("R1");}
void callback13(){Serial.println("Triangle");}
void callback14(){Serial.println("Circle");}
void callback15(){Serial.println("Cross");}
void callback16(){Serial.println("Square");}

void stickCallback0(byte x,byte y){
  Serial.print("Left Stick: ");
  Serial.print(x);
  Serial.print(",");
  Serial.println(y);
}

void stickCallback1(byte x,byte y){
  Serial.print("Right Stick: ");
  Serial.print(x);
  Serial.print(",");
  Serial.println(y);
}

void (*buttonCallbacks[16])()={
  callback1,callback2,callback3,callback4,
  callback5,callback6,callback7,callback8,
  callback9,callback10,callback11,callback12,
  callback13,callback14,callback15,callback16
};

void setup(){
  Serial.begin(115200);
  Serial.println("Inicjalizacja kontrolera...");
}

void loop(){
  if(!connected){
    config();
  }else{
    if(!psx.read()){
      Serial.println("Disconnected");
      connected=false;
    }else{
      readController();
    }
  }
  delay(16);
}

void readController(){
  PsxButtons buttons=psx.getButtonWord();
  if(oldButtons!=buttons){
    oldButtons=buttons;
    PsxButtons tempButtons=buttons;
    for(int i=0;i<16;i++){
      if(tempButtons&1){
        (*buttonCallbacks[i])();
      }
      tempButtons>>=1;
    }
  }

  byte lx,ly;
  psx.getLeftAnalog(lx,ly);
  if(lx!=lxOld||ly!=lyOld){
    stickCallback0(lx,ly);
    lxOld=lx;
    lyOld=ly;
  }

  byte rx,ry;
  psx.getRightAnalog(rx,ry);
  if(rx!=rxOld||ry!=ryOld){
    stickCallback1(rx,ry);
    rxOld=rx;
    ryOld=ry;
  }
}

void config(){
  if(psx.begin()){
    Serial.println("Connected");
    delay(300);
    if(!psx.enterConfigMode()){
      Serial.println("Cannot enter config mode");
    }else{
      psx.enableAnalogSticks();
      psx.enableAnalogButtons();
      psx.exitConfigMode();
    }
    connected=true;
  }
}