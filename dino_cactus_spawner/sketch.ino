
#include <LiquidCrystal.h>

//=======================================================
// LCD PINS
//=======================================================
#define RS 8
#define EN 9

#define D4 4
#define D5 5
#define D6 6
#define D7 7

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);


//=======================================================
// BUTTON
//=======================================================
int jump_btn = 12;
int jump_btn_lastState = HIGH;


//=======================================================
// TIMING
//=======================================================
unsigned long current_tms = 0;
//for dino
unsigned long previous_tms = 0;
unsigned long jump_tms = 0;
// for cactus
unsigned long cactus_tms = 0;
unsigned long cactus_move_tms;

unsigned long cactus_spawnInterval = 1500;
unsigned long cactus_moveInterval = 150;


//=======================================================
// DINO FRAMES
//=======================================================
byte dino[3][8] = {

  {
    0b00011,
    0b00010,
    0b00111,
    0b01110,
    0b11110,
    0b01010,
    0b01010
  },

  {
    0b00011,
    0b00010,
    0b00111,
    0b01110,
    0b11110,
    0b01010,
    0b01000
  },

  {
    0b00011,
    0b00010,
    0b00111,
    0b01110,
    0b11110,
    0b01010,
    0b00010
  }
};

//========================================================
// CACTUSES
//========================================================
byte C1[8] = {0b00100, 0b00101, 0b10101, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100};
byte C2[8] = {0b00000, 0b00100, 0b00100, 0b10110, 0b11110, 0b00110, 0b00110, 0b00110};
byte C3[8] = {0b00000, 0b00100, 0b00100, 0b10100, 0b11100, 0b00101, 0b00111, 0b00100};

class Cactus{
  private:

    int posX;
    const byte* pattern;
    byte charSlot;
    bool active;

  public:
    
    Cactus() {
      posX = 15;
      pattern = nullptr;
      charSlot = 0;
      active = false;
    }

    void activate(const byte* cactusPattern, byte slot){
      pattern = cactusPattern;
      charSlot = slot;
      active = true;
      posX = 15;
    }

    void deactivate(){
      active = false;
    }

    bool isActive(){
      return active;
    }

    void move(){
      
        posX--;
      
    }

    void draw(){
      lcd.setCursor(posX, 1);
      lcd.write(byte(charSlot));
    }

    void clear(){
      lcd.setCursor(posX, 1);
      lcd.print(" ");
    }

    int getPosX(){
      return posX;
    }
};

Cactus cactuses[3];

//cactusPatterns[0]  → C1
//cactusPatterns[1]  → C2
//cactusPatterns[2]  → C3
const byte* cactusPatterns[3] = {
  C1,
  C2,
  C3
};


//=======================================================
// STATES
//=======================================================
bool in_jump = false;
bool is_dinoRunning = true;

int dino_Rframe = 0;
int score = 0;

bool gameOver = false;


//=======================================================
// DINO RUNNING
//=======================================================
void dinoRun(){

  if((current_tms - previous_tms) >= 60){

    // Move to next frame
    dino_Rframe++;

    if(dino_Rframe > 2){
      dino_Rframe = 0;
    }

    // Load new frame
    lcd.createChar(0, dino[dino_Rframe]);

    // Draw dino on ground
    lcd.setCursor(0, 1);
    lcd.write(byte(0));

    // Reset animation timer
    previous_tms = current_tms;
  }
}


//=======================================================
// START JUMP
//=======================================================
void setDinoJump(){


  // Remove dino from ground
  lcd.setCursor(0, 1);
  lcd.print(" ");

  // Draw dino in air
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
}


//=======================================================
// END JUMP
//=======================================================
void clearDinoJump(){


  // Remove dino from air
  lcd.setCursor(0, 0);
  lcd.print(" ");

  // Put dino back on ground
  lcd.setCursor(0, 1);
  lcd.write(byte(0));

  // Resume running
  is_dinoRunning = true;

  // Restart animation timer
  previous_tms = current_tms;


}

//=======================================================
// CACTUS SPAWNER
//=======================================================
void spawnCactus() {

  int type = random(0, 3);

  for(int i = 0; i < 3; i++) {

    if(!cactuses[i].isActive()) {

      cactuses[i].activate(
        cactusPatterns[type],
        type + 1
      );

      break;
    }
  }
}

void updateCactuses(){

  for(int i = 0; i < 3; i++){

    if(cactuses[i].isActive()){

      cactuses[i].clear();

      cactuses[i].move();

      if(cactuses[i].getPosX() < 0){
        cactuses[i].deactivate();
      }
      else{
        cactuses[i].draw();
      }
    }
  }
}

//=======================================================
// COLLISION DETECTION
//=======================================================

void checkCollision(){

  for(int i = 0; i < 3; i++){

    if(cactuses[i].isActive()){

      if(cactuses[i].getPosX() == 0 && !in_jump){

        gameOver = true;
      }
    }
  }
}


//=======================================================
// SETUP
//=======================================================
void setup(){

  lcd.begin(16, 2);


  pinMode(jump_btn, INPUT_PULLUP);

  randomSeed(analogRead(A0));

  // Load first dino frame
  lcd.createChar(0, dino[0]);

  // load cactuses
  lcd.createChar(1, C1);
  lcd.createChar(2, C2);
  lcd.createChar(3, C3);
  
  // load score counter
  lcd.setCursor(13, 0);
  lcd.print("00");
  lcd.setCursor(15, 0);
  lcd.print(score);

  // Draw initial dino
  lcd.setCursor(0, 1);
  lcd.write(byte(0));

}

//=======================================================
// RESET GAME
//=======================================================
void resetGame(){

  // Reset game state
  gameOver = false;
  in_jump = false;
  is_dinoRunning = true;
  dino_Rframe = 0;
  score = 0;

  // Reset cactus pool
  for(int i = 0; i < 3; i++){
    cactuses[i].deactivate();
  }

  // Reset timers
  cactus_tms = current_tms;
  cactus_move_tms = current_tms;
  previous_tms = current_tms;
  jump_tms = current_tms;

  // Reset LCD
  lcd.clear();

  lcd.setCursor(13, 0);
  lcd.print("00");

  lcd.setCursor(0, 1);
  lcd.write(byte(0));
}


//=======================================================
// LOOP
//=======================================================
void loop(){

  // Current time
  current_tms = millis();

   // Read button
  int jump_btn_state = digitalRead(jump_btn);

  if(jump_btn_state == LOW && jump_btn_lastState == HIGH){

    if(gameOver){
        resetGame();
    }
    else if(!in_jump){
  //=====================================================
  // START JUMP
  //=====================================================
  
    is_dinoRunning = false;
    in_jump = true;

    jump_tms = current_tms;

    setDinoJump();
  }
}

  if(!gameOver){

  //====================================================
  // CACTUS SPAWNER
 //=====================================================
  if((current_tms - cactus_tms) >= cactus_spawnInterval){
    spawnCactus();

    cactus_tms = current_tms;
  }

  if(current_tms - cactus_move_tms >= cactus_moveInterval){

    updateCactuses();

    cactus_move_tms = current_tms;
  }


  //=====================================================
  // DINO RUNNING
  //=====================================================
  if(is_dinoRunning){

    dinoRun();
  }


  //=====================================================
  // END JUMP AFTER 500ms
  //=====================================================
  if(in_jump && (current_tms - jump_tms) >= 500){

    in_jump = false;

    clearDinoJump();
  }

  checkCollision();
  
  }

  //=====================================================
  // SAVE BUTTON STATE
  //=====================================================
  jump_btn_lastState = jump_btn_state;
}
