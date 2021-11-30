#include <math.h>

double tmp = 0;
double roomtmp = 0;

int x = 0;
double y = 0;
int z = 0;

int samplerate = 3;
int lastSamplerate = 3;
int d;

void setup()
{
  pinMode (A0, INPUT);
  pinMode (A1, INPUT);
  pinMode (A3, INPUT);
  pinMode (A5, INPUT);
  pinMode (2, OUTPUT);
  pinMode (3, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  //============Temp.===============

  //lastSamplerate = samplerate;

  lastSamplerate = samplerate;
  samplerate = 2 + analogRead(A3) / 50;
  /*Serial.println(samplerate);
    if(samplerate<1||samplerate>25){
    samplerate = lastSamplerate;
    }
    Serial.println(samplerate);*/

    /*x = analogRead(A0);
    z = analogRead(A1);

    Serial.println(x);
    Serial.println(y);*/

    delay(1000);

  if (lastSamplerate == samplerate) {
    for (int i = 0; i < samplerate; i++) {
      x = analogRead(A1) - analogRead(A0);
      y = ((double)x / 1024.0) * 5000;
      tmp += y / 10 - 3;
      delay(100);
    }
    //Serial.println(x);

    x = analogRead(A5);
    y = ((double)x / 1024.0) * 5000;
    roomtmp = y / 10;
    


    //=========Avg._Temp.=============

    //Serial.println((double)tmp / samplerate);
    String my_message = String(tmp / samplerate) + ";" + String(samplerate * 120) + ";" + String(roomtmp-1) + ";" + String(samplerate);
    String complete_msg = "{" + my_message + "}";
    char *to_send = &complete_msg[0];
    Serial.write(to_send);

    if (tmp / samplerate < 0) {
      digitalWrite(3, HIGH);
      delay(20);
      digitalWrite(3, LOW);
    } else {
      digitalWrite(2, HIGH);
      delay(20);
      digitalWrite(2, LOW);
    }
  }
  tmp = 0;
}
