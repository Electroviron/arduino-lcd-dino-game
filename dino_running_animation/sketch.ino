
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
unsigned long previous_tms = 0;
unsigned long jump_tms = 0;


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


//=======================================================
// STATES
//=======================================================
bool in_jump = false;
bool is_dinoRunning = true;

int dino_Rframe = 0;


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
// SETUP
//=======================================================
void setup(){

  lcd.begin(16, 2);


  pinMode(jump_btn, INPUT_PULLUP);

  // Load first dino frame
  lcd.createChar(0, dino[0]);

  // Draw initial dino
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


  //=====================================================
  // DINO RUNNING
  //=====================================================
  if(is_dinoRunning){

    dinoRun();
  }


  //=====================================================
  // START JUMP
  //=====================================================
  if(!in_jump &&
     jump_btn_state == LOW &&
     jump_btn_lastState == HIGH){

    is_dinoRunning = false;

    in_jump = true;

    jump_tms = current_tms;

    setDinoJump();
  }


  //=====================================================
  // END JUMP AFTER 500ms
  //=====================================================
  if(in_jump && (current_tms - jump_tms) >= 500){

    in_jump = false;

    clearDinoJump();
  }


  //=====================================================
  // SAVE BUTTON STATE
  //=====================================================
  jump_btn_lastState = jump_btn_state;
}
