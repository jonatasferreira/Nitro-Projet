
const int RH = A0;
const int RV = A1;
const int LH = A2;
const int LV = A3;

const int R1 = 2;
const int R2 = 3;
const int R3 = 4;
const int L1 = 5;
const int L2 = 6;
const int L3 = 7;
const int X = 8;  // X
const int Q = 9;  // Quadrado - Q
const int O = 10; // Redondo - O
const int T = 11; // Triangulo - T

int valueRH = 0;
int valueRV = 0;
int valueLH = 0;
int valueLV = 0;

int newRH;
int newRV;
int newLH;
int newLV;

const int err = 7;
int delayButtonJoystick = 0;
int delayButtonCircuit = 0;
int delayButtonStop = 0;
int delayButtonBreak = 0;
int delayButtonDiscover = 0;
int delayButtonFarol = 0;

int delayButton_X = 0;
int delayButton_Q = 0;
int delayButton_O = 0;
int delayButton_T = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  pinMode(R1, INPUT);
  pinMode(R2, INPUT);
  pinMode(R3, INPUT);
  pinMode(L1, INPUT);
  pinMode(L2, INPUT);
  pinMode(L3, INPUT);
  
  pinMode(X, INPUT);
  pinMode(Q, INPUT);
  pinMode(O, INPUT);
  pinMode(T, INPUT);
}


void loop() {
  buttonState();
  ponteciometerLeft();
  ponteciometerRight();
}


void buttonState(){
  if( ! digitalRead(R1)){ // LOW button is pressed.   function joystick
    if(delayButtonJoystick > 600){
      Serial.println("j");
      delayButtonJoystick = 0;
    }
  }
  if( ! digitalRead(R2)){ // LOW button is pressed.   function circuit
    if(delayButtonCircuit > 600){
      Serial.println("c");
      delayButtonCircuit = 0;
    }
  }
  if( ! digitalRead(R3)){ // LOW button is pressed.
    if(delayButtonStop > 600){
      Serial.println("NRS");
      delayButtonStop = 0;
    }
  }
  if( ! digitalRead(L1)){ // LOW button is pressed.   end break/function
    if(delayButtonBreak > 600){
      Serial.println("b");
      delayButtonBreak = 0;
    }
  }
  if( ! digitalRead(L2)){ // LOW button is pressed.   function discover
    if(delayButtonDiscover > 600){
      Serial.println("d");
      delayButtonDiscover = 0;
    }
  }
  if( ! digitalRead(L3)){ // LOW button is pressed.   farol
    if(delayButtonFarol > 600){
      Serial.println("NLF");
      delayButtonFarol = 0;
    }
  }
  if( ! digitalRead(X)){ // LOW button is pressed.   X
    if(delayButton_X > 300){
      Serial.println("NRX");
      delayButton_X = 0;
    }
  }
  if( ! digitalRead(Q)){ // LOW button is pressed.   Quadrado - Q
    if(delayButton_Q > 600){
      Serial.println("NRQ");
      delayButton_Q = 0;
    }
  }
  if( ! digitalRead(O)){ // LOW button is pressed.   Redondo - O
    if(delayButton_O > 300){
      Serial.println("NRO");
      delayButton_O = 0;
    }
  }
  if( ! digitalRead(T)){ // LOW button is pressed.   Triangulo - T
    if(delayButton_T > 600){
      Serial.println("NRT");
      delayButton_T = 0;
    }
  }
  if(delayButtonJoystick < 602) delayButtonJoystick ++;    // delay button joystick
  if(delayButtonCircuit < 602) delayButtonCircuit ++;      // delay button circuit
  if(delayButtonStop < 602) delayButtonStop ++;
  if(delayButtonBreak < 602) delayButtonBreak ++;
  if(delayButtonDiscover < 602) delayButtonDiscover ++;
  if(delayButtonFarol < 602) delayButtonFarol ++;
  
  if(delayButton_X < 302) delayButton_X ++;
  if(delayButton_Q < 602) delayButton_Q ++;
  if(delayButton_O < 302) delayButton_O ++;
  if(delayButton_T < 602) delayButton_T ++;
}


void ponteciometerLeft(){
  newLH = analogRead(LH);
  newLV = analogRead(LV);
  
  if(valueLH <= newLH-err || valueLH >= newLH+err){
    valueLH = newLH;
    Serial.print("NLH-");
    Serial.print(map(newLH, 0, 1023, 45, 130));
    Serial.println("-");
  }
/*  if(valueLV <= newLV-err || valueLV >= newLV+err){
    valueLV = newLV;
    Serial.print("NLV-");                //  command potentiometer not used
    Serial.print(map(newLV, 0, 1023, 0, 180));
    Serial.println("-");
  }  */
}

const int limMenorCamVert = 70;
const int limMaiorCamVert = 160;
int aux;
void ponteciometerRight(){
  newRH = analogRead(RH);
  newRV = analogRead(RV);
  
  if(valueRH <= newRH-err || valueRH >= newRH+err){
    valueRH = newRH;
    Serial.print("NRH-");              //  command potentiometer camera mov. horizontal
    Serial.print(map(newRH, 0, 1023, 0, 150));
    Serial.println("-");
  }
  if(valueRV <= newRV-err || valueRV >= newRV+err){
    valueRV = newRV;
    Serial.print("NRV-");              //  command potentiometer camera mov. vertical
    aux = map(newRV, 0, 1023, 180, 50);
    if(aux < limMenorCamVert){
      Serial.print(limMenorCamVert);
    }else{
      if(aux > limMaiorCamVert){
        Serial.print(limMaiorCamVert);
      }else{
        Serial.print(aux);
      }
    }
    Serial.println("-");
  }
}






