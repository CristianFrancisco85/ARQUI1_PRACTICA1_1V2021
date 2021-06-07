#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "LedControl.h"
/*IMPORT GAME*/
#include "snake.h"
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW


/*PINES*/
const int dirPin = 5;
const int LowSpeedPin = 6;
const int HighSpeedPin = 53;
const int textMode = 7;
const int startPin = 8;
/*BUTTONS*/
const int leftPin = 11;
const int rightPin = 10;
const int upPin = 12;
const int downPin = 13;

/*MATRIX FOR TEXT*/
MD_Parola displayMatrix = MD_Parola(HARDWARE_TYPE, DIN, CLK, LOAD, MAX_DEVICES);
/*MESSAGE*/
const String message = "*TP1 - GRUPO 10 - SECCION A*";

/*TEXT SPEED*/
int textSpeed;
/*WAITING TIME FOR INDIVIDUAL LETTERS*/
int waitTime;
/*FLAG OF CHANGE FOR MAIN TEXT*/
bool changeFlag;
/*DIFFICULT OF THE GAME*/
int difficult;
/*GAME SPEED*/
int speedGame;
/*DIFFICULT SELECTED*/
bool selectionDifficult;
/*TIMERS FOR GAMEOVER TEXT*/
int timer = 38;
int timer2 = 2;
/*TIME COUNTERS*/
long CurrentTime;
long Delimiter;

/*MODE (TEXT OR GAME)*/
bool onGame;

/*SCROLL TEXT*/
void mainText() {
  /*INIT TEXT FOR SCROLLING*/
  if (changeFlag) {
    displayMatrix.displayText("*TP1 - GRUPO 10 - SECCION A*", PA_LEFT, 100, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    changeFlag = false;
  }
  /*LEFT SCROLL*/
  if (digitalRead(dirPin) == HIGH) {
    displayMatrix.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    displayMatrix.setSpeed(map(textSpeed, 1023, 0, 400, 15));
  }
  /*RIGHT SCROLL*/
  else {
    displayMatrix.setTextEffect(PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    displayMatrix.setSpeed(map(textSpeed, 1023, 0, 400, 15));
  }
  /*RESET ANIMATION*/
  if (displayMatrix.displayAnimate()) {
    displayMatrix.displayReset();
  }
}
/*INDIVIDUAL LETTERS*/
void individualLetter() {
  int i = 0;
  int j = 0;
  int auxI = 0;
  int auxJ = message.length() - 1;
  changeFlag = true;
  displayMatrix.setTextAlignment(PA_CENTER);

  while (true) {
    //Modo de izquierda a derecha
    if (digitalRead(dirPin) == HIGH) {
      for (i = auxI; i < message.length() - 1; i++) {
        auxI = 0;
        displayMatrix.print(String(message[i]) + " " + String(message[i + 1]));
        delay(500);
        displayMatrix.displayClear();
        delay(waitTime);
        if (digitalRead(textMode) == LOW) {
          displayMatrix.displayClear();
          return;
        }
        if (digitalRead(dirPin) == LOW) {
          auxJ = i + 1;
          break;
        }
        //Se verifica si hay cambio de velocidad
        setupSpeed();
      }
    }
    //Modo derecha a izquierda
    else {
      for (j = auxJ; j > 0; j--) {
        auxJ = message.length() - 1;
        displayMatrix.print(String(message[j - 1]) + " " + String(message[j]));
        delay(500);
        displayMatrix.displayClear();
        delay(waitTime);
        if (digitalRead(textMode) == LOW) {
          displayMatrix.displayClear();
          return;
        }
        if (digitalRead(dirPin) == HIGH) {
          auxI = j - 1;
          break;
        }
        //Se verifica si hay cambio de velocidad
        setupSpeed();
      }
    }

  }


}

//LETRAS INDIVIDUALES GAME OVER
void individualLetterG(String wordDisplay, int timeToWait) {
  //changeFlag = 1;
  displayMatrix.setTextAlignment(PA_CENTER);
  int longText = wordDisplay.length();
  for (int i = 0; i < longText; i++) {
    if (i == longText - 2) {
      displayMatrix.print(score);
      i++;
    } else {
      char letter = wordDisplay[i];
      displayMatrix.print(String(wordDisplay[i]) + " " + String(wordDisplay[i]));
    }
    timer--;
    delay(700);
    if (timer <= 0) {
      displayMatrix.displayClear();
      timer = 38;
      break;
    }
  }
}
void individualLetterG2() {
  for (int i = 0; i < timer2; i++) {
    if (timer <= 0) {
      displayMatrix.displayClear();
      timer = 2;
      break;
    }
    timer2--;
    displayMatrix.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    displayMatrix.print("GAME OVER SCORE" + String(score));
    delay(2000);
  }
}
/*SETUP SPEED*/
void setupSpeed() {
  if (digitalRead(LowSpeedPin) == LOW) {
    textSpeed = 150;
    waitTime = 750;
  }
  if (digitalRead(HighSpeedPin) == LOW) {
    textSpeed = 50;
    waitTime = 250;
  }
}
/*SELECTION OF THE MODE THAT IS PRESENTED THE TEXT*/
void moveText() {
  if (digitalRead(textMode) == HIGH) {
    individualLetter();
  }
  else if (digitalRead(textMode) == LOW) {
    mainText();
  }
}
/*SNAKE GAME INIT*/
void gameMode() {
  if (digitalRead(startPin) == HIGH) {
    changeToTextMode();
  } else {
    Delimiter = millis();
    CurrentTime = millis();
  }
  unsigned long currentTime = millis();
  if (!gameover) {
    draw();
    elapsedTime += currentTime - previousTime;
    if (elapsedTime > speedGame) {
      move();
      eat();
      checkGameover();
      elapsedTime = 0;
      readInput = true;
    }
    if (readInput) {
      if (digitalRead(rightPin) && !lastInput) {
        direction = (direction + 1) % 4;
        readInput = false;
      }
      if (digitalRead(leftPin) && !lastInput) {
        direction = (4 + direction - 1) % 4;
        readInput = false;
      }
    }
    lastInput = digitalRead(rightPin) || digitalRead(leftPin);
  }
  else {
    if (gameover2 != 1) {
      String gameOverText = "GAME OVER SCORE: " + String(score);
      individualLetterG(gameOverText, waitTime);
      selectionDifficult = false;
      gameover2 = 1;
    }
    if (currentTime % 800 > 400) {
      draw();
      gameover -= currentTime - previousTime;
      if (gameover <= 0) {
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
  pinMode(LowSpeedPin, INPUT);
  pinMode(HighSpeedPin, INPUT);
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


  textSpeed = 150;
  waitTime = 750;


  CurrentTime = 0;
  Delimiter = 0;

  onGame = false;
  difficult = 1;
  speedGame = 400;
  selectionDifficult = false;

}

void changeToGameMode() {
  if (CurrentTime - Delimiter >= 3000) {
    Delimiter = millis();
    CurrentTime = millis();
    onGame = true;
  } else {
    CurrentTime = millis();
  }
}


void changeToTextMode() {
  if (CurrentTime - Delimiter >= 3000) {
    Delimiter = millis();
    CurrentTime = millis();
    onGame = false;
  } else {
    CurrentTime = millis();
  }
}

void quitGameDifficult() {
  if (CurrentTime - Delimiter >= 3000) {
    Delimiter = millis();
    CurrentTime = millis();
    onGame = false;
    loop();
  } else {
    CurrentTime = millis();
  }
}
void selectDifficult() {
  displayMatrix.displayReset();
  displayMatrix.setTextAlignment(PA_CENTER);
  while (!selectionDifficult) {
    if (digitalRead(startPin) == HIGH) {
      quitGameDifficult();
    }else{
      Delimiter = millis();
      CurrentTime = millis();
    }
    displayMatrix.print("0 " + String(difficult));
    delay(200);
    if (digitalRead(upPin) == HIGH) {
      difficult++;
      speedGame = speedGame - 100;
    } else if (digitalRead(downPin) == HIGH) {
      difficult--;
      speedGame = speedGame + 100;
    }
    if (difficult > 4) {
      difficult = 1;
      speedGame = 400;
    } else if (difficult < 1) {
      difficult = 4;
      speedGame = 100;
    }
    displayMatrix.displayClear();
    Serial.println(difficult);
    if (digitalRead(rightPin) == HIGH) {
      selectionDifficult = true;
      return;
    }
  }

}
void loop() {
  setupSpeed();
  if (digitalRead(startPin) == HIGH) {
    if (onGame) {
      changeToTextMode();
    } else {
      changeToGameMode();
    }
  } else {
    Delimiter = millis();
    CurrentTime = millis();
  }
  if (onGame) {
    selectDifficult();
    gameMode();
  } else {
    moveText();
  }
}
