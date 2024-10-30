#include "tic_tac_toe.h"

int cursorX = 0;  // Column position of cursor (0-2)
int cursorY = 0;  // Row position of cursor (0-2)
char board[3][3];  // 2D array to track the Tic Tac Toe board
bool playerX = true;  // Tracks if it's player X's turn
bool isInitializing = true;

void drawBoard(bool isInitializing) {
  oled.line(15, 22, 105, 22);  // Horizontal line 1
  oled.line(15, 44, 105, 44);  // Horizontal line 2
  oled.line(40, 5, 40, 64);    // Vertical line 1
  oled.line(80, 5, 80, 64);    // Vertical line 2

  if (!isInitializing) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (board[i][j] != ' ') {
          drawSymbol(j, i, board[i][j]);  // Draw each symbol on the board
        }
      }
    }
  }

  oled.update();
}

void highlightCell(int x, int y) {
  oled.rect(x, y, x + 15, y + 15, OLED_FILL);
}

void moveCursor(int directionX, int directionY) {
  oled.rect(17 + cursorX * 34, 5 + cursorY * 21, 17 + cursorX * 34 + 15, 5 + cursorY * 21 + 15, OLED_CLEAR);

  cursorX += directionX;
  cursorY += directionY;

  if (cursorX < 0) cursorX = 0;
  if (cursorX > 2) cursorX = 2;
  if (cursorY < 0) cursorY = 0;
  if (cursorY > 2) cursorY = 2;

  highlightCell(17 + cursorX * 34, 5 + cursorY * 21);
}

void initializeBoard() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      board[i][j] = ' ';
    }
  }
}

void placeSymbol(int x, int y) {
  if (board[y][x] == ' ' && !isInitializing) {
    char symbol = playerX ? 'X' : 'O';
    board[y][x] = symbol;
    drawSymbol(x, y, symbol);
    playerX = !playerX;
  }
}

void drawSymbol(int x, int y, char symbol) {
  int posX = x * 34 + 17;
  int posY = y * 21 + 5;

  if (symbol == 'X') {
    oled.line(posX + 2, posY + 2, posX + 13, posY + 13);
    oled.line(posX + 13, posY + 2, posX + 2, posY + 13);
  } else if (symbol == 'O') {
    oled.circle(posX + 8, posY + 8, 7, OLED_STROKE);
  }
}

bool checkWin() {
  char message[10];
  bool gameEnded = false;

  // Check for win conditions
  for (int i = 0; i < 3; i++) {
    // Check rows
    if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
      strcpy(message, (board[i][0] == 'X') ? "X Won!" : "O Won!");
      gameEnded = true;
      break; // Exit the loop when a win is found
    }
    // Check columns
    if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
      strcpy(message, (board[0][i] == 'X') ? "X Won!" : "O Won!");
      gameEnded = true;
      break; // Exit the loop when a win is found
    }
  }

  // Check diagonals
  if (!gameEnded) {
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
      strcpy(message, (board[0][0] == 'X') ? "X Won!" : "O Won!");
      gameEnded = true;
    } else if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
      strcpy(message, (board[0][2] == 'X') ? "X Won!" : "O Won!");
      gameEnded = true;
    }
  }

  // Check for a tie
  if (!gameEnded) {
    bool tie = true;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (board[i][j] == ' ') {
          tie = false;
          break;
        }
      }
    }
    if (tie) {
      strcpy(message, "Tie Game!");
      gameEnded = true;
    }
  }

  // If the game has ended, show the message
  if (gameEnded) {
    oled.clear();
    //oled.setCursor(10, 10);
    oled.print(message);
    oled.update();
    delay(2000); // Delay to show message
    initializeBoard(); // Reset the board
    oled.clear(); // Clear screen after the delay
    drawBoard(true); // Redraw the board after initializing
    return true;
  }
  return false;
}


void run_tic_tac_toe() {
  initializeBoard();
  isInitializing = true;
  drawBoard(isInitializing);
  highlightCell(17 + cursorX * 34, 5 + cursorY * 21);
  back_key_pressed = false;

  while(!back_key_pressed) { 
    int directionX = 0;
    int directionY = 0;
    
    if (KeyStatus0 & 0x04) {
      directionY = 1;                                                                                                     
    }
    if (KeyStatus0 & 0x02) {
      directionY = -1;
    }
    if (KeyStatus0 & 0x08) {
      directionX = 1;
    }
    if (KeyStatus0 & 0x01) {
      directionX = -1;
    }

    if (directionX != 0 || directionY != 0) {
      moveCursor(directionX, directionY);
      drawBoard(isInitializing);
    }
    
    if (KeyStatus0 & 0x10) {
      placeSymbol(cursorX, cursorY);
      drawBoard(false);
      isInitializing = false;
    }
    
    if (checkWin()) {
      isInitializing = true;
      drawBoard(true);
    }

    ReadAndProcessKeyStatus();
    delay(10);
  }
  
  tic_tac_toe_game = false;
  back_key_pressed = false;
  delay(100);
}
