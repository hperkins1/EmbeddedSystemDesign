/**************************************************************
 *  "Introduction to Analog-to-Digital Converters"
 * ECE 487 Lab 4
 * 
 * Author:  Hannah Perkins
 * CWID:    11472559
 * Date:    25 February 2021
 * Course:  ECE 487
 * 
 * Function:      The program converts an analog signal from 0 to
 *        +5 V to a digital value using the built in ADC. The user
 *        can adjust the signal value by using the potentiometer.
 *  
 * Description:   The application will perform 30 ADC conversions 
 *        using the analogRead function and calculate the time it  
 *        took (in microseconds) for the conversion to take place. 
 *        The program will also calculate the average conversion
 *        time and print it to the Serial Monitor at the end of all
 *        the conversions. 
 *        
 *                Additionally, the program converts and prints the 
 *        hexadecimal digital value for each ADC Conversion.
 *        
 *                The only valid input it the lower case letter 'c'
 *        All other input will result in an error message.
 *        
 *                The watchdog timer hardware is set up such that 
 *        after 4 seconds of no user input (valid or invalid) the
 *        board will reset. A  board reset message informs the 
 *        user whenever a reset occurred and/or the program is run
 *        for the first time. 
 *        
 * Arduino Version: 1.8.14
 *****************************************************************/

// Watchdog Timer Library
#include <avr/wdt.h>

// Name Pins
#define analogIN A0

/********************************************************
 * Set Up
 *******************************************************/
void setup() {
  // Disable watchdog timer (low probability event)
  wdt_disable();

  // Initialize 7-Segment Display Pins
  pinMode(analogIN, INPUT);

  // Setup Watchdog Timer
  watchdogSetup();

  // Initialize serial communication (9600 bits/sec)
  Serial.begin(9600);
  while (!Serial) {
    delay(50); // Wait for serial port to connect
  }

  // Board Reset Message
  boardResetMessage();
  
  // Send instructions to User
  initialPrompt();

  // Reset Watchdog Timer
  wdt_reset();
}

/********************************************************
 * Main Loop
 *******************************************************/
void loop() { 
  // Initialize input variable
  static String inputString;

  // Get input from Serial Monitor
  while (Serial.available() > 0) {    
    // readStringUntil is faster than readString
    inputString = Serial.readStringUntil('\n');
  
    // Reset Watchdog Timer 
    wdt_reset();

    // Check User Input
    if (inputString == "c") {
      // Begin analogRead ADC Conversions
      ADCConversions();
    }

    // Send Error Message for invalid user input
    else {
      errorMessage();
    }
    
    // Send instructions to User
    initialPrompt();
        
    // Reset Watchdog Timer & totalTime
    wdt_reset();
  }
}

/********************************************************
 * Watchdog Timer Set Up (RESET & INTERRRUPT ENABLED)
 *******************************************************/
void watchdogSetup() {
  // Watchdog Configuration Mode
  WDTCSR |= 0b00011000;
  
  // Set Watchdog Settings for interrupts and no reset
  // WDP3 = 1 --> 4 second interrupts
  WDTCSR = (1<<WDIE) | (1<<WDE);

  // Set Timeout Period to 4 seconds
  wdt_enable(WDTO_4S);
}

/********************************************************
 * Function Name:     errorMessage
 * Input Parameters:  void
 * Return Value:      void
 * Purpose:           Informs user of invalid input.
 *******************************************************/
void errorMessage() {
  Serial.println("ERROR: Invalid user input - the only "
    "valid user input is 'c'");
}

/********************************************************
 * Function Name:     boardResetMessage
 * Input Parameters:  void
 * Return Value:      void
 * Purpose:           Informs user that board was reset.
 *******************************************************/
void boardResetMessage() {
  Serial.println("Board was reset.");
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
  Serial.println("Enter 'c' to start a set of conversions:");
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
  Serial.print("Time = ");
  Serial.print(reactionTime);
  Serial.print(" usecs\n");
}

/********************************************************
 * Function Name:     ADCConversions
 * Input Parameters:  void
 * Return Value:      void
 * Purpose:           Performs 30 ADC Conversions and 
 *                    prints hex digital value and time 
 *                    for each conversion to serial 
 *                    monitor. Also prints average time
 *                    it took for all conversions.
 *******************************************************/
void ADCConversions() {
  // Initialize Analog value variable
  int analogValue;

  // Initialize Time Variables
  float reactionTime;
  float startTime;
  float totalReactionTime = 0;
  
  // Inform user that conversions are starting
  Serial.println("Starting a set of conversions:");

  // Perform 30 conversions
  for(int i=0; i<30; i++) {   
    // Set Start Time
    startTime = micros();
    
    // Read analog value
    analogValue = analogRead(analogIN);
    
    // Calculate Reaction Time
    reactionTime = micros() - startTime;

    // Add reaction time to sum
    totalReactionTime += reactionTime;
    
    // Print Conversion Number
    Serial.print("#");
    Serial.print(i+1);
    Serial.print(":\t");

    // Print Digital Value
    printDigitalValue(analogValue);

    // Print Reaction Time
    printTime(reactionTime);

    // Small Delay between Readings
    delay(100);
  }

  // Calculate & Print Average Reaction Time
  Serial.print("\navg conversion time = ");
  Serial.print((totalReactionTime/30), 2);
  Serial.print(" usecs\n\n");

  // Flush Serial Buffer
  while(Serial.available()) {
    Serial.read();
  }
}

/********************************************************
 * Function Name:     printDigitalValue
 * Input Parameters:  int value - represents number to be 
 *                    converted & displayed on Serial 
 *                    Monitor
 * Return Value:      void
 * Purpose:           Turns the decimal value of integer
 *                    into hexadecimal string equivalent
 *                    & prints it to Serial Monitor
 *******************************************************/
void printDigitalValue(int value) {
  // Initialize digital value to null string
  String digitalValue = "";

  // Convert base 10 to hexadecimal values
  for(int i=0; i<3; i++) {
    // Add Least significant digit to digitalValue
    int temp = value%16;
    digitalValue = hex(temp) + digitalValue;

    // Shift right
    value = value/16;
  }

  // Print digital Value to Serial Monitor
  printInput(digitalValue);
}

/********************************************************
 * Function Name:     hex
 * Input Parameters:  int count - represents number to be 
 *                    displayed on Serial Monitor
 * Return Value:      String
 * Purpose:           Turns the decimal value of integer
 *                    into hexadecimal string equivalent
 *                    ready for printing to Serial Monitor
 *******************************************************/
String hex(int count) {
  if (count < 10) {
    return String(count);
  }
  else if (count == 10) {
    return "A";
  }
  else if (count == 11) {
    return "B";
  }
  else if (count == 12) {
    return "C";
  }
  else if (count == 13) {
    return "D";
  }
  else if (count == 14) {
    return "E";
  }
  else {
    return "F";
  }
}

/********************************************************
 * Function Name:     printInput
 * Input Parameters:  String num - represents number to be 
 *                    displayed on Serial Monitor
 * Return Value:      void
 * Purpose:           Prints the input num to the Serial 
 *                    Monitor
 *******************************************************/
void printInput(String num) {
  Serial.print("digital value = ");
  Serial.print(num);
  Serial.print("\t");
}
