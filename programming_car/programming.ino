#include <Servo.h>

    //Servos
Servo servoWheel;      // Port 2  #  WARNING : Limit 5 a 150 
Servo myESC;           // Port 3
Servo servoCamHor;     // Port 6
Servo servoCamVer;     // Port 7
Servo servoSonarUp;    // Port 8
Servo servoSonarFront; // Port 9

    //Wheel
const int limWheelRigth = 5;
const int limWheelLeft  = 150;
const int middleWheel   = 77;

    //ESC
const int ESCBack       = 76;  //  Run back
const int ESCNeutral    = 90;  //  Neutral position
const int ESCBeginFront = 105; //  Run inital front

    //Digital ports
const int wheelPin      = 2;
const int ESCPin        = 3;
const int trigUp        = 4;
const int echoUp        = 5;
const int camHorPin     = 6;
const int camVerPin     = 7;
const int sonarUpPin    = 8;
const int sonarFrontPin = 9;
const int echoFront     = 10;
const int trigFront     = 11;
const int ligthFront    = 12;
const int ligthBack     = 13;

    //Analog ports
const int luminosity    = A0;
const int odometerPin   = A1;


void setup(){
  Serial.begin(9600);
  
  // servos
  pinMode(sonarFrontPin,OUTPUT);
  pinMode(sonarUpPin,OUTPUT);
  pinMode(wheelPin,OUTPUT);
  // ESC
  pinMode(ESCPin,OUTPUT);
  // sonar
  pinMode(echoFront,INPUT);
  pinMode(trigFront,OUTPUT);  
  pinMode(echoUp,INPUT);
  pinMode(trigUp,OUTPUT);  
  pinMode(camHorPin,INPUT);
  pinMode(camVerPin,OUTPUT);  
  // ligth
  pinMode(ligthFront,OUTPUT);
  pinMode(ligthBack,OUTPUT);
  // sensor
  pinMode(odometerPin,INPUT);
  
  servoWheel.attach(wheelPin);
  servoSonarUp.attach(sonarUpPin);
  servoSonarFront.attach(sonarFrontPin);  
  servoCamHor.attach(camHorPin);
  servoCamVer.attach(camVerPin);
  myESC.attach(ESCPin);
  
  myESC.write(ESCNeutral);
  armESC();
}



// ------------------- Variable Global -------------------------

    //  loop
char readSerial;

    //  remote & joystick
boolean farol = false;
int actualVelocity;

    //  discover & circuit
float distanceUp, distanceFront, distanceBack;
int numReadingsMaior, numReadingsMenor;
int velocity = 0, velocityCar = 0;
float minObstacleFront = 70.0;
boolean turnFlag = false;
boolean sideWallFlag;
long initialDistance;
boolean odometerFlag;
int odometerReading;
int odometerCont;
int history[6]; 
float media;
float tolerance = 10.0;
int valWheel;
float dif;

// -------------------------------------------------------------

void setupCircuito(){
  odometerCont = 0;
  numReadingsMaior = 0;
  numReadingsMenor = 0;
  // Read the initial value of the odometer
  odometerReading = analogRead(odometerPin);
  if(odometerReading > 100){//  reading odometerReading = HIGH
      odometerFlag = true;
  }
  else{                    //  reading odometerReading = LOW
      odometerFlag = false;
  }
  
  servoWheel.write(middleWheel);
  servoSonarFront.write(90);
  servoCamHor.write(90);
  servoCamVer.write(90);
  
  servoSonarUp.write(160);
  delay(500);
  long distanceRigth = getDistanceSonarUp();
  servoSonarUp.write(20);
  delay(500);
  long distanceLeft = getDistanceSonarUp();
  
  if(distanceRigth < distanceLeft){
    sideWallFlag = true;            // right side wall
    initialDistance = distanceRigth;
    servoSonarUp.write(160);
    servoSonarFront.write(65);
  }
  else{
    sideWallFlag = false;          // left side wall
    initialDistance = distanceLeft;
    servoSonarUp.write(20);
    servoSonarFront.write(115);
  }
  for(int i = 0; i < 5; i = i + 1) {
    history[i] = initialDistance;
  }
  
  Serial.println();
  Serial.print("    initialDistance = ");
  Serial.println(initialDistance);
  Serial.println();
}






void setupDiscovery(){
  odometerCont = 0;
  // Read the initial value of the odometer
  odometerReading = analogRead(odometerPin);
  if(odometerReading > 100){//  reading odometerReading = HIGH
      odometerFlag = true;
  }
  else{                    //  reading odometerReading = LOW
      odometerFlag = false;
  }
  
  servoWheel.write(middleWheel);
  servoSonarFront.write(90);
  servoSonarUp.write(90);
  servoCamHor.write(90);
  servoCamVer.write(90);
  delay(300);
}







void loop(){
  speedCar(0);
  Serial.flush();
  digitalWrite(ligthBack,HIGH);
  servoWheel.write(middleWheel);
  servoSonarFront.write(90);
  servoSonarUp.write(90);
  servoCamHor.write(90);
  servoCamVer.write(180);
  
  readSerial = readingSerial(); // Reading serial 
  
  switch(readSerial) {
    case 'c':            // circuito
      digitalWrite(ligthBack,LOW);
      circuit();
      break;
    case 'd':            // discovery
      discovery();
      break;
    case 'r':            // remote control
      remote();
      break;
    case 'j':            // joystick control
      joystick();
      break;
  }
}






void joystick(){
  
  char c = 'x';
  int value;
  int t = 0;
  int aux;
  servoWheel.write(middleWheel);
  servoSonarFront.write(90);
  servoSonarUp.write(90);
  servoCamHor.write(90);
  servoCamVer.write(90);
  
  Serial.flush();
  
  while(true){                                     //  loop joystick
    c = readingSerial();
    if(c == 'N'){
	c = readingSerial();
      switch(c){
        case 'L':// ----------------- Left side of the joystick ---------------------
          c = readingSerial();
          switch(c){
            case 'F':
              //------------ command button farol -----------------
              farol = ! farol;
              digitalWrite(ligthFront,farol);
              delay(500);
              break; 
            case 'H':
              //--------- command potentiometer wheel -------------
              c = readingSerial();
              if(c == '-'){
                value = 0;
                aux = 0;
                for( t=0; t<=3  &&  aux >= 0  &&  aux <= 9; t++){
                  aux = readingSerial();
                  aux = aux - 48;
                  if(aux >= 0  &&  aux <= 9){
                    if(value == 0){
                      value = aux;
                    }
                    else{
                      value = (value * 10) + aux;
                    }
                  }
                }
                servoWheel.write(value);
              }
              break;
            case 'V':
              //---------- command potentiometer not used ----------
              break;
          }
          break;
        case 'R':// -------------------- Right side of the joystick --------------------
	    c = readingSerial();
          switch(c){
            case 'Q':
              //------------ command button stop/Nitro ---------------
              speedCar(0);
              velocityCar = 0;
              break;
            case 'X':
              //------- command button X - Up velocity ------------
              speedCar(++velocityCar);
              break;
            case 'O':
              //------- command button redondo - Dow velocity --------
              if(velocityCar > -1) speedCar(--velocityCar);
              break;
            case 'T':
              //------------ command button triangulo ---------------
              break;
            case 'H':
              //---- command potentiometer camera mov. horizontal -------
              c = readingSerial();
              if(c == '-'){
                value = 0;
                aux = 0;
                for( t=0; t<=3  &&  aux >= 0  &&  aux <= 9; t++){
                  aux = readingSerial();
                  aux = aux - 48;
                  if(aux >= 0  &&  aux <= 9){
                    if(value == 0){
                      value = aux;
                    }
                    else{
                      value = (value * 10) + aux;
                    }
                  }
                }
                servoCamHor.write(value);
              }
              break;
            case 'V':
		//------- command potentiometer camera mov. vertical ---------
              c = readingSerial();
              if(c == '-'){
                value = 0;
                aux = 0;
                for( t=0; t<=3  &&  aux >= 0  &&  aux <= 9; t++){
                  aux = readingSerial();
                  aux = aux - 48;
                  if(aux >= 0  &&  aux <= 9){
                    if(value == 0){
                      value = aux;
                    }
                    else{
                      value = (value * 10) + aux;
                    }
                  }
                }
                servoCamVer.write(value);
              }
              break;
          }
          break;
      }
    }// end if
    if(c == 'b') break;// end joystick
  }// end while
}

char readingSerial(){
  boolean flag = false;
  do{
    if(Serial.available() > 0){
      char h = Serial.read();                      // read and delete the character
      return h;
    }
    else{
      flag = true;
    }
  }while(flag);
}





void remote(){
  long distance;
  readSerial = '-';
  Serial.flush();
  servoWheel.write(middleWheel);
  servoSonarFront.write(90);
  servoSonarUp.write(90);
  servoCamHor.write(90);
  servoCamVer.write(90);
  
  while(true){
    if(readSerial == 'b') break;// end remote
      
    for(int angleSonar = 0; angleSonar < 180; angleSonar +=1) {  // going left to right.

      servoSonarFront.write(angleSonar);
      delay(50);
      distance = getDistanceSonarFront();

      Serial.print("X");                                   // print leading X to mark the following value as degrees
      Serial.print(angleSonar);                            // current servo position
      Serial.print("V");                                   // preceeding character to separate values
      Serial.println(distance);                            // average of sensor readings
      Serial.print("W");                                   // preceeding character to separate values
      Serial.println(actualVelocity);                      // velocity
      
      readSerial = Serial.read();                          // read serial      
      if(readSerial == 'b') break;
      doAnything(readSerial);                              // make action
    }
  
    //start going right to left after we got to 180 degrees
    //same code as above
  
    for(int angleSonar = 180; angleSonar > 0 && readSerial != 'b'; angleSonar -=1) {  // going right to left
      
      servoSonarFront.write(angleSonar);
      delay(50);
      distance = getDistanceSonarFront();

      Serial.print("X");
      Serial.print(angleSonar);
      Serial.print("V");
      Serial.println(distance);
      Serial.print("W");                                   // preceeding character to separate values
      Serial.println(actualVelocity);                      // velocity
      
      readSerial = Serial.read();
      if(readSerial == 'b') break;
      doAnything(readSerial);
    }  
  }
}



/*
* Make action
*/
void doAnything(char entry){
  switch(entry) {
      case '0':
        speedCar(0);
        actualVelocity = 0;
        break;
      case '1':
        speedCar(1);
        actualVelocity = 1;
        break;
      case '2':
        speedCar(2);
        actualVelocity = 2;
        break;
      case '3':
        speedCar(3);
        actualVelocity = 3;
        break;
      case '4':
        speedCar(4);
        actualVelocity = 4;
        break;
      case '5':
        speedCar(5);
        actualVelocity = 5;
        break;
      case '6':
        speedCar(6);
        actualVelocity = 6;
        break;
      case '7':
        speedCar(7);
        actualVelocity = 7;
        break;
      case 'i':
        servoWheel.write(limWheelLeft); //  Full turn left
        break;
      case 'o':
        servoWheel.write(middleWheel);
        break;
      case 'p':
        servoWheel.write(limWheelRigth); //  Full turn right
        break;
      case 'f':
        farol = ! farol;
        digitalWrite(ligthFront,farol);
        break;
    }
}






void discovery(){
/*
  setupDiscovery();
  readSerial = '-';
  Serial.flush();
  
  while(true){
    
    readSerial = Serial.read(); // Reading serial
    if(readSerial == 'b') break; // end discovery
    
    // Implement discovery function here
    
  }
*/
}






void circuit(){
  
  setupCircuito();
  readSerial = '-';
  Serial.flush();
  
  while(true){   //   loop circuito
    
    readSerial = Serial.read();  // Reading serial 
    if(readSerial == 'b') break; // end circuit
  
    // --------  Code responsible for handling the odometer  -------------------

    odometer();

    Serial.print("odometerCont = ");
    Serial.print(odometerCont);

    // --------  Code responsible for sonar  -------------------

    delay(5); // minimum delay between each reading 
    distanceFront = getDistanceSonarFront();
    delay(5); // minimum delay between each reading
    distanceUp = getDistanceSonarUp();
    
    distanceUp = smooth(distanceUp);
  
    Serial.print("    distanceFront = ");
    Serial.print(distanceFront);
    Serial.print("    distanceUp = ");
    Serial.print(distanceUp);

    // --------  Code responsible for handling the speed  -------------------

    if(turnFlag == false){                                // If you are not doing curve
      if(distanceFront == 0.0){                           // If the reading is infinite front.
        if(velocityCar < 7) speedCar(++velocityCar);      // Speeds of 1 in 1 to MAX Speed
      }else{
        velocity = distanceFront / 57;                    // Calculation of speed
        if(velocity > velocityCar){                       // If the calculated speed is GREATER than the current
          if(velocityCar < 7) speedCar(++velocityCar);    // Speeds of 1 on 1
        }else if(velocity < velocityCar){                 // If the calculated speed is LOWER than the current
          if(velocityCar > 1)  speedCar( --velocityCar ); // Reduces 1 on 1
        }
        else{}                                            // Calculated speed equal to the current
      }
      Serial.print("    turnFlag = ");
      Serial.print("false");
    }
    else{
      if(velocityCar > 1)  speedCar( --velocityCar );     // Reduces to the first gear
      else speedCar( velocityCar = 1 );
      Serial.print("    turnFlag = ");
      Serial.print("true ");
    }
  
    Serial.print("    velocityCar = ");
    Serial.print(velocityCar);
    Serial.println();

    // --------  Code responsible for handling objects in front  -------------------
  
    if(distanceFront < minObstacleFront && distanceFront != 0.0){
      numReadingsMaior = numReadingsMaior + 1;
    }
    else{
      if(numReadingsMenor >= 2 || numReadingsMaior <= 2){
        numReadingsMaior = 0;
      }
      else{
        numReadingsMenor ++;
      }
    }
    if(numReadingsMaior >= 3){                       // There is an obstacle ahead
      if( ! ((distanceUp - initialDistance) > 40) ){ // If there sidewall
        distanceUp = initialDistance / 4;            // Forces away from the sidewall
      }
    }

    // --------  Code responsible for turning left or right  -------------------

    // --------- Approaches the side wall

    if(distanceUp > initialDistance){
      dif = distanceUp - initialDistance;
      if(dif > 20){
        turnFlag = true;
        if(sideWallFlag == true){          // Right side wall
          servoWheel.write(limWheelRigth); // Full turn toward the wall
        }else{                             // Left side wall
          servoWheel.write(limWheelLeft);  // Full turn toward the wall
        }
      }
      else{
        valWheel = 36 * cos(dif / (6 + 4/11) ) + 41;     // Equation CosX
        //valWheel = - ( (dif * dif) / (89/16) ) + 77 ;  // Equation X^2  --  Is not good, needs some adjustments
        if(valWheel > 41){
          turnFlag = false;
        }else{
          turnFlag = true;
        }
        if(sideWallFlag == true){                // Right side wall
          servoWheel.write(valWheel);            // Turning toward the wall
        }else{                                   // Left side wall
          servoWheel.write(77 + (77 - valWheel));// Turning toward the wall
        }
      }
    
    // --------- Distance from the side wall

    }else if(distanceUp < initialDistance){
      dif = initialDistance - distanceUp;
      if(dif > 20){
        turnFlag = true;
        if(sideWallFlag == true){          // Right side wall
          servoWheel.write(limWheelLeft);  // Full turn to the opposite side wall
        }else{                             // Left side wall
          servoWheel.write(limWheelRigth); // Full turn to the opposite side wall
        }
      }
      else{
        valWheel = (154) - ( 36 * cos(dif / (6 + 4/11) ) + 41 ); // Equation CosX
        //valWheel =  ( (dif * dif) / (89/16) ) + 77;            // Equation X^2  --  Is not good, needs some adjustments
        if(valWheel < 113){
          turnFlag = false;
        }else{
          turnFlag = true;
        }
        if(sideWallFlag == true){                // Right side wall
          servoWheel.write(valWheel);            // Turning to the opposite side wall
        }else{                                   // Left side wall
          servoWheel.write(77 - (valWheel - 77));// Turning to the opposite side wall
        }
      }
    
    // --------- Wheel Front

    }else{
      servoWheel.write(middleWheel);
    }
  }
}

boolean firstError = false;

int smooth(int input){
  for(int i=0; i<4; i++){
    history[i] = history[i+1];
  }// update
  history[4] = input;
  
  float mont = 0.0;
  mont += history[0] * 0.1;
  mont += history[1] * 0.2;
  mont += history[2] * 0.3;
  mont += history[3] * 0.4;
  mont += history[4] * 1.0;
  
  media = mont/2;
  tolerance = input - media;
  
  if(tolerance < 0.0){
    tolerance *= -1;
  }
  
  if(input == initialDistance){
    firstError = false;
    return input;
  }else if(input > 2500 && !firstError){
    firstError = true;
    return history[3];
  }else if(input < history[3] + tolerance && (input > history[3] - tolerance || input > tolerance - history[3])){
    firstError = false;
    return input;
  }else{
    firstError = false;
    return media;
  }
}





void odometer(){
  odometerReading = analogRead(odometerPin);
  if(odometerReading > 100 && odometerFlag == false){//  reading odometerReading = HIGH
    odometerCont ++;
    odometerFlag = true;
  }
  else{                   //  reading odometerReading = LOW
    if(odometerFlag == true){
      odometerCont ++;
      odometerFlag = false;
    }
  }
}

long getDistanceSonarFront(){
   long duration, cm; 
   digitalWrite(trigFront, LOW); 
   delayMicroseconds(2); 
   digitalWrite(trigFront, HIGH);
   delayMicroseconds(10); 
   digitalWrite(trigFront, LOW); 
   delayMicroseconds(2);
   duration = pulseIn(echoFront, HIGH);
   cm = duration/29/2;
   return cm;
}

long getDistanceSonarUp(){
   long duration, cm; 
   digitalWrite(trigUp, LOW); 
   delayMicroseconds(2); 
   digitalWrite(trigUp, HIGH);
   delayMicroseconds(10); 
   digitalWrite(trigUp, LOW); 
   delayMicroseconds(2);
   duration = pulseIn(echoUp, HIGH);
   cm = duration/29/2;
   return cm;
}

/*     Rear Sonar Off
long getDistanceSonarBack(){
   long duration, cm; 
   digitalWrite(trigBack, LOW); 
   delayMicroseconds(2); 
   digitalWrite(trigBack, HIGH);
   delayMicroseconds(10); 
   digitalWrite(trigBack, LOW); 
   delayMicroseconds(2);
   duration = pulseIn(echoBack, HIGH);
   cm = duration/29/2;
   return cm;
}   */

void speedCar(int gait){
  switch(gait){
    case -1:
      myESC.write(ESCNeutral); // stationary
      delay(200);
      myESC.write(ESCBack);
      break;
    case 0:
      myESC.write(ESCNeutral); // stationary
      break;
    case 1:
      myESC.write(ESCBeginFront);// minimum speed
      break;
    case 2:
      myESC.write(ESCBeginFront+1);
      break;
    case 3:
      myESC.write(ESCBeginFront+2);
      break;
    case 4:
      myESC.write(ESCBeginFront+3);
      break;
    case 5:
      myESC.write(ESCBeginFront+4);
      break;
    case 6:
      myESC.write(ESCBeginFront+5);
      break;
     case 7:
      myESC.write(ESCBeginFront+6);
      break;
     case 8:
      myESC.write(ESCBeginFront+7);
      break;
    default:
      if(gait >= 9){
        myESC.write(ESCBeginFront+8);
      }
  }
}

void armESC(){    //Arming sequence of sending a minimum or 'joystick zero' for 4 seconds
               //       to simulate radio gear is ON and operational. myESC.write(ESCbegin);
               
  myESC.write(200);          // Max front
  delay(2000); //Delay for the ESC to activate.
  myESC.write(20);          // Min back
  delay(2000); //Delay for the ESC to activate.
  myESC.write(ESCNeutral);  // Neutral
  delay(2000); //Delay for the ESC to activate.
}





