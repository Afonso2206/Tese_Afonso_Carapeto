#ifndef TIC_TAC_TOE_H
#define TIC_TAC_TOE_H

#include "oled_functions.h"

extern int cursorX;  // Column position of cursor (0-2)
extern int cursorY;  // Row position of cursor (0-2)
extern char board[3][3];  // 2D array to track the Tic Tac Toe board
extern bool playerX;  // Tracks if it's player X's turn
extern bool isInitializing;
extern bool tic_tac_toe_game;
extern bool back_key_pressed;
extern uint8_t KeyStatus0;


extern void ReadAndProcessKeyStatus(); 


void drawBoard(bool isInitializing);

void highlightCell(int x, int y);

void moveCursor(int directionX, int directionY);

void initializeBoard();

void placeSymbol(int x, int y);

void drawSymbol(int x, int y, char symbol);

void run_tic_tac_toe();

bool checkWin();

#endif
