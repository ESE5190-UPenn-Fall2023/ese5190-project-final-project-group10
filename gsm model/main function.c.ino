#include <MKRGSM.h>

#include "arduino_secrets.h" 
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instance
GSM gsmAccess; // include a 'true' parameter for debug enabled
GSMVoiceCall vcs;

String remoteNumber = "";  // the number you will call
char charbuffer[20];

char Number;
bool Hang = false;
bool WaitCom = true;
bool pin0Value = false;
bool pin1Value = false;
bool pin2Value = false;
int CommValue;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Hello\n");
  pinMode(6,OUTPUT);
  bool connected = false;

  while (!connected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      connected = true;
    } else {
      delay(1000);
      Serial.print("Not Connected\n");
    }
  }
  digitalWrite(6,HIGH);
  Serial.print("Connected\n");
}

void loop() {
 // while (Serial.available() > 0) {}
    Serial.print("In Loop\n");
    pinMode(0,INPUT);
    pinMode(1,INPUT);
    pinMode(2,OUTPUT);
    digitalWrite(2,LOW);
    while (true){
      pin0Value = digitalRead(0);
      if (pin0Value){
        ReadCom();
      }
        //remoteNumber = "3059032533";
        //com_Phonecall();
        //Serial.print("End\n");
    }
}


void com_ClearNum() {
  remoteNumber = "";
}

void com_Phonecall() {
  if (remoteNumber.length() == 10)
  {
    // Call
    remoteNumber.toCharArray(charbuffer, 20); 
    // Check if picked up the call
    if (vcs.voiceCall(charbuffer)) 
    {
      // Turn on LED
      while(!(Hang)){
        pin0Value = digitalRead(0);
        if (pin0Value == 1){
          ReadCom();
        }
      }
      vcs.hangCall();
      Hang = false;
      // Turn off LED
    }
  }
}

void ReadCom() {
  //Serial.print("Star\n");
  bool Comm = 0;
  CommValue = 1;
  Comm = digitalRead(1);
  if (Comm){
    CommValue = 11;
    //Serial.print("Get 1\n");
  }
  else{
    CommValue = 10;
    //Serial.print("Get 0\n");
  }
  for (int i=0; i<3; i++){
    delay(10);
    digitalWrite(2,HIGH);
    //Serial.print("2 High\n");
    delay(10);
    while (pin0Value){
      pin0Value = digitalRead(0);
      }
    //Serial.print("0 Low\n");
    delay(10);
    digitalWrite(2,LOW);
    //Serial.print("2 Low\n");
    delay(10);
    while (!(pin0Value)){
      pin0Value = digitalRead(0);
      }
    //Serial.print("0 High\n");
    delay(10);
    Comm = digitalRead(1);
    if (Comm){
      CommValue = CommValue*10+1;
      //Serial.print("Get 1\n");
    }
    else{
      CommValue = CommValue*10;
      //Serial.print("Get 0\n");
    }
  }
  delay(10);
  digitalWrite(2,HIGH);
  delay(10);
  while (pin0Value){
    pin0Value = digitalRead(0);
    }
  delay(10);
  digitalWrite(2,LOW);
  //Serial.print("End\n");
  DoCom();
}

void DoCom() {
  if (CommValue == 11111){
    com_ClearNum();
    Serial.print("Clear\n");
  }
  else if(CommValue == 11110){
    com_Phonecall();
    com_ClearNum();
    Serial.print("Call\n");
  }

  else if(CommValue == 11101){
    Hang = true;
    Serial.print("Hang\n");
  }

  else if(CommValue == 10000){
    Number = '0';
    remoteNumber += Number;
    Serial.print("0\n");
  }

  else if(CommValue == 10001){
    Number = '1';
    remoteNumber += Number;
    Serial.print("1\n");
  }

  else if(CommValue == 10010){
    Number = '2';
    remoteNumber += Number;
    Serial.print("2\n");
  }

  else if(CommValue == 10011){
    Number = '3';
    remoteNumber += Number;
    Serial.print("3\n");
  }

  else if(CommValue == 10100){
    Number = '4';
    remoteNumber += Number;
    Serial.print("4\n");
  }

  else if(CommValue == 10101){
    Number = '5';
    remoteNumber += Number;
    Serial.print("5\n");
  }

  else if(CommValue == 10110){
    Number = '6';
    remoteNumber += Number;
    Serial.print("6\n");
  }

  else if(CommValue == 10111){
    Number = '7';
    remoteNumber += Number;
    Serial.print("7\n");
  }

  else if(CommValue == 11000){
    Number = '8';
    remoteNumber += Number;
    Serial.print("8\n");
  }

  else if(CommValue == 11001){
    Number = '9';
    remoteNumber += Number;
    Serial.print("9\n");
  }
}


