#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "./buttonPressed.h"

const int MUXOUT = 5
const int MUXS0 = 6
const int MUXS1 = 7
const int MUXS2 = 8
const int V_t = 5 // voltage threshold for button


buttonPressed curr = NULL;

int position = 0;
bool gameStart = false;
int stage = 0; // for now, 8 stages, everytime we press the correct button we increase the stage to the next value which will be whatever note needs to be played next

buttonPressed[] challenge = [A,B,C,D,E,F,G];
buttonPressed[] notes = [A,B,C,D,E,F,G, Start] // abcdefg


buttonPressed readButtonPressed() { 
  //since we are using a mux for our buttons we have to cycle through all the mux select combinations and read if our value is high at 
  for (int i = 0; i < 8 ; i++) {
    if (i == 0) {
      digitalWrite(MUXS0, LOW);
      digitalWrite(MUXS1, LOW);
      digitalWrite(MUXS2, LOW);
    }
    else if (i == 1) {
      digitalWrite(MUXS0, HIGH);
      digitalWrite(MUXS1, LOW);
      digitalWrite(MUXS2, LOW);
    }
    else if (i == 2) {
      digitalWrite(MUXS0, LOW);
      digitalWrite(MUXS1, HIGH);
      digitalWrite(MUXS2, LOW);
    }
    else if (i == 3) {
      digitalWrite(MUXS0, HIGH);
      digitalWrite(MUXS1, HIGH);
      digitalWrite(MUXS2, LOW);
    }
    else if (i == 4) {
      digitalWrite(MUXS0, LOW);
      digitalWrite(MUXS1, LOW);
      digitalWrite(MUXS2, HIGH);
    }
    else if (i == 5) {
      digitalWrite(MUXS0, HIGH);
      digitalWrite(MUXS1, LOW);
      digitalWrite(MUXS2, HIGH);
    }
    else if (i == 6) {
      digitalWrite(MUXS0, LOW);
      digitalWrite(MUXS1, HIGH);
      digitalWrite(MUXS2, HIGH);
    }
    else if (i == 7) {
      digitalWrite(MUXS0, HIGH);
      digitalWrite(MUXS1, HIGH);
      digitalWrite(MUXS2, HIGH);
    }
    if (digitalRead(MUXOUT) == 1) {
      return notes[i];
    }

  }
}


void printToScreen(char[] my_str) {
  lcd.clear();


}

void displayWelcomeScreen() {
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Welcome to the game");
  lcd.setCursor(2,1);
  lcd.print("Press any button to start!");
  lcd.noCursor();
}

const int motorPin1 = 5;
const int motorPin2 = 6;
const int motorDelay = 1000;

void setup() {
  // Our setup will setup all the connections to the different peripherals
  // We can default our LCD to blit to display a welcome screen/message for the game. Write this logic in another function that is running a while True loop and only breaks when we transition or start the game
  
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  

  displayWelcomeScreen();

  while (!gameStart) {
    if (buttonPressed) {
      gameStart = true;
      while (gameStart) {
        digitalWrite(motorPin1, HIGH);
        digitalWrite(motorPin2, LOW);
        delay(1000);
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, HIGH);
        delay(1000);
      }
    }
  }
  

}

void loop() {
  // loop for everything
  if (gameStart) { 
    lcd.clear();
    lcd.print("3...");
    sleep(1000);
    lcd.clear();
    lcd.print("2...");
    sleep(1000);
    lcd.clear();
    lcd.print("1...");
    sleep(1000);
    lcd.clear();
    lcd.print("Start!");
    sleep(500);
  }
  while (gameStart) {
    curr = readButtonPressed() //read the button pressed in and play the note of the button pressed
    if (curr != buttonPress.Start && curr != NULL)
      play(curr); // play whatever note is being played (write a function for this maybe)

    displayToLCD(note[stage]);

    if (curr == challenge[stage]) { // if the button pressed is the correct note that we are currently at, then move to the next note
      stage++;
    }
  }



  // Constantly read the input signal in from the hall effect sensor. 


  // case statement for button pressed to send what sound. 

  // Scale volume/attack of sound based on the value of the hall efffect sensor  



}

// For now I think it will be best to just have one "song" for the user to playy and they can adjust the level with the challenge mode.