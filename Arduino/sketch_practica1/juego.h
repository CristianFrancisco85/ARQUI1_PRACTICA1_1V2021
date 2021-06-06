#define MAX_BODY_LENGTH 12
#define GAME_AREA_WIDTH 9
#define GAME_AREA_HEIGHT 16
#define GAME_OVER_TIME 3000
#define DIRECTION_UP 0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3

//CONSTANTES PARA LAS SALIDAS DEL DRIVER
const int MAX_DEVICES = 2;
const int DIN = 2;
const int LOAD = 3; 
const int CLK = 4;
//OBJETO MATRIZ PARA EL JUEGO
LedControl lc = LedControl(DIN, CLK, LOAD, MAX_DEVICES);
typedef struct p{
  int x;
  int y;
} Position;

Position body[MAX_BODY_LENGTH];
int lastInput;
int head;
int tail;
int bodyLength;
int direction;
Position food;
int gameover=1;
int gameover2=1;
int score;
int elapsedTime;
bool readInput;
unsigned long previousTime;


void setPixel(int row, int column){
  if(row == 8){  
    gameover2= 0;
    gameover = GAME_OVER_TIME;
    return;
  }  
  if (column <8 ){
    lc.setLed(0, row, column, true);  
  }
  else{ 
    lc.setLed(1, row, column-8, true);
  }    
}

bool foodPositionIsValid(){
  if (food.x < 0 || food.y < 0){
    return false;
  } 

  for (int i = tail; i <= (head > tail ? head : MAX_BODY_LENGTH - 1); i++){
    if (body[i].x == food.x && body[i].y == food.y){
      return false; 
    } 
  }

  if (head < tail){
    for (int i = 0; i <= head; i++){
      if (body[i].x == food.x && body[i].y == food.y){
        return false; 
      }    
    }
  }
  
  return true;
}

void checkGameover(){
  for (int i = tail; i <= (head > tail ? head - 1 : MAX_BODY_LENGTH - 1); i++)  {
    if (body[head].x == body[i].x && body[head].y == body[i].y){
      gameover2= 0;
      gameover = GAME_OVER_TIME;
      return;
    }
  }

  if (head < tail){
    for (int i = 0; i < head; i++){
      if (body[head].x == body[i].x && body[head].y == body[i].y){
        gameover2= 0;
        gameover = GAME_OVER_TIME;
        return;
      }
    }
  }
  
}

void spawnFood() {
  while (!foodPositionIsValid()){
    food = { random(GAME_AREA_WIDTH-1), random(GAME_AREA_HEIGHT) };
  }
}

void draw(){

  for (int i = tail; i <= (head > tail ? head : MAX_BODY_LENGTH - 1); i++){
    setPixel(body[i].x, body[i].y);
    delay(5);
  }
  
  if (head < tail)   {
    for (int i = 0; i <= head; i++){
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

void reset(){
  body[0] = { 1,1 };
  body[1] = { 1,2 };
//  body[2] = { 1,3 };
//  body[3] = { 1,4 };
//  body[4] = { 1,5 };
//  body[5] = { 1,6 };

  bodyLength = 2;
  head = 1;
  tail = 0;
  direction = DIRECTION_DOWN;
  food = {-1, -1};
  gameover = 0;
  score = 0;
  spawnFood();
  readInput = true;
}
