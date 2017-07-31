//int giveRandom(int min, int max){
//   randomSeed(analogRead(0));
//   return random(min, max);
//}

void drawScene(){  
  oled.drawFilledEllipse(50, 6, 3, 3, U8G2_DRAW_ALL);
  oled.drawLine(0, 63, 128, 63);
//  oled.drawFrame(0, 0, 128, 63);
}

bool checkCollission(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){
//  return ((x1 < x2+w2) && (x1 + w1 > x2) && (y1 > y2+h2) && (y1+h1 < y2));
return !(x1 > x2+w2 || x1+w1 < x2 || y1 > y2+h2 || y1+h1 < y2);
    
}
