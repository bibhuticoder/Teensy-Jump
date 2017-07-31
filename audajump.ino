#include <SPI.h>
#include <U8g2lib.h>
#define SPI_CLK 14
#define SAMPLE_FREQ 500     // Hz, sample rate
#define SPI_CLK 14

U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI oled(U8G2_R2, 10, 15, 16);

/* CONSTANTS */
const int BIT_DEPTH = 16;
const int AUDIO_IN = A3;
const unsigned long TIME_BETWEEN_SAMPLES = 1000000/SAMPLE_FREQ;
const unsigned long MIN_INTERVAL_TIME = 500;
const int OFFSET = 24000;
const int THRESHOLD = 8000;

/* GLOBAL VARIABLES */
elapsedMicros time_since_sample = 0;     
elapsedMillis time_since_pulse = 0;
int value = 0;    // sound reading
int state = 0;    // system state
int in_pulse = 0; // pulse state
int score;
int game = 1;

int giveRandom(int min, int max){
   randomSeed(analogRead(0));
   return random(min, max);
}

class Player{

  public:
    int x;
    int y;
    int width = 5;
    int height = 10;
    int fixedY;
    int vx;
    int vy;
    int jumpHeight;
    int maxJumpHeight;      
    int jumpSpeed;
    int state; //0:dead, 1 : running, 2:jumping Up, 3 : falling Down
    
    Player(){
      x = 50;
      y = 53;
      vy = 5;
      fixedY = 50;
      state = 0;
      jumpHeight= 0;
      maxJumpHeight = 20;
      jumpSpeed = 4;
      
    }
        
    void jump(){
      if(jumpHeight == 0) state = 2;
    }

    void draw(){

      //jump
      if(state == 2){
        if(jumpHeight < maxJumpHeight){
          y-=jumpSpeed;
          jumpHeight+=jumpSpeed;
        }else{
          state = 3;
        }
      }
      else if(state == 3){
        if(jumpHeight > 0){
          y+=jumpSpeed;
          jumpHeight-=jumpSpeed;
        }else{
          state = 1;
        }
      }      
      //draw
      oled.drawFilledEllipse(x + width/2, y + width/2, width/1.5, width/1.5, U8G2_DRAW_ALL);          
      oled.drawFrame(x, y, width, height);      
    }
  
};

class Obstacle{
  public:
    int x;
    int y;
    int width;   
    int height;
    int vx;      
    
    Obstacle(){
      x = giveRandom(150, 500);
      height = giveRandom(4, 8);
      y = 63 - height;
      width = 5;      
      vx = 4;     
    }    
   
    void draw(){      
      x -= vx;
      if(x + width < 0){
        score++;            
        x = giveRandom(128, 500);
      }    
      oled.drawFrame(x, y, width, height); 
    }
  
};

Player *player;
Obstacle *obstacles = new Obstacle[5];

void setup() {
  SPI.setSCK(SPI_CLK); 
  analogReadResolution(BIT_DEPTH);// Set ADC bit depth
  oled.begin();
  Serial.begin(9601); 

  player = new Player();
  score = 0;

  Obstacle obs;
  for(int i=0; i<2; i++){
    obs = Obstacle();
    obstacles[i] = obs;
  } 
}

void loop() {
  record_audio();
  send_from_teensy();  
  if(game == 1){ 
    oled.clearBuffer();
    drawScene();//scene  
    player->draw();//player

    //score
    oled.setFont(u8g2_font_pxplustandynewtv_8_all);
    char buffer [33];
    oled.drawStr(100, 15, itoa(score, buffer, 10));
  
    //obstacles    
    for(int i=0; i<2; i++){
      obstacles[i].draw();  
      //collission  
      if(checkCollission(player->x, player->y, player->width, player->height,obstacles[i].x, obstacles[i].y, obstacles[i].width,  obstacles[i].height)) game = 2;    
    }
    
  }
  else if(game == 2){
    oled.setFont(u8g2_font_pxplustandynewtv_8_all);  
    oled.drawStr(30, 32, "Game Over");
    oled.drawStr(35, 45, "Score: ");
    char buffer [33];
    oled.drawStr(90, 45, itoa(score, buffer, 10));
  }

   if (Serial.available() > 0) {   
    int input = Serial.read();
    if(input == 32){
      player->jump();
    }
    Serial.println(input);
   }

  oled.sendBuffer();
  delay(50);
}

int flag = 0;
void record_audio() {
  value = analogRead(AUDIO_IN);
  time_since_sample = 0;
  bool loud_sound = (value-OFFSET) > THRESHOLD;  
  in_pulse = pulse_detector(loud_sound,in_pulse);
  
  if(time_since_pulse >= MIN_INTERVAL_TIME && in_pulse == 2){
    if(flag){      
      flag = 0;
    }else{
      player->jump();
    }      
  }
  // wait till next time to read      
  while (time_since_sample <= TIME_BETWEEN_SAMPLES) delayMicroseconds(10);       
}

void send_from_teensy() {
  Serial.println(value);
}

int pulse_detector(bool ls, int in_pulse) {
  //YOUR CODE HERE
 if(in_pulse == 0 && ls == 1){
    state = 1;
    return 1;
  }
  else if(in_pulse == 1 && ls == 0){
    state = 2;
    return 2;
  }
  else if(in_pulse == 2){
    state = 0;
    return 0;
  }
}
