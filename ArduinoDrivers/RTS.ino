 /* Basic RTS Example

   You must select Real-Time Systems from the "Tools > USB Type" menu

   Optional: LEDs, button and potentiometer should be connected to pins mentioned below.

*/

/*****************************************************************************************
					TODO 5 Begin
*****************************************************************************************/

const int boardLed = 13; // LED on Teensy board
const int redPin = 23; // Red LED
const int greenPin = 21; // Green LED
const int yellowPin = 4; // Yellow LED
const int buttonPin = 2; // Push Button//USED SWITCH INSTEAD OF BUTTON
const int potentioPin = A8;                             // Pin for Potentiometer

/*****************************************************************************************
					TODO 5 End
*****************************************************************************************/
int val = 0;

void setup() {
/*****************************************************************************************
					TODO 6 Begin
*****************************************************************************************/
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(boardLed, OUTPUT);
  pinMode(potentioPin, INPUT);
/*****************************************************************************************
					TODO 6 End
*****************************************************************************************/
}

void loop() {
  digitalWrite(boardLed, HIGH);
  delay(100);
  digitalWrite(boardLed, LOW);
  delay(100);
}

/*****************************************************************************************
					TODO 7 Begin
*****************************************************************************************/

void read_cmd(int wValue, int wIndex){
  if(wIndex == 0){
   wValue= redPin;
    delay(20);
  }
  if(wIndex == 1){
    wValue=yellowPin;
    delay(20);
  }
    if(wIndex == 2){
    wValue=greenPin;
    delay(20);
  }
}

void write_cmd(int* reply_value, int wIndex){
  if(wIndex == 0){
   *reply_value =buttonPin;
    delay(20);
  }
  if(wIndex == 1){
    *reply_value=potentioPin;
    delay(20);
  }
}





/*****************************************************************************************
					TODO 7 End
*****************************************************************************************/

