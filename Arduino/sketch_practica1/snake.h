#define MAX_BODY_LENGTH 12
#define GAME_AREA_WIDTH 9
#define GAME_AREA_HEIGHT 16
#define GAME_OVER_TIME 3000
#define DIRECTION_UP 0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3

/*DRIVER CONSTANTS (ALSO FOR THE MD_Parola MATRIX)*/
const int MAX_DEVICES = 2;
const int DIN = 2;
const int LOAD = 3;
const int CLK = 4;

/*OBJECT FOR GAME MATRIX*/
LedControl lc = LedControl(DIN, CLK, LOAD, MAX_DEVICES);
/*STRUCT TO CREATE A COORDENATE IN THE MATRIX (X,Y)*/
typedef struct p {
  int x;
  int y;
} Position;
/*OBJECT FOR BODY SIZE MAX 12 LEDS*/
Position body[MAX_BODY_LENGTH];
int lastInput;
int head;
int tail;
int bodyLength;
int direction;
/*OBJECT FOR SPAWN THE FOOD (X,Y)*/
Position food;
/*VARIABLE FOR GAMEOVER (HIT ITSELF)*/
int gameover = 1;
/*VARIABLE FOR GAMEOVER (HIT THE CORNERS)*/
int gameover2 = 1;
/*SCORE OF GAME*/
int score;
int elapsedTime;
bool readInput;
unsigned long previousTime;
/*CHANGE OF MATRIX (0,1)*/
bool cambio = false;

/*SETTING THE LED ON OR OFF AND CHEKING THE CORNERS OF THE MATRIX*/
void setPixel(int row, int column) {
  /*LIMIT OF HEIGHT*/
  if (row == 8) {
    gameover2 = 0;
    gameover = GAME_OVER_TIME;
    return;
  }
  /*LIMIT OF WIDTH*/
 if (column == 8 and cambio == false) {
    if(body[head].x ==8){
      gameover2 = 0;
      gameover = GAME_OVER_TIME;
      return;
    }

  } else if (column == 7 and cambio == true) {
    if(body[head].x ==7){
      gameover2 = 0;
      gameover = GAME_OVER_TIME;
      return;
    }
  }
  /*CHANGE OF MATRIX (ALREADY 2 MATRIX IN THE DRIVER)*/
  if (column < 8 ) {
    lc.setLed(0, row, column, true);
    cambio = false;
  }
  else {
    lc.setLed(1, row, column - 8, true);
    cambio = true;
  }   
}
/*CHECK IF THE POSITION OF FOOD IS CORRECTLY SPAWN*/
bool foodPositionIsValid() {
  /*POSITION OF FOOD NEED TO BE IN COORDENATES OF X>0 AND Y>0*/
  if (food.x < 0 || food.y < 0) {
    return false;
  }
  /*CHEKING THE SIZE AND POSITION OF THE SNAKE*/
  for (int i = tail; i <= (head > tail ? head : MAX_BODY_LENGTH - 1); i++) {
    if (body[i].x == food.x && body[i].y == food.y) {
      return false;
    }
  }
  /*CHECK IF THE SNAKE IS INVERTED*/
  if (head < tail) {
    for (int i = 0; i <= head; i++) {
      if (body[i].x == food.x && body[i].y == food.y) {
        return false;
      }
    }
  }

  return true;
}
/*CHECK IF THE HEAD OF THE SNAKE ISN'T HIT THE BODY OF ITSELF*/
void checkGameover() {
  for (int i = tail; i <= (head > tail ? head - 1 : MAX_BODY_LENGTH - 1); i++)  {
    if (body[head].x == body[i].x && body[head].y == body[i].y) {
      /*VARIABLES THAT CHANGE FOR ENTER IN THE GAME OVER*/
      gameover2 = 0;
      gameover = GAME_OVER_TIME;
      return;
    }
  }
  /*CHECK IF THE SNAKE IS INVERTED*/
  if (head < tail) {
    for (int i = 0; i < head; i++) {
      if (body[head].x == body[i].x && body[head].y == body[i].y) {
        /*VARIABLES THAT CHANGE FOR ENTER IN THE GAME OVER*/
        gameover2 = 0;
        gameover = GAME_OVER_TIME;
        return;
      }
    }
  }

}
/*SPAWN FOOD IN THE MATRIX AREA*/
void spawnFood() {
  while (!foodPositionIsValid()) {
    /*CREATE THE RANDOM COORDENATE AND CREATE THE OBJECT*/
    food = { random(GAME_AREA_WIDTH - 1), random(GAME_AREA_HEIGHT) };
  }
}

void draw() {

  for (int i = tail; i <= (head > tail ? head : MAX_BODY_LENGTH - 1); i++) {
    setPixel(body[i].x, body[i].y);
    delay(5);
  }

  if (head < tail)   {
    for (int i = 0; i <= head; i++) {
      setPixel(body[i].x, body[i].y);
      delay(5);
    }
  }

  lc.shutdown(0, false);
  lc.setIntensity(0, 15);  // entre 0 y 7 // lc.setIntensity(1, 15);
  lc.clearDisplay(0);  // entre 0 y 7 //  lc.clearDisplay(1);

  lc.shutdown(1, false);
  lc.setIntensity(1, 15);  // entre 0 y 7 // lc.setIntensity(1, 15);
  lc.clearDisplay(1);  // entre 0 y 7 //  lc.clearDisplay(1);

  setPixel(food.x, food.y);
  delay(5);

}

void move()  {
  tail = tail + 1 == MAX_BODY_LENGTH ? 0 : tail + 1;
  Position prevHead = body[head];
  head = head + 1 == MAX_BODY_LENGTH ? 0 : head + 1;
  body[head] = { prevHead.x + (direction == DIRECTION_LEFT ? -1 : (direction == DIRECTION_RIGHT ? 1 : 0)), prevHead.y + (direction == DIRECTION_UP ? -1 : (direction == DIRECTION_DOWN ? 1 : 0)) };
  body[head].x = body[head].x < 0 ? GAME_AREA_WIDTH - 1 : (body[head].x >= GAME_AREA_WIDTH ? 0 : body[head].x);
  body[head].y = body[head].y < 0 ? GAME_AREA_HEIGHT - 1 : (body[head].y >= GAME_AREA_HEIGHT ? 0 : body[head].y);
}

void eat() {
  if (body[head].x == food.x && body[head].y == food.y)  {
    if (bodyLength < MAX_BODY_LENGTH)    {
      bodyLength++;
      tail--;
      if (tail < 0) tail = MAX_BODY_LENGTH - 1;
    }
    score++;
    food = { -1, -1 };
    spawnFood();
  }
}

void reset() {
  /*CREATE DEFAUT SNAKE (SIZE 2 LEDS)*/
  body[0] = { 1, 1 };
  body[1] = { 1, 2 };
  /*
    body[2] = { 1,3 };
    body[3] = { 1,4 };
    body[4] = { 1,5 };
    body[5] = { 1,6 };
  */
  bodyLength = 2;
  head = 1;
  tail = 0;
  direction = DIRECTION_DOWN;
  food = { -1, -1};
  gameover = 0;
  score = 0;
  spawnFood();
  readInput = true;
}
