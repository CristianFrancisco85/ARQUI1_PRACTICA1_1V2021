/*DEFINICIONES PARA EL JUEGO "SNAKE"*/
#define largoSerpiente 24
/*DIMENSIONES DE LA MATRIZ DE JUEGO*/
#define altoZona 9
#define anchoZona 16
/*TIEMPO PARA REINICIAR EL JUEGO*/
#define tiempoFinJuego 3000
/*CONTROLES PARA EL JUEGO*/
#define arriba 0
#define derecha 1
#define abajo 2
#define izquierda 3

/*CONSTANTES PARA LOS OBJETOS MATRICES (INCLUIDA MD_Parola MATRIX)*/
const int MAX_DEVICES = 2;
const int DIN = 2;
const int LOAD = 3;
const int CLK = 4;

/*MATRIZ PARA EL JUEGO*/
LedControl matrizJuego = LedControl(DIN, CLK, LOAD, MAX_DEVICES);
/*STRUCT PARA CREAR COORDENADAS EN EL JUEGO(X,Y)*/
typedef struct p {
  int x;
  int y;
} Coordenada;
/*OBJETO COORDENADA COMO ARREGLO PARA OBTENER EL TAMAÑO DE LA SNAKE*/
Coordenada cuerpo[largoSerpiente];
/*PARTE INICIAL Y FINAL DE LA SERPIETNE*/
int cabeza;
int cola;
/*LONGITUD DE LA SERPIENTE*/
int largoCuerpo;
/*DIRECCION HACIA DONDE SE DIRIGE LA SERPIENTE*/
int direccion;
/*OVJETO PARA APARECER COMIDA CON COORDENADAS (X,Y)*/
Coordenada comida;
/*FIN DE JUEGO Y REINCIAR EL PROCESO (GOLPEARSE A SI MISMO O CHOCAR CON LAS PAREDES)*/
int finJuego = 1;
/*ENTRAR EN EL TEXTO DE FIN DE JUEGO Y MOSTRAR PUNTUACION*/
int finJuego2 = 1;
/*PUNTAJE DE LA PARTIDA*/
int puntaje;
/*VARIABLES PARA EL TIEMPO DE PAUSA Y VELOCIDAD DEL JUEGO*/
int tiempoTranscurrido;
unsigned long tiempoAnterior;
/*VARIABLES PARA LA LECTURA DE CONTROLES*/
int ultimoMovimiento;
bool leerMovimiento;
/*VELOCIDAD DEL JUEGO*/
int velocidadJuego;
/*SETTING THE LED ON OR OFF AND CHEKING THE CORNERS OF THE MATRIX*/
void colocarPixel(int fila, int columna) {
  if (columna < 8 ) {
    matrizJuego.setLed(0, fila, columna, true);
  }
  else {
    matrizJuego.setLed(1, fila, columna - 8, true);
  }
}
/*METODO PARA VERIFICAR LA POSICION DE LA COMIDA*/
bool validarComida() {
  /*NO SE ACEPTAN NUMEROS NEGATIVOS*/
  if (comida.x < 0 || comida.y < 0) {
    return false;
  }
  /*NO SE ACEPTA QUE APAREZCAN ENCIMA DE LA SERPIENTE*/
  for (int i = cola; i <= (cabeza > cola ? cabeza : largoSerpiente - 1); i++) {
    if (cuerpo[i].x == comida.x && cuerpo[i].y == comida.y) {
      return false;
    }
  }
  if (cabeza < cola) {
    for (int i = 0; i <= cabeza; i++) {
      if (cuerpo[i].x == comida.x && cuerpo[i].y == comida.y) {
        return false;
      }
    }
  }
  return true;
}
/*SE VERIFICA QUE NO HAYA CHOCADO LA SERPIENTE CON ELLA MISMA*/
void verificarJuego() {
  for (int i = cola; i <= (cabeza > cola ? cabeza - 1 : largoSerpiente - 1); i++)  {
    if (cuerpo[cabeza].x == cuerpo[i].x && cuerpo[cabeza].y == cuerpo[i].y) {
      finJuego2 = 0;
      finJuego = tiempoFinJuego;
      return;
    }
  }
  if (cabeza < cola) {
    for (int i = 0; i < cabeza; i++) {
      if (cuerpo[cabeza].x == cuerpo[i].x && cuerpo[cabeza].y == cuerpo[i].y) {
        finJuego2 = 0;
        finJuego = tiempoFinJuego;
        return;
      }
    }
  }

}
/*COLOCAR LA COMIDA EN LA MATRIZ DEL JUEGO*/
void aparecerComida() {
  while (!validarComida()) {
    /*LIMITES DE LA ZONA SON 15 PARA LA ANCHURA (0-15) Y 7 PARA LA ALTURA (0-7)*/
    comida = { random(1, altoZona - 2), random(anchoZona) };
  }
}

void dibujar() {
  /*DIBUJAR EL CUERPO DE LA SERPIENTE*/
  for (int i = cola; i <= (cabeza > cola ? cabeza : largoSerpiente - 1); i++) {
    colocarPixel(cuerpo[i].x, cuerpo[i].y);
    delay(5);
  }
  if (cabeza < cola)   {
    for (int i = 0; i <= cabeza; i++) {
      colocarPixel(cuerpo[i].x, cuerpo[i].y);
      delay(5);
    }
  }
  /*REINICIAR MATRIZ 0*/
  matrizJuego.shutdown(0, false);
  matrizJuego.setIntensity(0, 15);
  matrizJuego.clearDisplay(0);
  /*REINICIAR MATRIZ 1*/
  matrizJuego.shutdown(1, false);
  matrizJuego.setIntensity(1, 15);
  matrizJuego.clearDisplay(1);
  /*DIBUJAR LA COMIDA EN LA MATRIZ*/
  colocarPixel(comida.x, comida.y);
  delay(5);

}

void mover()  {
  cola = cola + 1 == largoSerpiente ? 0 : cola + 1;
  Coordenada prevcabeza = cuerpo[cabeza];
  cabeza = cabeza + 1 == largoSerpiente ? 0 : cabeza + 1;
  cuerpo[cabeza] = { prevcabeza.x + (direccion == izquierda ? -1 : (direccion == derecha ? 1 : 0)), prevcabeza.y + (direccion == arriba ? -1 : (direccion == abajo ? 1 : 0)) };
  //cuerpo[cabeza].x = cuerpo[cabeza].x < 0 ? altoZona - 1 : (cuerpo[cabeza].x >= altoZona ? 0 : cuerpo[cabeza].x);
  cuerpo[cabeza].y = cuerpo[cabeza].y < 0 ? anchoZona - 1 : (cuerpo[cabeza].y >= anchoZona ? 0 : cuerpo[cabeza].y);

  /*CONDICIONES PARA PERDER EL JUEGO CUANDO SE SALGA DE LOS LIMITES DE LA ZONA PARA JUGAR*/
  if (cuerpo[cabeza].x < 0 || cuerpo[cabeza].x > altoZona - 2) {
    finJuego2 = 0;
    finJuego = tiempoFinJuego;
    return;
  }
  delay(5);
  if (cuerpo[cabeza].y == 8 && prevcabeza.y == 7) {
    finJuego2 = 0;
    finJuego = tiempoFinJuego;
    return;
  }
  delay(5);
  if (cuerpo[cabeza].y == 7 && prevcabeza.y == 8) {
    finJuego2 = 0;
    finJuego = tiempoFinJuego;
    return;
  }
  delay(5);

}
/*METODO PARA ALIMENTAR Y HACER CRECER A LA SERPIENTE*/
void comer() {
  /*SE VALIDA QUE LA CABEZA CRUCE POR ENCIMA DE LA COMIDA*/
  if (cuerpo[cabeza].x == comida.x && cuerpo[cabeza].y == comida.y)  {
    if (largoCuerpo < largoSerpiente)    {
      /*SINO SE HA ALCANZADO EL LARGO MAXIMO, SE AUMENTA*/
      largoCuerpo++;
      cola--;
      if (cola < 0) cola = largoSerpiente - 1;
    }
    velocidadJuego = velocidadJuego - 20;
    if (velocidadJuego <= 75) {
      velocidadJuego = 75;
    }
    /*SE AUMENTA EL PUNTAJE*/
    puntaje++;
    /*COMIDA INVALIDA PARA GENERAR UNA POSICION AL AZAR*/
    comida = { -1, -1 };
    /*APARECE NUEVA COMIDA*/
    aparecerComida();
  }
}

void reiniciar() {
  /*TAMAÑO DEFAULT DE LA SERPIENTE "2"*/
  /*APARICION RANDOM EN LA PANTALLA LED*/
  int aparicionY  = random(0, 8);
  int aparicionX = random(0, 2);
  if (aparicionX == 0) {
    cuerpo[0] = { aparicionY, 0 };
    cuerpo[1] = { aparicionY, 1 };
  } else {
    cuerpo[0] = { aparicionY, 8 };
    cuerpo[1] = { aparicionY, 9 };
  }

  largoCuerpo = 2;
  cabeza = 1;
  /*ELEMENTOS EN COLA 0*/
  cola = 0;
  direccion = abajo;
  /*COMIDA AL AZAR*/
  comida = { -1, -1};
  /*REINICIAR VARIABLE DE FIN DE JUEGO*/
  finJuego = 0;
  /*REINICIAR PUNTAJE*/
  puntaje = 0;
  /*APARECE LA NUEVA COMIDA*/
  aparecerComida();
  leerMovimiento = true;
  velocidadJuego=400;
}
