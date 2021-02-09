/* "Introduction to the Arduino Platform"
 * ECE 487 Lab 1 
 * 
 * Author:  Hannah Perkins
 * CWID:    11472559
 * Date:    30 January 2021
 * Course:  ECE 487
 * 
 * Function:      Blink onboard and offboard LEDs out of phase.
 *  
 * Description:   Application will prompt user for input of 'g'
 *        to start blinking LEDs or 's' to stop blinking LEDs. 
 *        During Blinking operation, onboard LED will be ON for
 *        1 second and OFF for 2 seconds. Offboard LED will be 
 *        exactly out-of-phase: OFF for 1 second and ON for 2
 *        seconds. LEDs will continuously blink in this mode 
 *        until new user input is provided to Serial Monitor.
 *        
 *        Incorrect input during Blinking operation will result
 *        in error message as well as the blinking to stop until 
 *        correct input is entered.
 *        
 * Hardware Setup:  Use pin 9 for the Offboard LED.
 *        
 * Arduino Version: 1.8.14
 */

void setup() {
  // Initialize onboard and offboard LED Pins
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);

  // Initialize LEDs to OFF
  digitalWrite(13, LOW);
  digitalWrite(9, LOW);

  // Initialize serial communication (9600 bits/sec)
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }

  // Send instructions to User
  initialPrompt();
}

void loop() {
  // Initialize input string variable for user command
  String inputString;
  // Initialize blinkMode bool to OFF
  bool blinkMode = false;
  
  // Get input from Serial Monitor
  while (Serial.available() > 0) {
    inputString = Serial.readString();
    
    // Print User's input back to Serial Monitor
    printUserInput(inputString);

    // Use user input to find out operating mode for blinkMode
    blinkMode = checkInput(inputString);

    // Blink LEDs if TRUE
    while(blinkMode) {
      // Blink LEDs on Hardware
      blinkLeds();
      
      // Check for changes to input
      if (Serial.available() > 0) {
        // Read input String and print input to Serial Monitor
        inputString = Serial.readString();
        printUserInput(inputString);

        // Update BlinkMode with new input
        blinkMode = checkInput(inputString);
      }
    }
  }
}

/* Check User Input 
 * Function:    Interpret Input from Serial Monitor
 * Description: This function takes an input string and determines the 
 *              operating mode for blinkMode. TRUE means that the LEDs
 *              should be blinking whereas FALSE will mean that the LEDs
 *              need to be off.
 *              This function also handles errors for incorrect input
 *              length and incorrect input characters by sending error 
 *              messages and prompting for new input.
 * Parameters:  String    (inputString)
 * Return:      bool      (blinkMode) 
 */
bool checkInput(String inputString) {
  // Check for valid length
  if (inputString.length() != 2) {
    // Send Error Messages
    invalidLength();
    initialPrompt();
    // Set blinkMode to OFF
    return false;
  }

  // Check if user entered 'g'
  if (inputString == "g\n") {
    ledsOnMessage();
    // Set blinkMode to ON
    return true;
  }

  // Check if user entered 's'
  else if (inputString == "s\n") {
    ledsOff();
    // Set blinkMode to OFF
    return false;
  }

  // Handle all other options
  else {
    // Send Error Message
    invalidChar();
    initialPrompt();
    // Set blinkMode to OFF
    return false; 
  }
}

// Print and Send Back User Input to Serial Monitor
void printUserInput(String inputString) {
  Serial.println("\nYou entered: ");
  Serial.println(inputString);
}

// Prompt User for input
void initialPrompt() {
  Serial.println("\nPlease enter 'g' to begin blinking LEDs"
    " and 's' to stop blinking LEDs.");
}

// Invalid Input Length Message
void invalidLength() {
  Serial.println("ERROR: \tInvalid length entered."
    " Expected length of 1 character.");
  // Turn LEDs Off
  ledsOff();
}

// Invalid Input Character Message
void invalidChar() {
  Serial.println("ERROR: \tInvalid character entered. "
    "Expected 'g' or 's'");
  // Turn LEDs Off
  ledsOff();
}

// Turn LEDs ON Message
void ledsOnMessage() {
  Serial.println("Turning LEDs ON...");
  Serial.println("LED blinking Mode: \tON");
}

// Turn LEDs OFF 
void ledsOff() {
  // Send Message to User via Serial Monitor
  Serial.println("Turning LEDs OFF...");
  Serial.println("LED blinking Mode: \tOFF");
  
  // Turn off all LEDs on Hardware
  digitalWrite(13, LOW);
  digitalWrite(9, LOW);
  delay(1000);
}

// Blink 2 LEDS out-of-phase of eachother
void blinkLeds() {
  // Onboard LED ON & Offboard LED OFF for 1 second
  digitalWrite(13, HIGH);
  digitalWrite(9, LOW);
  delay(1000);
  
  // Onboard LED OFF & Offboard LED ON for 2 seconds
  digitalWrite(13, LOW);
  digitalWrite(9, HIGH);
  delay(2000);
}
