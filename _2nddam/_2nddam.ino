#include <pt.h>
#include <Servo.h>
#define PT_DELAY(pt, ms, ts) \
    ts = millis(); \
    PT_WAIT_WHILE(pt, millis()-ts < (ms));

#define Low 0
#define Med 1
#define High 2
#define Need 3
#define Commanded 4
#define Opened 1
#define Closed 2

String val = "";

int state = 0;
int state2= 0;
int reason = 0;
int gate = 0;
int state3= 0;
int Vol;

struct pt pt_taskUS2;
struct pt pt_taskSendSerial;
struct pt pt_taskUS;

///////////////////////////////////////////////////////
void setValue(){
  if(val.charAt(1) == 2)
  switch(val.charAt(0) {
   case 'O' :
   state3 = Opened;
   break;
   case 'C' :
   state3 = Closed;
   break;
  }
  switch(val.charAt(2) {
   case '0' :
   state2 = Low;
   break;
   case '1' :
   state2 = Med;
   break;
   case '2' :
   state3 = High;
   break;
  }
}

///////////////////////////////////////////////////////
void serialEvent() {
  if (Serial1.available() > 0) {
    val = Serial1.readStringUntil('\r');
    Serial.print("value Recieve : ");
    Serial.println(val);
    Serial1.flush();
    setValue();
  }
}

///////////////////////////////////////////////////////
void sendSerial(){
  String sendData = String(state)+String(gate)+String(reason)+String(Vol);
  Serial1.print(sendData);
  Serial1.print('\r');
  Serial.print(sendData);
  Serial.print('\r');
}

///////////////////////////////////////////////////////
PT_THREAD(taskSendSerial(struct pt* pt))
{
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1)
  {
    sendSerial();
    PT_DELAY(pt, 600, ts);
  }

  PT_END(pt);
}
///////////////////////////////////////////////////////
#define trigPin 3
#define echoPin 2

long distance,duration;

//////////////////////////////////////////////////////
void USVol(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
    Vol = (7-dis)/7*100;
    
}

///////////////////////////////////////////////////////
PT_THREAD(taskUS(struct pt* pt))
{
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1)
  {
    USVol();
    if(Vol >= 70){
      state = High;
      opengate();
      reason = High;
      }
    }
    else if(Vol <= 30){
      state = Low;
      closegate();
      reason = Low;
    }
   else{
    state = Med;
      closegate();
      reason = Med;
    }
   }
    
          
    PT_DELAY(pt, 600, ts);
  }

  PT_END(pt);
}

String Mes;
///////////////////////////////////////////////////////
PT_THREAD(taskUS2(struct pt* pt))
{
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1)
  {
    USVol();
    if(state3 ==1){
      opengate();
      reason = Commanded;
    }
    else if(state3 == 2)
    {
      closegate();
      reason = Commanded;
    }
    PT_DELAY(pt,600,ts);
  }

  PT_END(pt);
}
///////////////////////////////////////////////////////
Servo myservo;
void opengate()
{
   myservo.write(20) ;
   gate = 1;
}
void closegate()
{
  myservo.write(60) ;
  gate = 0;
}

///////////////////////////////////////////////////////
void setup()
{
  myservo.attach(11);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  Serial1.begin(115200);
  Serial.begin(115200);
  PT_INIT(&pt_taskUS);
  PT_INIT(&pt_taskUS2);
  PT_INIT(&pt_taskSendSerial);
}

///////////////////////////////////////////////////////
void loop()
{
  if(state3 = 0){
    taskLED(&pt_taskUS);
  }
  
  else {
    taskLED(&pt_taskUS2);
  }

  serialEvent();
  taskSendSerial(&pt_taskSendSerial);
}
