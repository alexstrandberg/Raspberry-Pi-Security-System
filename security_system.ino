// RFID Reader code taken from: http://playground.arduino.cc/Learning/PRFID


#include <Servo.h>
#include <SoftwareSerial.h>

const int switchPin = 5;     //     // Reed switch to digital pin 5
const int servoPin = 6;
const int rfidEnablePin = 7;
int val = 0;
char code[10];
int bytesread = 0;

String inputString = "";
boolean stringComplete = false;

boolean systemArmed = false;

unsigned long incidentTime = 0;
unsigned long waitForAuthentication = 15000; // 15 seconds are given to disable the system

unsigned long blinkInterval = 200;
unsigned long blinkMillis = 0;
int ledState = LOW;
boolean shouldDeactivate = false;
boolean incidentInProgress = false;
boolean showedFailedMessage = false;

Servo myServo;
SoftwareSerial RFID = SoftwareSerial(8, 2);

const int redLED = 9;
const int greenLED = 10;
const int blueLED = 11;

const int buttonPin = 12;
boolean waitingToChangeStatus = false;

void setup() {
  pinMode(switchPin, INPUT);        // switchPin is an input
  pinMode(13, OUTPUT);              // Board LED is output
  pinMode(rfidEnablePin, OUTPUT);
  digitalWrite(switchPin, HIGH);    // Activate internal pullup resistor
  digitalWrite(rfidEnablePin, HIGH);
  
  RFID.begin(2400);
  
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  digitalWrite(redLED, LOW);
  digitalWrite(blueLED, LOW);
  
  pinMode(buttonPin, INPUT_PULLUP);
  
  Serial.begin(9600);
  Serial.println("ARDUINO READY");
}

void loop() {
  if (incidentInProgress) {
    if (millis() - blinkMillis > blinkInterval) { // Blinks LED in background
        blinkMillis = millis();
        if (ledState == LOW) ledState = HIGH;
        else ledState = LOW;
        digitalWrite(13, ledState);
    }
    if (millis() - incidentTime > waitForAuthentication) {
      if (showedFailedMessage==false) {
        Serial.println("FAILED TO AUTHENTICATE");
        showedFailedMessage = true;
        digitalWrite(rfidEnablePin, HIGH);
      }
      if (digitalRead(switchPin)==LOW) {
        Serial.println("DOOR CLOSED");
        incidentInProgress = false;
        showedFailedMessage = false;
        digitalWrite(13, LOW);
      }
    } else if (shouldDeactivate) {
      incidentInProgress = false;
      systemArmed = false; // Since the system was deactivated, don't look for door openings until reactivated later
      shouldDeactivate = false;
      Serial.println("DEACTIVATED");
      digitalWrite(13, LOW);
      digitalWrite(rfidEnablePin, HIGH); // Deactivate RFID Reader
    } else if ((val = RFID.read()) == 10) {
      // check for header
      bytesread = 0;
      while(bytesread<10) {
        val = RFID.read();
        if ((val == 10) || (val == 13)) {
          // if header or stop bytes before the 10 digit reading
          break;
        }
        code[bytesread] = val;
        bytesread++;
      }
      
      if (bytesread == 10 && systemArmed == true) {
        // if 10 digit read is complete
        Serial.print("CODE: ");
        Serial.println(code);
      }
      bytesread = 0;
      delay(1000);
      RFID.flush();
    }
  } else if (digitalRead(switchPin)==HIGH && systemArmed==true) {
    Serial.println("DOOR OPENED");
    incidentTime = millis();
    blinkMillis = millis();
    incidentInProgress = true;
    digitalWrite(rfidEnablePin, LOW); // Enable RFID Reader
  } else  if (digitalRead(buttonPin)==LOW && !waitingToChangeStatus) {
    waitingToChangeStatus = true;
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(blueLED, HIGH);
    delay(200);
    digitalWrite(blueLED, LOW);
    delay(200);
    digitalWrite(blueLED, HIGH);
    delay(200);
    digitalWrite(blueLED, LOW);
    delay(200);
    digitalWrite(blueLED, HIGH);
    delay(200);
    digitalWrite(rfidEnablePin, LOW);
  } else if (waitingToChangeStatus && (val = RFID.read()) == 10) {
    // check for header
    bytesread = 0;
    while(bytesread<10) {
      val = RFID.read();
      if ((val == 10) || (val == 13)) {
        // if header or stop bytes before the 10 digit reading
        break;
      }
      code[bytesread] = val;
      bytesread++;
    } 
    if (bytesread == 10) {
      // if 10 digit read is complete
      Serial.print("CODE: ");
      Serial.println(code);
    }
    bytesread = 0;
    delay(1000);
    RFID.flush();
  }
  
  delay(200);
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    
    if (inputString=="PI READY") {
      digitalWrite(greenLED, HIGH);
      inputString = "";
    }
    
    else if (inputString=="SERVO LEFT") {
      myServo.attach(servoPin);
      myServo.write(60);
      delay(125);
      myServo.write(90);
      delay(125);
      inputString = "";
      myServo.detach();
    }
    
    else if (inputString=="SERVO RIGHT") {
      myServo.attach(servoPin);
      myServo.write(120);
      delay(125);
      myServo.write(90);
      delay(125);
      inputString = "";
      myServo.detach();
    }
    
    else if (inputString=="ARM SYSTEM") {
      systemArmed = true;
      waitingToChangeStatus = false;
      Serial.println("SYSTEM ARMED");
      digitalWrite(greenLED, LOW);
      digitalWrite(blueLED, LOW);
      digitalWrite(rfidEnablePin, HIGH);
      
      for (int x = 0; x < 40; x++) {
        digitalWrite(redLED, HIGH);
        delay(200);
        digitalWrite(redLED, LOW);
        delay(200);
      }
      digitalWrite(redLED, HIGH);
      
      inputString = "";
    }
    
    else if (inputString=="DEACTIVATE") {
      waitingToChangeStatus = false;
      shouldDeactivate = true;
      digitalWrite(redLED, LOW);
      digitalWrite(blueLED, LOW);
      digitalWrite(greenLED, HIGH);
      digitalWrite(rfidEnablePin, HIGH);
      Serial.println("SYSTEM DEACTIVATED");
      inputString = "";
    }
    
    else if (inputString.indexOf("WRONG CODE")>=0) {
      digitalWrite(blueLED, LOW);
      digitalWrite(redLED, HIGH);
      delay(200);
      digitalWrite(redLED, LOW);
      delay(200);
      digitalWrite(redLED, HIGH);
      delay(200);
      digitalWrite(redLED, LOW);
      delay(200);
      digitalWrite(redLED, HIGH);
      delay(200);
      digitalWrite(redLED, LOW);
      digitalWrite(blueLED, HIGH);
      inputString = "";
    }
  }
}
