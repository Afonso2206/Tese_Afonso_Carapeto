// работаем с буфером. Выводим битмап, обновляем только в области изменения
// получаем огромную скорость обновления!

const uint8_t bitmap_32x32[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xE0, 0xF0, 0x70, 0x70, 0x30, 0x30, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF0, 0x70, 0x30, 0x30, 0x20, 0x00, 0x00,
  0x00, 0x30, 0x78, 0xFC, 0x7F, 0x3F, 0x0F, 0x0F, 0x1F, 0x3C, 0x78, 0xF0, 0xE0, 0xC0, 0x80, 0x80, 0x80, 0x40, 0xE0, 0xF0, 0xF8, 0xFC, 0xFF, 0x7F, 0x33, 0x13, 0x1E, 0x1C, 0x1C, 0x0E, 0x07, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xF7, 0xEF, 0x5F, 0x3F, 0x7F, 0xFE, 0xFD, 0xFB, 0xF1, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x1E, 0x33, 0x33, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x1F, 0x0E, 0x04, 0x00, 0x00, 0x00, 0x00,
};

#include <GyverOLED.h>
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
// можно передать адрес: GyverOLED oled(0x3C);

void setup() {
  oled.init();
  Wire.setClock(400000L);
}

int posX = 64, posY = 32; // начальная позиция
int velX = 4, velY = 3;   // скорость
void loop() {
  // движение с отскоками. Заходим на половину за стенки! Для теста
  posX += velX;
  posY += velY;
  if (posX >= 128 - 16 || posX <= -16) velX = -velX;
  if (posY >= 64 - 16 || posY <= -16) velY = -velY;

  // выводим картинку
  oled.drawBitmap(posX, posY, bitmap_32x32, 32, 32);

  // обновляем буфер только в области изменения (+-4х и +-3y от рисования)
  oled.update(posX - 4, posY - 3, posX + 32 + 4, posY + 32 + 3);

  // очищаем буфер
  oled.clear(posX, posY, posX + 32, posY + 32);
}
