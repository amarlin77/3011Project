#include <Wire.h>
#include <SerLCD.h>

// Create an instance of the SerLCD display
SerLCD lcd;

// -----------------------
// Pin assignments (update these to match your wiring)
// -----------------------
const int sonarTriggerPin = 10;   // Sonar trigger pin
const int sonarEchoPin = 0;      // Sonar echo pin

// Piano keys pins (assuming 7 keys)
const int keyPins[7] = {4, 2, 5, 1, 12, 15, 14};

// Start button pin (one of the 8 push buttons)
const int startButtonPin = 13;

// Motor control pins for TB6612FNG (example pins)
const int motorPWMPin = 16;
const int motorA1Pin = 17;
const int motorA2Pin = 15;

// Speaker pin (using tone() for note playback)
const int speakerPin = 4;

// -----------------------
// Game States
// -----------------------
enum GameState { SLEEP, TITLE_SCREEN, GAME, END };
GameState gameState = SLEEP;

// Note structure for mapping keys to notes and frequencies (3rd octave)
struct Note {
  char letter;      // e.g., 'C', 'D', etc.
  int frequency;    // frequency in Hz for 3rd octave
};

// Example note mapping for 7 piano keys (update frequencies as needed)
Note notes[7] = {
  { 'C', 130 },
  { 'D', 146 },
  { 'E', 164 },
  { 'F', 174 },
  { 'G', 196 },
  { 'A', 220 },
  { 'B', 246 }
};

// The sequence (challenge) to play - indices refer to notes[]
int challengeSequence[] = {0, 2, 4, 5, 4, 2, 0}; // Example sequence
const int challengeLength = sizeof(challengeSequence) / sizeof(challengeSequence[0]);
int currentChallengeIndex = 0;

// Motor metronome variables
unsigned long lastMotorUpdate = 0;
const unsigned long motorInterval = 500; // milliseconds
bool motorDirection = true;  // true: one direction, false: the other

// -----------------------
// Setup
// -----------------------
void setup() {
  Serial.begin(115200);
  
  // Initialize LCD over I2C.
  // Adjust the dimensions (columns, rows) as per your display.
  Wire.begin(6,7);
  lcd.begin(Wire);
  lcd.clear();
  
  // Set pin modes for sonar
  pinMode(sonarTriggerPin, OUTPUT);
  pinMode(sonarEchoPin, INPUT);

  // Set pin modes for keys and button (with internal pull-downs for active HIGH)
  for (int i = 0; i < 7; i++) {
    pinMode(keyPins[i], INPUT_PULLDOWN);
  }
  pinMode(startButtonPin, INPUT_PULLDOWN);

  // Motor pins
  pinMode(motorPWMPin, OUTPUT);
  pinMode(motorA1Pin, OUTPUT);
  pinMode(motorA2Pin, OUTPUT);

  // Speaker pin will be used by tone()

  // Start in SLEEP state - clear LCD (already cleared)
}

// -----------------------
// Main loop
// -----------------------
void loop() {
  switch (gameState) {
    case SLEEP:
      // Remain in sleep until sonar detects a presence.
      if (sonarTriggered()) {
        gameState = TITLE_SCREEN;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Press Start");
        Serial.println("Transition to TITLE_SCREEN");
      }
      break;

    case TITLE_SCREEN:
      // Wait for the start button to be pressed.
      if (buttonPressed(startButtonPin)) {
        gameState = GAME;
        currentChallengeIndex = 0;
        lcd.clear();
        Serial.println("Game started");
      }
      break;

    case GAME: {
      updateMotorMetronome();
      
      // Display the current note to play on the top line of the LCD
      char noteToPlay = notes[ challengeSequence[currentChallengeIndex] ].letter;
      lcd.setCursor(0, 0);
      lcd.print("Play: ");
      lcd.print(noteToPlay);
      lcd.print("   "); // clear extra characters

      // Check if any piano key is pressed
      int pressedKeyIndex = checkPianoKeys();
      if (pressedKeyIndex != -1) {
        // Play the note (regardless if it’s correct or not)
        playNote(pressedKeyIndex);

        // Display pressed note and its frequency on second line
        lcd.setCursor(0, 1);
        lcd.print("Got: ");
        lcd.print(notes[pressedKeyIndex].letter);
        lcd.print(" ");
        lcd.print(notes[pressedKeyIndex].frequency);
        lcd.print("Hz   "); // clear extra characters

        // If correct key, move to next note in challenge
        if (pressedKeyIndex == challengeSequence[currentChallengeIndex]) {
          currentChallengeIndex++;
          if (currentChallengeIndex >= challengeLength) {
            // Challenge complete, go to END state
            gameState = END;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Play again? Y/N");
          }
        }
        delay(300);  // simple debounce and delay after a key press
      }
      break;
    }

    case END:
      // In end state, wait for user to choose to play again.
      // For this example, pressing the start button will restart the game.
      if (buttonPressed(startButtonPin)) {
        gameState = GAME;
        currentChallengeIndex = 0;
        lcd.clear();
      }
      break;
  }
}

// -----------------------
// Function Definitions
// -----------------------

// Check if a button is pressed (assuming active LOW with internal pull-ups)
bool buttonPressed(int pin) {
  return digitalRead(pin) == LOW;
}

// Check if sonar sensor is triggered (e.g., if an object is within a threshold distance)
bool sonarTriggered() {
  long duration;
  int distance;
  
  // Clear trigger
  digitalWrite(sonarTriggerPin, LOW);
  delayMicroseconds(2);
  // Trigger pulse
  digitalWrite(sonarTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sonarTriggerPin, LOW);
  
  // Read echo time in microseconds
  duration = pulseIn(sonarEchoPin, HIGH, 30000); // timeout after 30ms
  // Convert to distance in cm (approximate conversion)
  distance = duration * 0.034 / 2;
  
  // For example, trigger if object is closer than 50cm
  if (distance > 0 && distance < 40) {
    return true;
  }
  return false;
}

// Check which piano key is pressed. Returns the index (0-6) or -1 if none.
int checkPianoKeys() {
  for (int i = 0; i < 7; i++) {
    if (digitalRead(keyPins[i]) == HIGH) { // assuming active LOW
      return i;
    }
  }
  return -1;
}

// Play the note corresponding to the given piano key index.
void playNote(int keyIndex) {
  int freq = notes[keyIndex].frequency;
  tone(speakerPin, freq, 200);  // play note for 200ms
}

// Update motor direction for metronome effect
void updateMotorMetronome() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastMotorUpdate >= motorInterval) {
    lastMotorUpdate = currentMillis;
    // Toggle motor direction
    motorDirection = !motorDirection;
    if (motorDirection) {
      // Set motor to turn one way
      digitalWrite(motorA1Pin, HIGH);
      digitalWrite(motorA2Pin, LOW);
      analogWrite(motorPWMPin, 128);  // Adjust PWM value (0-255) for speed
    } else {
      // Reverse direction
      digitalWrite(motorA1Pin, LOW);
      digitalWrite(motorA2Pin, HIGH);
      analogWrite(motorPWMPin, 128);
    }
  }
}
