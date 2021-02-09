/**************************************************************
 *  "Introduction to Digital Input/Output"
 * ECE 487 Lab 2 
 * 
 * Author:  Hannah Perkins
 * CWID:    11472559
 * Date:    8 February 2021
 * Course:  ECE 487
 * 
 * Function:      Blink onboard and offboard LEDs out of phase
 *        based on Toggle Switch input. Keep track of decimal
 *        and hexadecimal count of switch changes on Serial
 *        Monitor and 7-Segment Display.
 *  
 * Description:   Application will blink onboard and offboard 
 *        LEDs out of phase while Toggle Switch is in "on" 
 *        position. During Blinking operation, onboard LED will 
 *        be ON for 1 second and OFF for 1 second. Offboard LED 
 *        will be exactly out-of-phase: OFF for 1 second and ON 
 *        for 1 second. LEDs will continuously blink in this mode 
 *        until Toggle Switch is moved to the "off" position. 
 *        While Toggle Switch is "off" the LED's will both be off.
 *        
 *        The application will keep a count of the total number
 *        of user inputs (switch changes) that have occurred. 
 *        This output will be displayed on the Serial Monitor
 *        in both decimal and hexadecimal. The count range is 
 *        0 to 15 and will rollover after 15. 
 *        
 *        The 7-Segment display will display the hexadecimal 
 *        value of the count. This will match the count 
 *        maintained on the Serial Monitor. Note: If decimal 
 *        point is lit on 7-Segment display, then the number is 
 *        hexadecimal A through F.
 *        
 * Arduino Version: 1.8.14
 *****************************************************************/

// Name Pins
#define OnboardLED 13
#define OffboardLED 9
#define ToggleSwitch 3
#define SegA 50
#define SegB 48
#define SegC 26
#define SegD 28
#define SegE 32
#define SegF 46
#define SegG 42
#define DP 22

void setup() {
  // Initialize onboard and offboard LED Pins
  pinMode(OnboardLED, OUTPUT);
  pinMode(OffboardLED, OUTPUT);

  // Initialize 7-Segment Display Pins
  pinMode(SegA, OUTPUT);
  pinMode(SegB, OUTPUT);
  pinMode(SegC, OUTPUT);
  pinMode(SegD, OUTPUT);
  pinMode(SegE, OUTPUT);
  pinMode(SegF, OUTPUT);
  pinMode(SegG, OUTPUT);
  pinMode(DP, OUTPUT);

  // Initialize Toggle Switch input
  pinMode(ToggleSwitch, INPUT);

  // Set LEDs to OFF
  digitalWrite(OnboardLED, LOW);
  digitalWrite(OffboardLED, LOW);

  // Set 7-Segment Display to 0
  sevenSegDisplay(0);

  // Initialize serial communication (9600 bits/sec)
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }

  // Send instructions to User
  initialPrompt();
}

void loop() {
  // Test all of the 7 seg displays
  for(int i=0; i<16; i++) {
    sevenSegDisplay(i);
    delay(1000);
  }
}

// Prompt User for input
void initialPrompt() {
  Serial.println("\nPlease enter move Toggle Switch to 'on' "
    "position to begin blinking LEDs. To turn off LED's, move "
    "the Toggle Switch to 'off' position.");
  Serial.println("\nThe 7-Segment Display and Serial Monitor "
    "will keep track of the total number of user inputs "
    "that have occurred. (Toggle switch inputs ONLY.)");
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
  digitalWrite(OnboardLED, LOW);
  digitalWrite(OffboardLED, LOW);
  delay(1000);
}

// Blink 2 LEDS out-of-phase of eachother
void blinkLeds() {
  // Onboard LED ON & Offboard LED OFF for 1 second
  digitalWrite(OnboardLED, HIGH);
  digitalWrite(OffboardLED, LOW);
  delay(1000);
  
  // Onboard LED OFF & Offboard LED ON for 1 second
  digitalWrite(OnboardLED, LOW);
  digitalWrite(OffboardLED, HIGH);
  delay(1000);
}

// 7-Segment Display Decoder

/********************************************************
 * Function Name:     sevenSegDisplay
 * Input Parameters:  int num - represents number to be 
 *                    displayed on 7-segment display
 * Return Value:      void
 * Purpose:           Decodes the integer and calls ssd 
 *                    function to light up appropriate
 *                    segments
 *******************************************************/
void sevenSegDisplay(int num) {
  // Range of num should be from 0 to 15
  switch (num) {
    case 0:
      ssd(1,1,1,1,1,1,0,0);
      break;
    case 1:
      ssd(0,1,1,0,0,0,0,0);
      break;
    case 2:
      ssd(1,1,0,1,1,0,1,0);
      break;
    case 3:
      ssd(1,1,1,1,0,0,1,0);
      break;
    case 4:
      ssd(0,1,1,0,0,1,1,0);
      break;
    case 5:
      ssd(1,0,1,1,0,1,1,0);
      break;
    case 6:
      ssd(1,0,1,1,1,1,1,0);
      break;
    case 7:
      ssd(1,1,1,0,0,0,0,0);
      break;
    case 8:
      ssd(1,1,1,1,1,1,1,0);
      break;
    case 9:
      ssd(1,1,1,1,0,1,1,0);
      break;
    case 10:
      ssd(1,1,1,0,1,1,1,1);
      break;
    case 11:
      ssd(0,0,1,1,1,1,1,1);
      break;
    case 12:
      ssd(1,0,0,1,1,1,0,1);
      break;
    case 13:
      ssd(0,1,1,1,1,0,1,1);
      break;
    case 14:
      ssd(1,0,0,1,1,1,1,1);
      break;
    case 15:
      ssd(1,0,0,0,1,1,1,1);
      break;
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
