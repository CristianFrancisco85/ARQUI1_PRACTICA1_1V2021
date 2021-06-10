#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "LedControl.h"
/*IMPORTAR EL JUEGO DE LA SERPIENTE DEL ARCHIVO .h*/
#include "snake.h"
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW


/*PINES*/
const int dirPin = 5;
const int LowSpeedPin = 6;
const int HighSpeedPin = 53;
const int textMode = 7;
const int startPin = 8;
/*BOTONES*/
const int rightPin = 11;
const int leftPin = 10;
const int upPin = 12;
const int downPin = 13;

/*MATRIZ PARA EL TEXTO*/
MD_Parola displayMatrix = MD_Parola(HARDWARE_TYPE, DIN, CLK, LOAD, MAX_DEVICES);
/*MENSAJE*/
const String message = "*TP1 - GRUPO 10 - SECCION A*";

/*TEXT SPEED*/
int textSpeed;
/*WAITING TIME FOR INDIVIDUAL LETTERS*/
int waitTime;
/*FLAG OF CHANGE FOR MAIN TEXT*/
bool changeFlag;
/*nivel OF THE GAME*/
int nivel;
/*nivel SELECTED*/
bool dificultad;
/*TIMERS FOR finJuego TEXT*/
int timer = 38;
int timer2 = 2;
/*TIME COUNTERS*/
long CurrentTime;
long Delimiter;

/*MODE (TEXT OR GAME)*/
bool onGame;

/*INICIAR BARRIDO DE TEXTO*/
void mainText() {
  /*TEXTO INICIAL PARA MOSTRAR EN LA MATRIZ*/
  if (changeFlag) {
    displayMatrix.displayText("*TP1 - GRUPO 10 - SECCION N*", PA_CENTER, 100, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    changeFlag = false;
  }
  /*MOVER HACIA LA IZQUIERDA*/
  if (digitalRead(dirPin) == HIGH) {
    displayMatrix.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    displayMatrix.setSpeed(map(textSpeed, 1023, 0, 400, 15));
  }
  /*MOVER HACIA LA DERECHA*/
  else {
    displayMatrix.setTextEffect(PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    displayMatrix.setSpeed(map(textSpeed, 1023, 0, 400, 15));
  }
  /*REINICIAR LA ANIMACION DE LA PANTALLA*/
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
void textoFinJuego() {
  displayMatrix.displayText("GAME OVER YOUR SCORE IS:", PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  while (digitalRead(startPin) == LOW) {
    displayMatrix.setSpeed(map(150, 1023, 0, 400, 15));
    if (displayMatrix.displayAnimate()) {
      displayMatrix.setTextAlignment(PA_CENTER);
      displayMatrix.print(puntaje);
      delay(300);
      displayMatrix.displayText("GAME OVER YOUR SCORE IS:", PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      displayMatrix.displayReset();
    }
  }
  delay(200);
  displayMatrix.displayReset();
  //displayMatrix.displayText("*TP1 - GRUPO 10 - SECCION N*", PA_CENTER, 100, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
  onGame = false;
  changeFlag = true;
  delay(200);
  loop();
}
void textoPausa() {
  delay(700);
  displayMatrix.displayText("PAUSE CURRENT SCORE:", PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  while (digitalRead(startPin) == LOW) {
    displayMatrix.setSpeed(map(150, 1023, 0, 400, 15));
    if (displayMatrix.displayAnimate()) {
      displayMatrix.setTextAlignment(PA_CENTER);
      displayMatrix.print(puntaje);
      delay(300);
      displayMatrix.displayText("PAUSE CURRENT SCORE:", PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      displayMatrix.displayReset();
    }
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
void moverText() {
  if (digitalRead(textMode) == HIGH) {
    individualLetter();
  }
  else if (digitalRead(textMode) == LOW) {
    mainText();
  }
}
/*INICIAR EL JUEGO*/
void gameMode() {
  /*VALIDACION DE LOS 3 SEGUNDOS PARA CAMBIAR AL MODO TEXTO*/
  if (digitalRead(startPin) == HIGH) {
    changeToTextMode();
  } else {
    Delimiter = millis();
    CurrentTime = millis();
  }
  unsigned long tiempoActual = millis();
  /*SI NO SE HA LLEGADO A UNA CONDICION DE PERDER EL JUEGO SEGUIRA DIBUJANDO LA SERPIENTE EN LA MATRIZ*/
  if (!finJuego) {
    dibujar();
    /*VELOCIDAD DEL JUEGO DEPENDIENDO DE LA DIFICULTAD*/
    tiempoTranscurrido += tiempoActual - tiempoAnterior;
    if (tiempoTranscurrido > velocidadJuego) {
      mover();
      comer();
      verificarJuego();
      tiempoTranscurrido = 0;
      leerMovimiento = true;
    }
    /*LECTURA DE LOS CONTROLES PARA JUGAR LA SNAKE*/
    if (leerMovimiento) {
      if (digitalRead(rightPin) && !ultimoMovimiento) {
        if (direccion != arriba) {
          direccion = abajo;
        }
        leerMovimiento = false;
      }
      if (digitalRead(leftPin) && !ultimoMovimiento) {
        if (direccion != abajo) {
          direccion = arriba;
        }
        leerMovimiento = false;
      }
      if (digitalRead(upPin) && !ultimoMovimiento) {
        if (direccion != derecha) {
          direccion = izquierda;
        }
         leerMovimiento = false;
      }
      if (digitalRead(downPin) && !ultimoMovimiento) {
        if (direccion != izquierda) {
          direccion = derecha;
        }
        leerMovimiento = false;
      }
    }
    ultimoMovimiento = digitalRead(rightPin) || digitalRead(leftPin) || digitalRead(upPin) || digitalRead(downPin);
  }
  else {
    if (finJuego2 != 1) {
      finJuego2 = 1;
      delay(200);
      textoFinJuego();
      dificultad = false;
      reiniciar();
      return;
    }
    if (tiempoActual % 800 > 400) {
      dibujar();
      finJuego -= tiempoActual - tiempoAnterior;
      if (finJuego <= 0) {
        reiniciar();
      }
    }
  }
  tiempoAnterior = tiempoActual;
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
  nivel = 1;
  velocidadJuego = 400;
  dificultad = false;

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
    dificultad = false;
    reiniciar();
    onGame = false;
  } else if ((CurrentTime - Delimiter <= 2000) and (CurrentTime - Delimiter >= 1000)) {
    textoPausa();
    Delimiter = millis();
    CurrentTime = millis();
  } else {
    CurrentTime = millis();
  }
}


void selecionarDificultad() {
  while (!dificultad and onGame) {
    displayMatrix.setTextAlignment(PA_CENTER);
    displayMatrix.print("0 " + String(nivel));
    delay(200);
    if (digitalRead(upPin) == HIGH) {
      nivel++;
      velocidadJuego = velocidadJuego - 100;
    } else if (digitalRead(downPin) == HIGH) {
      nivel--;
      velocidadJuego = velocidadJuego + 100;
    }
    if (nivel > 4) {
      nivel = 1;
      velocidadJuego = 400;
    } else if (nivel < 1) {
      nivel = 4;
      velocidadJuego = 100;
    }
    displayMatrix.displayClear();
    if (digitalRead(rightPin) == HIGH) {
      dificultad = true;
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
    selecionarDificultad();
    gameMode();
  } else {
    moverText();
  }
}
