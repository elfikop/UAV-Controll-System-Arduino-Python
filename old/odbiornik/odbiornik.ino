#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(7,10);
const byte address[6]="00001";

Servo ESC;
Servo rightServo;
Servo leftServo;
int16_t data[3];

void setup(){
  Serial.begin(115200);
  
  ESC.attach(2,1000,2000);
  delay(1000);
  ESC.write(30);
  rightServo.attach(3,1000,2000);
  leftServo.attach(4,1000,2000);

  if(!radio.begin()){
    Serial.println("Radio nRF24L01 nie odpowiada!");
    while(1); 
  }
  
  radio.openReadingPipe(0,address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(15,15);
  radio.startListening();

  data[0]=0;
  data[1]=80;
  data[2]=72;

  delay(1000);
  Serial.println("Odbiornik aktywny...");

  rightServo.write(63);
  leftServo.write(89);
  delay(1000);
  rightServo.write(51);
  leftServo.write(101);
  delay(1000);
  rightServo.write(63);
  leftServo.write(89);
  delay(1000);
  rightServo.write(51);
  leftServo.write(101);
  delay(1000);
  rightServo.write(75);
  leftServo.write(77);
  delay(1000);
  rightServo.write(63);
  leftServo.write(89);
  delay(5000);
  ESC.write(0);
}

void loop(){
  if(radio.available()){
    radio.read(&data,sizeof(data));

    data[0]=constrain(data[0],0,180);
    ESC.write(data[0]);

    data[1]=constrain(data[1],60,92);
    rightServo.write(data[1]);

    data[2]=constrain(data[2],60,92);
    leftServo.write(data[2]);

    Serial.print("XXX -> ESC: "); Serial.print(data[0]);
    Serial.print(" | ServoR: "); Serial.print(data[1]);
    Serial.print(" | ServoL: "); Serial.println(data[2]);
  }
}