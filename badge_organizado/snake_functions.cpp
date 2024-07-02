#include "snake_functions.h"

int tsz = 4;              //tile size, 4 recommended (for full use of screen, use powers of 2)
int xsz = 128/tsz;        //canvas size
int ysz = 64/tsz;
int ax = 0;               //apple position
int ay = 0;
int px = 16;              //player position
int py = 8;
int plen = 2;             //initial player length
int posx[64];             //array containing snake coordinates
int posy[64];
int score = 0;            //player score
int sx = 12 / tsz;        //bounds of score display in top left corner
int sy = 8 / tsz;
String scoreStr = "0";    //player score as string
double tps = 10;          //snake speed in tiles per second

void snake_game(int xdir, int ydir){

    px += xdir;                                 //new head position
      py += ydir;
      if (px < 0) px = xsz - 1;                   //wrap snake around screen
      if (px > xsz - 1) px = 0;
      if (py < 0) py = ysz - 1;
      if (py > ysz - 1) py = 0;
    
      if(px == ax && py == ay){                   //check if snake is eating an apple
        plen++;                                   //increase length
        newApple();                               //generate new apple
        score++;                                  //increase score
        scoreStr = String(score);                 //update score string
        tps += 0.4;                               //increase speed by 0.4 tiles/s
      }
    
      for (int i = 0; i < plen; i++){
        if (px == posx[i] && py == posy[i]){
          gameOver();
        }
      }
      
      oled.clear();
      drawSnake();
      drawApple();
      displayScore();
      oled.update();

}

void initialize(){                            //put snake in initial position
  for (int i = 0; i < plen; i++){
    posx[i] = px-i;
    posy[i] = py;
  }
}

void newApple(){                              //generate random pos for new apple
  ax = random(sx, xsz);                       //new pos excluding top left corner due to score display
  ay = random(sy, ysz);
  for (int i = 0; i < plen; i++){             //check if apple pos occupied by snake
    while((ax == posx[i] && ay == posy[i]) || (ax == px && ay == py)){
      newApple();                             //generate new apple recursively until not on snake
      break;
    }
  }
}

void drawSnake(){  
  for (int i = plen; i > 0; i--){             //update snake coordinates in pos array
    posx[i] = posx[i-1];
    posy[i] = posy[i-1];
  }
  posx[0] = px;                               //new head inserted in pos array
  posy[0] = py;
  
  for (int i = 0; i < plen; i++){             //draw snake from pos array
    oled.rect(posx[i]*tsz, posy[i]*tsz, posx[i]*tsz + tsz, posy[i]*tsz + tsz, OLED_FILL);
  }
}

void drawApple(){
  oled.rect(ax*tsz, ay*tsz, ax*tsz+tsz, ay*tsz+tsz, OLED_FILL);
}

void displayScore(){
  oled.setScale(1);
  oled.setCursor(0, 0);
  oled.println(scoreStr);
}

void gameOver(){                              
  for (int i = 0; i < 4; i++){  //blink snake 4 times
    oled.clear();
    for (int i = 0; i < plen; i++) oled.rect(posx[i]*tsz, posy[i]*tsz, posx[i]*tsz+tsz, posy[i]*tsz+tsz, OLED_FILL);
    drawApple();
    displayScore();
    oled.update();
    delay(300);
    oled.clear();
    drawApple();
    displayScore();
    oled.update();
    delay(300);
  }

  resetGame();
  tps = 10;
  displayScore();
  initialize();
  newApple();
}

void resetGame(){
  px = 16;                                    //reset game
  py = 8;
  xdir = 1;
  ydir = 0;
  plen = 2;
  score = 0;
  scoreStr = String(score);
  tps = 10;
}
