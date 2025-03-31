#include <Wire.h>
#include <SerLCD.h>
#include "./buttonPressed.h"
// 
// const int MUXOUT = 5;
// const int MUXS0 = 6;
// const int MUXS1 = 7;
// const int MUXS2 = 8;

// Button pins
const int BUT1 = 0;
const int BUT2 = 1;
const int BUT3 = 2;
const int BUT4 = 3;
const int BUT5 = 4;
const int BUT6 = 5;
const int BUT7 = 6;

// Motor Driver
const int MOTI1 = 7;
const int MOTI2 = 8;
const int PWM = 9;

//LCD Pins
const int SDA = 12;
const int SDC = 13;

// Speaker
const int audioPin = 15;

//Mode selection button?
const int BUT8 = 18;

// sonar pins
const int TRIG = 19;
const int ECHO = 20;

// State of the Game
enum state {
  SLEEP,
  TITLE_SCREEN,
  GAME_SCREEN,
  GAME_OVER
};

// The last button or current button being pressed
buttonPressed curr;
int position = 0; // not sure what this is

int stage = 0; // for now, 8 stages, everytime we press the correct button we increase the stage to the next value which will be whatever note needs to be played next

// Start the game in sleep mode
state gs = SLEEP;

// We need to define the challenge the use is expected to play
buttonPressed[] challenge = [A,B,C,D,E,F,G];
// so we can just check if curr == challenge[stage] stage++

// function to display to the screen
void displayToScreen() {
  lcd.clear();
  if (gs == SLEEP) {
    lcd.clear();
  }
  else if (gs == TITLE_SCREEN) {
    lcd.setCursor(0,0);
    lcd.print("Welcome to Game")
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Press Button to");
    lcd.setCursor(0,1);
    lcd.print("Start");
  } else if (gs == GAME_SCREEN) {
    lcd.setCursor(0,0);
    lcd.print("Play: ");
    lcd.print(challenge[stage]);
    lcd.setCursor(0,1);
    lcd.print("Played: ");
    if (curr == NULL) {
      lcd.print("Bruh");
    } else {
      lcd.print(curr); //may have to map this to be a character
    }
  } else if (gs == GAME_OVER) {
    lcd.setCursor(0,0);
    lcd.print("You scored: ");
    lcd.print(score);
  }
}

long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  long distance = duration * 0.034 / 2;
  return distance;
}


void playNote(int freq, int duration) {
  analogWriteFrequency(audioPin, freq);
  delay(duration);
}

void playSound() {
  analogWriteFrequency(audioPin, 1000);
  analogWriteResolution(audioPin, 8);
  analogWrite(audioPin, 127);

  // using the 4th octave
  playNote() // JUST PLAY ALL THE NOTES WITH A 400 DELAY
  playNote() // JUST PLAY ALL THE NOTES WITH A 400 DELAY
  playNote() // JUST PLAY ALL THE NOTES WITH A 400 DELAY
  playNote() // JUST PLAY ALL THE NOTES WITH A 400 DELAY
  playNote() // JUST PLAY ALL THE NOTES WITH A 400 DELAY
  playNote() // JUST PLAY ALL THE NOTES WITH A 400 DELAY
  playNote() // JUST PLAY ALL THE NOTES WITH A 400 DELAY

}

void motorControl(int speed, int time) {
  
  // move "forward" at speed (0-255) for half the period
  digitalWrite(MOTI1, HIGH);
  digitalWrite(MOTI2, LOW);
  analogWrite(PWM,speed);
  delay(time/2);

  // now we have to move backward for half the period
  digitalWrite(MOTI2, HIGH);
  digitalWrite(MOTI1, LOW);
  analogWrite(PWM,speed);
  delay(time/2);

  //this should end up being cyclical

}



// buttonPressed readButtonPressed() { 
//   //since we are using a mux for our buttons we have to cycle through all the mux select combinations and read if our value is high at 
//   for (int i = 0; i < 8 ; i++) {
//     if (i == 0) {
//       digitalWrite(MUXS0, LOW);
//       digitalWrite(MUXS1, LOW);
//       digitalWrite(MUXS2, LOW);
//     }
//     else if (i == 1) {
//       digitalWrite(MUXS0, HIGH);
//       digitalWrite(MUXS1, LOW);
//       digitalWrite(MUXS2, LOW);
//     }
//     else if (i == 2) {
//       digitalWrite(MUXS0, LOW);
//       digitalWrite(MUXS1, HIGH);
//       digitalWrite(MUXS2, LOW);
//     }
//     else if (i == 3) {
//       digitalWrite(MUXS0, HIGH);
//       digitalWrite(MUXS1, HIGH);
//       digitalWrite(MUXS2, LOW);
//     }
//     else if (i == 4) {
//       digitalWrite(MUXS0, LOW);
//       digitalWrite(MUXS1, LOW);
//       digitalWrite(MUXS2, HIGH);
//     }
//     else if (i == 5) {
//       digitalWrite(MUXS0, HIGH);
//       digitalWrite(MUXS1, LOW);
//       digitalWrite(MUXS2, HIGH);
//     }
//     else if (i == 6) {
//       digitalWrite(MUXS0, LOW);
//       digitalWrite(MUXS1, HIGH);
//       digitalWrite(MUXS2, HIGH);
//     }
//     else if (i == 7) {
//       digitalWrite(MUXS0, HIGH);
//       digitalWrite(MUXS1, HIGH);
//       digitalWrite(MUXS2, HIGH);
//     }
//     if (digitalRead(MUXOUT) == 1) {
//       return notes[i];
//     }
//     delay(1000);
//   }
// }



const int motorPin1 = 5;
const int motorPin2 = 6;
const int motorDelay = 1000;

void setup() {
  // Our setup will setup all the connections to the different peripherals
  // We can default our LCD to blit to display a welcome screen/message for the game. Write this logic in another function that is running a while True loop and only breaks when we transition or start the game
  //  pinMode(MUXS0, OUTPUT);
  //  pinMode(MUXS1, OUTPUT);
  //  pinMode(MUXS2, OUTPUT);
  //  pinMode(MUXOUT, INPUT);

  // Setup Sonar
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT)
  digitalWrite(TRIG, LOW);

  //setup motor
  pinMode(MOTI1, OUTPUT);
  pinMode(MOTI2, OUTPUT);
  pinMode(PWM, OUTPUT);

  //setup LCD
  Wire.begin(12,13);
  lcd.begin(Wire);
  lcd.clear();

  //  while (!gameStart) {
  //    if (buttonPressed) {
  //      gameStart = true;
  //      while (gameStart) {
  //        digitalWrite(motorPin1, HIGH);
  //        digitalWrite(motorPin2, LOW);
  //        delay(1000);
  //        digitalWrite(motorPin1, LOW);
  //        digitalWrite(motorPin2, HIGH);
  //        delay(1000);
  //      }
  //    }
  //  }

}

void loop() {
  unsigned long currentMillis = millis();

  // loop for everything
  displayToScreen();
  //  while (gameStart) {
  //    curr = readButtonPressed() //read the button pressed in and play the note of the button pressed
  //    if (curr != buttonPress.Start && curr != NULL)
  //      play(curr); // play whatever note is being played (write a function for this maybe)
  //
  //    displayToLCD(note[stage]);
  //
  //    if (curr == challenge[stage]) { // if the button pressed is the correct note that we are currently at, then move to the next note
  //      stage++;
  //    }
  //  }

  userDistance = measureDistance(TRIG,ECHO);
  if (gc == SLEEP && userDistance > distanceThreshold) {
    delay(2000);
    gs = TITLE_SCREEN;
  }
  while (gc != SLEEP) {
    int startButton = digitalRead(BUT8);
    if (gs == TITLE_SCREEN && startButton == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.println("Game Starting in:");
      lcd.setCursor(0,1);
      lcd.println("3");
      delay(1000);
      lcd.println("2");
      delay(1000);
      lcd.println("1");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.println("START!!!!");
      delay(500);
      gs = GAME_SCREEN;
    }

    if (gs == GAME_SCREEN) {
      motorControl(200, 2);
    }
    //  String pressedButtons = "";
    //
    //  for (int i = 0; i < 8; i++) {
    //    if (digitalRead(buttonPins[i]) == HIGH) {
    //      playNote
    //      if (pressedButtons != " ") pressedButtons += ", ";
    //      pressedButtons += String(i+1);
    //    }
    //  }
    //  if (pressedButtons == challenge[stage]) {
    //    stage++;
    //}
    int buttonA = digitalRead(BUT6); 
    int buttonB = digitalRead(BUT7);
    int buttonC = digitalRead(BUT1);
    int buttonD = digitalRead(BUT2);
    int buttonE = digitalRead(BUT3);
    int buttonF = digitalRead(BUT4);
    int buttonG = digitalRead(BUT5);

    int temp = 0; // we can correlate each button pressed with a value. A = 1, B = 2, etc.. if more than one are pressed, they add to each other so in our challenge we will stay put
    if (buttonA) {
      playNote(220, 1000);// make sure to go back and adjust these notes for frequencies
     temp += 1;
    } 
    if (buttonB) {
      playNote(247, 1000);// make sure to go back and adjust these notes for frequencies
       temp += 2;
    } 
    if (buttonC) {
      playNote(131, 1000);// make sure to go back and adjust these notes for frequencies
      temp += 3;
    } 
    if (buttonD) {
      playNote(147, 1000); // make sure to go back and adjust these notes for frequencies
      temp += 4;
    } 
    if (buttonE) {
      playNote(165, 1000); // make sure to go back and adjust these notes for frequencies
      temp += 5;
    } 
    if (buttonF) {
      playNote(175, 1000); // make sure to go back and adjust these notes for frequencies
      temp += 6;
    } 
    if (buttonG) {
      playNote(196, 1000); // make sure to go back and adjust these notes for frequencies
      temp += 7;
    } 

    // So when the correct button is pressed it should increase the stage. This should update as the displayToScreen() function is always being called
    if (temp == (challenge[stage])+1){
      stage++;
    }

    if (stage == sizeof(challenge)/sizeof(buttonPressed)) {
      gs = GAME_OVER;
    }

  }
// Constantly read the input signal in from the hall effect sensor. 




// case statement for button pressed to send what sound. 

// Scale volume/attack of sound based on the value of the hall efffect sensor  



}

// For now I think it will be best to just have one "song" for the user to playy and they can adjust the level with the challenge mode.
