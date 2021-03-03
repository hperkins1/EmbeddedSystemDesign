/**************************************************************
 *  "Introduction to Watchdog Timers"
 * ECE 487 Lab 3 
 * 
 * Author:  Hannah Perkins
 * CWID:    11472559
 * Date:    25 February 2021
 * Course:  ECE 487
 * 
 * Function:      The program will echo the hexadecimal 
 *        equivalent of user's valid input to Serial Monitor 
 *        and seven-segment display. The program also displays
 *        the reaction time it took for the user to enter 
 *        input after the prompt was given. A watchdog timer 
 *        is used such that if no input is entered 4 seconds
 *        after the prompt, then the board will reset.
 *  
 * Description:   The application will prompt the user to enter 
 *        a number from 0 to 15 and then display that number on
 *        both the Serial Monitor and the seven-segment display.
 *        The reaction time it took for the user to enter input
 *        after the prompt will also be displayed on the Serial 
 *        Monitor.
 *        
 *        Out of Range input values result in an error message  
 *        to the Serial Monitor and 3 horizontal lines on the 
 *        seven-segment display. The watchdog timer will reset,
 *        however, the reaction time will NOT be displayed.
 *        
 *        A watchdog timer is used such that the board resets 
 *        if no user input is entered after 4 seconds. If the 
 *        system times out, then the decimal point will blink 
 *        for 5 seconds (0.5 sec on, 0.5 sec off) before reset.
 *        All user input entered during the blinking mode will
 *        be ignored.
 *        
 *        Note: If decimal point is lit on 7-Segment display, 
 *        then the number is hexadecimal A through F.
 *        
 * Arduino Version: 1.8.14
 *****************************************************************/

// Watchdog Timer Library
#include <avr/wdt.h>

// Name Pins
#define SegA 50
#define SegB 48
#define SegC 26
#define SegD 28
#define SegE 32
#define SegF 46
#define SegG 42
#define DP 22

// Global Variable for Watchdog Timer Setting
// False: Accepting input from serial monitor
// True: Watchdog timed out and blinking LED
volatile int resetFlag;

/********************************************************
 * Set Up
 *******************************************************/
void setup() {
  // Disable watchdog timer (low probability event)
  wdt_disable();

  // Initialize 7-Segment Display Pins
  pinMode(SegA, OUTPUT);
  pinMode(SegB, OUTPUT);
  pinMode(SegC, OUTPUT);
  pinMode(SegD, OUTPUT);
  pinMode(SegE, OUTPUT);
  pinMode(SegF, OUTPUT);
  pinMode(SegG, OUTPUT);
  pinMode(DP, OUTPUT);

  // Setup Watchdog Timer
  watchdogSetup();

  // Set 7-Segment Display to "-"
  ssd(0,0,0,0,0,0,1,0);

  // Set Reset Flag to 0
  resetFlag = 0;

  // Initialize serial communication (9600 bits/sec)
  Serial.begin(9600);
  while (!Serial) {
    delay(50); // Wait for serial port to connect
  }

  // Send instructions to User
  initialPrompt();
  wdt_reset();
}

/********************************************************
 * Main Loop
 *******************************************************/
void loop() { 
  // Check if Time out occurred
  if (resetFlag) {
    // Arm the Watchdog Timer to reset after 4 seconds
    watchdogArm();
    
    // Blink Decimal Point LED 
    blinkLED();
  }

  // resetFlag hasn't been triggered yet
  else {
    // Initialize input variable
    static String inputString;
  
    // Initialize reaction time variables
    static float reactionTime;
    static float totalTime = millis();

    // Get input from Serial Monitor
    while (Serial.available() > 0) {    
      // readStringUntil is faster than readString
      inputString = Serial.readStringUntil('\n');
  
      // Reset Watchdog Timer & calculate reaction time
      wdt_reset();
      reactionTime = millis() - totalTime;
  
      // Display output on 7-Segment Display
      sevenSegDisplay(inputString);
  
      // Check if input is valid
      if (isValid(inputString)) {
        // Only print input & reactiontime if valid
        printInput(inputString);
        printTime(reactionTime);
      }
  
      // Invalid input Message
      else {
        Serial.println("ERROR: Invalid input. "
          "Expected values from 0 to 15.");
      }
  
      // Send instructions to User
      initialPrompt();
        
      // Reset Watchdog Timer & totalTime
      wdt_reset();
      totalTime = millis();  
    }
  }
}

/********************************************************
 * Watchdog Timer Set Up (ONLY INTERRRUPT ENABLED)
 *******************************************************/
void watchdogSetup() {
  // Watchdog Configuration Mode
  WDTCSR |= 0b00011000;
  
  // Set Watchdog Settings for interrupts and no reset
  // WDP3 = 1 --> 4 second interrupts
  WDTCSR = (1<<WDIE) | (0<<WDE) | (1<<WDP3);
}

/********************************************************
 * Watchdog Timer Arming Setup (RESET ENABLED)
 *******************************************************/
void watchdogArm() {
  // Watchdog Configuration Mode
  WDTCSR |= 0b00011000;
  
  // Set Watchdog Settings for interrupts and no reset
  WDTCSR = (1<<WDIE) | (1<<WDE);

  // Set Timeout Period to 4 seconds
  wdt_enable(WDTO_4S);
}

/********************************************************
 * Watchdog Timer Interrupt Service Routine
 *******************************************************/
ISR(WDT_vect) {
  // If reset flag hasn't been triggered yet...
  if (resetFlag == 0) {
    // Set Flag to true
    resetFlag = 1;
    
    // Reset the interrupt
    WDTCSR |= (1<<WDIE);
  } 
}

/********************************************************
 * Function Name:     blinkLED
 * Input Parameters:  void
 * Return Value:      void
 * Purpose:           Blinks the decimal point LED for 
 *                    5 seconds (0.5 sec ON, 0.5 sec OFF)
 *                    and resets the board when done
 *******************************************************/
void blinkLED() {
  // Notify User that Interrupt occurred
  Serial.println("-- TIME OUT OCCURRED --");
  Serial.println("More than 4 seconds passed without "
    "user input.");
  Serial.println("-- RESETTING BOARD --");
  
  // Clear 7-Segment Display
  ssd(0,0,0,0,0,0,0,0);
  
  // Blink Decimal Point on/off for 5 seconds
  for(int i=0; i<5; i++){
    // ON for 0.5 sec
    digitalWrite(DP,1);
    delay(500);

    // OFF for 0.5 sec
    digitalWrite(DP,0);
    delay(500);
  }

  // Make sure watchdog timer reset is triggered 
  delay(2000);
}

/********************************************************
 * Function Name:     isValid
 * Input Parameters:  String input - Represents the input
 *                    to test whether valid or not
 * Return Value:      bool - true means the input is a
 *                    valid number between 0 and 15 
 * Purpose:           Returns a bool letting user know
 *                    if the input String is valid input
 *                    or not.
 *******************************************************/
bool isValid(String input) {
  // Check if length is longer than 2
  if (input.length() > 2 || input.length() == 0) {
    return 0;
  }

  // Check 0-9
  if (input.length() == 1 && isDigit(input[0])) {
    return 1;
  }

  // Check if 10-15
  if (input[0] == '1' && isDigit(input[1])) {
    if (input[1] == '0' || input[1] == '1' || input[1] == '2') { 
      return 1;
    }
    if (input[1] == '3' || input[1] == '4' || input[1] == '5') {
      return 1;
    }
  }

  // Catch all other invalid inputs
  return 0;
}

/********************************************************
 * Function Name:     initialPrompt
 * Input Parameters:  void
 * Return Value:      void
 * Purpose:           Prints instructions to Serial 
 *                    Monitor and prompts user to toggle
 *                    switch to blink LEDs
 *******************************************************/
void initialPrompt() {
  Serial.println("\nPlease enter a number between 0 and 15:");
}

/********************************************************
 * Function Name:     printTime
 * Input Parameters:  float reactionTime - represents the 
 *                    reaction time in ms
 * Return Value:      void
 * Purpose:           Prints the reaction time to the 
 *                    Serial Monitor.
 *******************************************************/
void printTime(float reactionTime) {
  Serial.print("reaction time = ");
  Serial.print((reactionTime/1000), 3);
  Serial.print("\n");
}

/********************************************************
 * Function Name:     printInput
 * Input Parameters:  String num - represents number to be 
 *                    displayed on Serial Monitor
 * Return Value:      void
 * Purpose:           Prints the input num to the Serial 
 *                    Monitor in hexadecimal
 *******************************************************/
void printInput(String num) {
  Serial.print("Your input = (hex) ");
  Serial.print(hex(num));
  Serial.print("\n");
}

/********************************************************
 * Function Name:     hex
 * Input Parameters:  String input - represents number to 
 *                    be displayed on Serial Monitor
 * Return Value:      String
 * Purpose:           Turns the decimal value of String
 *                    into hexadecimal string equivalent
 *                    ready for printing to Serial Monitor
 *******************************************************/
String hex(String input) {
  // Return self if 0-9
  if (input.length() == 1) {
    return input;
  }

  // Converts decimal Strings 10-15 to Hex values
  else if (input == "10") {
    return "A";
  }
  else if (input == "11") {
    return "B";
  }
  else if (input == "12") {
    return "C";
  }
  else if (input == "13") {
    return "D";
  }
  else if (input == "14") {
    return "E";
  }
  else {
    return "F";
  }
}

/********************************************************
 * Function Name:     sevenSegDisplay
 * Input Parameters:  String num - represents number to be 
 *                    displayed on 7-segment display
 * Return Value:      void
 * Purpose:           Decodes the String and calls ssd 
 *                    function to light up appropriate
 *                    segments
 *******************************************************/
void sevenSegDisplay(String num) {  
  // Print 0 on the 7-Segment display
  if (num == "0") {
    ssd(1,1,1,1,1,1,0,0);
  }

  // Print 1 on the 7-Segment display
  else if (num == "1") {
    ssd(0,1,1,0,0,0,0,0);
  }

  // Print 2 on the 7-Segment display
  else if (num == "2") {
    ssd(1,1,0,1,1,0,1,0);
  }

  // Print 3 on the 7-Segment display
  else if (num == "3") {
    ssd(1,1,1,1,0,0,1,0);
  }

  // Print 4 on the 7-Segment display
  else if (num == "4") {
    ssd(0,1,1,0,0,1,1,0);
  }

  // Print 5 on the 7-Segment display
  else if (num == "5") {
    ssd(1,0,1,1,0,1,1,0);
  }
  
  // Print 6 on the 7-Segment display
  else if (num == "6") {
    ssd(1,0,1,1,1,1,1,0);
  }

  // Print 7 on the 7-Segment display
  else if (num == "7") {
    ssd(1,1,1,0,0,0,0,0);
  }

  // Print 8 on the 7-Segment display
  else if (num == "8") {
    ssd(1,1,1,1,1,1,1,0);
  }

  // Print 9 on the 7-Segment display
  else if (num == "9") {
    ssd(1,1,1,1,0,1,1,0);
  }

  // Print A on the 7-Segment display
  else if (num == "10") {
    ssd(1,1,1,0,1,1,1,1);
  }

  // Print B on the 7-Segment display
  else if (num == "11") {
    ssd(0,0,1,1,1,1,1,1);
  }

  // Print C on the 7-Segment display
  else if (num == "12") {
    ssd(1,0,0,1,1,1,0,1);
  }

  // Print D on the 7-Segment display
  else if (num == "13") {
    ssd(0,1,1,1,1,0,1,1);
  }

  // Print E on the 7-Segment display
  else if (num == "14") {
    ssd(1,0,0,1,1,1,1,1);
  }

  // Print F on the 7-Segment display
  else if (num == "15") {
    ssd(1,0,0,0,1,1,1,1);
  }

  // Invalid Input
  // Print 3 horizontal bars to the 7-segment display
  else {
    ssd(1,0,0,1,0,0,1,0);
  }
}

/****************************************************************
 * Function Name:     ssd
 * Input Parameters:  bool a - represents segment a of display
 *                    bool b - segment b
 *                    bool c - segment c
 *                    bool d - segment d
 *                    bool e - segment e
 *                    bool f - segment f
 *                    bool g - segment g
 *                    bool dp - decimal point
 * Return Value:      void
 * Purpose:           Lights up 7-Segment display from 
 *                    0 to F according to input
 ****************************************************************/
void ssd(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool dp) {
  digitalWrite(SegA, a);
  digitalWrite(SegB, b);
  digitalWrite(SegC, c);
  digitalWrite(SegD, d);      
  digitalWrite(SegE, e);
  digitalWrite(SegF, f);
  digitalWrite(SegG, g);
  digitalWrite(DP, dp);
}
