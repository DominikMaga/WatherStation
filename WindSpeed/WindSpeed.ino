#include <Wire.h>;

const int numreadings = 10;
int readings[numreadings];
unsigned long average = 0;
int indexs = 0;
unsigned long total;

volatile int rpmcount = 0;//see http://arduino.cc/en/Reference/Volatile
unsigned long rpm = 0;
unsigned long lastmillis = 0;
bool ledstat = 0;
int stablerpm = 0;

void setup()
{
  Serial.begin(9600);
  //  pinMode(13, OUTPUT);
  attachInterrupt(0, rpm_fan, FALLING);
  Wire.begin(38);
  Wire.onRequest(requestEvent);
}

void loop()
{
setWindSpeed();
delay(100);
}
void setWindSpeed() {

  if (millis() - lastmillis >= 1000) { 
    /*Aktualizowanie co sekunde, równa czętotliwość odczytów (Hz).*/
    detachInterrupt(0);    //wyłączenie przerywania podczas odczytów
    total = 0;
    readings[indexs] = rpmcount * 60;  													                                            /*  note: this works for one interruption per full rotation. For two interrups per full rotation use rpmcount * 30.*/

    for (int x = 0; x <= 9; x++) {
      total = total + readings[x];
    }
    average = total / numreadings;
    rpm = average;

    rpmcount = 0; // Restart licznika RPM
    indexs++;
    if (indexs >= numreadings) {
      indexs = 0;
    }

    if (millis() > 11000) { // oczekiwnaie na średnie RPM, aby uzyskać stabilnosc 
      stablerpm = rpm;
    }
    lastmillis = millis(); // aktualizowanie lasmillis
    attachInterrupt(0, rpm_fan, FALLING); //włącz przerwanie
  }
}
void rpm_fan() { /* tan kod będzie wykonywany z każdym przerwaniem 0 (pin2) kiedy będzie low.*/
  rpmcount++;
}
void requestEvent()
{
  int32_t bigNum = stablerpm;
  //Serial.println(bigNum);
  byte myArray[4];

  myArray[0] = (bigNum >> 24) & 0xFF;
  myArray[1] = (bigNum >> 16) & 0xFF;
  myArray[2] = (bigNum >> 8) & 0xFF;
  myArray[3] = bigNum & 0xFF;

  Wire.write(myArray, 4);
}
