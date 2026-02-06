#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7,6);
const byte address[6]="00001";
int16_t data[3];

void setup(){
  Serial.begin(115200);
  if(!radio.begin()){
    Serial.println("Radio Error!");
    while(1);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(15,15);
  radio.stopListening();

  data[0]=0;
  data[1]=76;
  data[2]=76;

  Serial.println("Nadajnik gotowy.");
}

void loop(){
  if(Serial.available()>0){
    char key=Serial.read();

    switch(key){
      case 'q': data[0]=0; break;
      case 'w': data[0]=36; break;
      case 'e': data[0]=72; break;
      case 'r': data[0]=108; break;
      case 't': data[0]=144; break;
      case 'y': data[0]=180; break;
      
      case 'a': data[2]=92; break; // Lewe serwo
      case 'z': data[2]=60; break;
      
      case 's': data[1]=92; break; // Prawe serwo
      case 'x': data[1]=60; break;
      
      case ' ': data[1]=76; data[2]=76; break;
    }

    if(radio.write(&data,sizeof(data))){
      Serial.print("Wyslano -> ESC: "); Serial.print(data[0]);
      Serial.print(" | R: "); Serial.print(data[1]);
      Serial.print(" | L: "); Serial.println(data[2]);
    }else{
      Serial.println("Blad wysylania - brak polaczenia!");
    }
  }
}