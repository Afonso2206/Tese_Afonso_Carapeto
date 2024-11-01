#ifndef SNAKE_H
#define SNAKE_H

#include "oled.h"

extern int xdir;             //player direction in snake game
extern int ydir;
extern int tsz;              //tile size, 4 recommended (for full use of screen, use powers of 2)
extern int xsz;        //canvas size
extern int ysz;
extern int ax;               //apple position
extern int ay;
extern int px;              //player position
extern int py;
extern int plen;             //initial player length
extern int posx[64];             //array containing snake coordinates
extern int posy[64];
extern int score;            //player score
extern int sx;        //bounds of score display in top left corner
extern int sy;
extern String scoreStr;    //player score as string
extern double tps;          //snake speed in tiles per second

extern bool Snake_Game;
extern bool back_key_pressed;
extern uint8_t KeyStatus0;


extern void ReadAndProcessKeyStatus(); 


void snake_game(int xdir, int ydir);

void initialize();

void newApple();

void drawSnake();

void drawApple();

void displayScore();

void gameOver();

void resetGame();

void run_snake_game();

#endif
