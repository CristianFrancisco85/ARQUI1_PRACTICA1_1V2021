#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "LedControl.h"
#include "juego.h"
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW


//DEFINICION DE PINES
const int dirPin = 5;
const int speedPin = 6;
const int textMode = 7;
const int startPin = 8;
//BOTONES PARA JUEGO
const int leftPin = 11;
const int rightPin = 10;
const int upPin = 12;
const int downPin = 13;

//MATRIZ 
MD_Parola displayMatrix = MD_Parola(HARDWARE_TYPE, DIN, CLK, LOAD, MAX_DEVICES);
//MENSAJE A MOSTRAR
const String message = "*TP1 - GRUPO 10 - SECCION A*";

//VELOCIDAD DEL TEXTO
int textSpeed; 
//TIEMPO DE ESPERA ENTRE LETRAS INDIVIDUALES   
int waitTime;
//BANDERA DE CAMBIO DE TEXTO    
bool changeFlag;

int timer = 38;
int timer2 = 2;


//TEXTO CON MOVIMIENTO
void mainText(){
  if(changeFlag){
    displayMatrix.displayText("*TP1 - GRUPO 10 - SECCION A*", PA_LEFT, 100, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    changeFlag = false;
  }
  //Modo de izquierda a derecha
  if(digitalRead(dirPin)==HIGH){
    displayMatrix.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT); 
    displayMatrix.setSpeed(map(textSpeed, 1023, 0, 400,15));
  }
  //Modo derecha a izquierda
  else{
    displayMatrix.setTextEffect(PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    displayMatrix.setSpeed(map(textSpeed, 1023, 0, 400,15));
  }
  //Se reinicia animacion
  if(displayMatrix.displayAnimate()){
    displayMatrix.displayReset();
  }
}
//LETRAS INDIVIDUALES
void individualLetter(){
  int i=0;
  int j=0;
  int auxI=0;
  int auxJ=message.length()-1;
  changeFlag = true;
  displayMatrix.setTextAlignment(PA_CENTER);
  
  while(true){
    //Modo de izquierda a derecha
    if(digitalRead(dirPin)==HIGH){
      for(i=auxI; i<message.length()-1;i++){
        auxI=0;
        displayMatrix.print(String(message[i])+" "+String(message[i+1]));
        delay(500);
        displayMatrix.displayClear();
        delay(waitTime);
        if(digitalRead(textMode) == LOW){
          displayMatrix.displayClear();
          return;
        }
        if(digitalRead(dirPin)==LOW){
          auxJ=i+1;
          break;
        }
        //Se verifica si hay cambio de velocidad
        setupSpeed();
      }
    }
    //Modo derecha a izquierda
    else{
      for(j=auxJ;j>0;j--){
        auxJ=message.length()-1;
        displayMatrix.print(String(message[j-1])+" "+String(message[j]));
        delay(500);
        displayMatrix.displayClear();
        delay(waitTime);
        if(digitalRead(textMode) == LOW){
          displayMatrix.displayClear();
          return;
        }
        if(digitalRead(dirPin)==HIGH){
          auxI=j-1;
          break;
        }
        //Se verifica si hay cambio de velocidad
        setupSpeed();
      }
    }
    
  }
  

}

//LETRAS INDIVIDUALES GAME OVER 
void individualLetterG(String wordDisplay, int timeToWait){
  //changeFlag = 1;
  displayMatrix.setTextAlignment(PA_CENTER);
  int longText = wordDisplay.length();
  for(int i=0; i<longText;i++){
    if(i== longText-2){
      displayMatrix.print(score);
      i++;
    }else{
      char letter = wordDisplay[i];
      displayMatrix.print(String(wordDisplay[i])+" "+String(wordDisplay[i]));
    }
    timer--;
    delay(700);
    if(timer <=0){
      displayMatrix.displayClear();
      timer = 38;
      break;
    }
  }
}
void individualLetterG2(){
  for(int i=0; i<timer2;i++){
    if(timer <=0){
      displayMatrix.displayClear();
      timer = 2;
      break;
    }
    timer2--;
    displayMatrix.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    displayMatrix.print("GAME OVER SCORE"+String(score));
    delay(2000);
  }
}
//CONFIGURA LA VELOCIDAD
void setupSpeed(){
  if(digitalRead(speedPin)== LOW){
    textSpeed = 150;
    waitTime = 750;
  }
  else{
    textSpeed = 50;
    waitTime = 250;
  }
}

void moveText(){
  if(digitalRead(textMode)== HIGH){
    individualLetter();
  }
  else if(digitalRead(textMode)== LOW){
    mainText();
  }
}

void gameMode(){
  unsigned long currentTime = millis(); 
  if(!gameover){
    draw();
    elapsedTime += currentTime - previousTime;
    if(elapsedTime > 500){
       move();
       eat();
       checkGameover();
       elapsedTime = 0;
       readInput = true;
    }
    if(readInput){
      if(digitalRead(rightPin) && !lastInput) {  
        direction = (direction + 1) % 4; 
        readInput = false; 
      }
      if(digitalRead(leftPin) && !lastInput) { 
        direction = (4 + direction-1) % 4; 
        readInput = false; 
      }
    }
    lastInput = digitalRead(rightPin) || digitalRead(leftPin);     
  }
  else{
    if(gameover2 != 1){
      String gameOverText = "GAME OVER SCORE: "+String(score);
      individualLetterG(gameOverText,waitTime);
      gameover2 = 1;
    }
    if(currentTime % 800 > 400){
      draw();
      gameover -= currentTime - previousTime;
      if(gameover <= 0){
        reset();
      } 
    }  
  }
    previousTime = currentTime;
}

//*************************************************************************************************************************************************
void setup() {
  
  Serial.begin(9600);
  //PINES
  pinMode(speedPin, INPUT);
  pinMode(dirPin, INPUT);
  pinMode(textMode, INPUT);
  pinMode(startPin, INPUT);
  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);
  pinMode(upPin, INPUT);
  pinMode(downPin, INPUT);
  
  changeFlag = true;
  displayMatrix.begin();
  displayMatrix.setIntensity(5);
  displayMatrix.displayClear();

}

void loop(){
  setupSpeed();
  if(digitalRead(startPin)== HIGH){
    gameMode();
  }
  else{
    moveText();
  }
}
