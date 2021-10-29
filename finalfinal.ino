#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <String.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); //declaring the LCD screen

int NOTE_C1  =33; //exact frequencies to be played
int NOTE_D2  =73;
int NOTE_E3  =165;
int NOTE_F4  =349;
int NOTE_A5  =880;
int NOTE_B6 = 1976;
int NOTE_C7 = 2093;
int NOTE_C8 = 3186;
int NOTE_D8 = 3699;

int notes; // variable for identifying the triplet based on distance
long duration=0; // variable for the duration of sound wave travel
long distance=0; // variable for the distance measurement

String note; // for display

#define echoPin 9 
#define trigPin 8
int Speaker =   4;
int PIN_read1 = 11;
int PIN_read2 = 12;
int PIN_read3 = 13;
int toggler = 2; // interrupt pin for toggling between the modes

//array for storing names and frequencies
String characters[9] = {"C1", "D2", "E3", "F4", "A5", "B6", "C7", "C8", "D8"}; 
int Notes[] = {NOTE_C1, NOTE_D2, NOTE_E3, NOTE_F4, NOTE_A5, NOTE_B6, NOTE_C7, NOTE_C8, NOTE_D8}; 

int t = 500; //delay variable for the program
bool val1;
bool val2;
bool val3;

int addr = 0; //for writing on EEPROM
int flag = 0; //flag variable for toggling between play and record modes

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(PIN_read1, INPUT); //button inputs
  pinMode(PIN_read2, INPUT); //button inputs
  pinMode(PIN_read3, INPUT); //button inputs
  pinMode(toggler, INPUT_PULLUP); //Interrupt input
  
  Serial.begin(9600); 
  attachInterrupt(digitalPinToInterrupt(toggler), recordplayer, FALLING); //attaching ISR
  lcd.begin(); // initializing the LCD screen
}

void recordplayer() //ISR
{
  flag = 1;
}

void player() //function to play the stored value from EEPROM and also reset it
{
  int address = 0;
  int value = 0;
  String noteval;
  while(EEPROM.read(address)!=255) //end of data condition
  {
    value=EEPROM.read(address); //gets the array index from EEPROM to play the note 
    lcd.clear();
    lcd.setCursor(7, 0);
    noteval=String(characters[value]);
    lcd.print(noteval); //display the note being played
    tone(Speaker, Notes[value], t); //play the note
    delay(t);
    EEPROM.write(address, 255); //resetting the EEPROM data at that location
    delay(300);
    address++; 
  }
  flag = 0; //resetting the flag variable to go back to record mode
  lcd.setCursor(4, 0);
  lcd.print("Thank you");
  lcd.setCursor(7, 1);
  lcd.print("- Team TBD");
  delay(1000);
  
}

void loop() {
  if (flag==1) //polling to check play mode or record mode
  {
    player();
    addr=0; //resetting the storage address for EEPROM to begin from 0
  }
  else
  {
    //distance block for ultrasonic sensor
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.017; // Speed of sound wave divided by 2 (go and back)
    
    notes=(int)distance/10; // the value of distance cannot be more than 30, so identifying three triplets based on distance 
    notes = notes*3;
    if(notes > 6){
      notes = 0;
    } //to throw any errors or overflow

    //displaying the possible notes on LCD screen
    note=String(characters[notes]);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print(note);
    lcd.print(" ");
    note=String(characters[notes+1]);
    lcd.print(note);
    lcd.print(" ");
    note=String(characters[notes+2]);
    lcd.print(note);
    lcd.print(" ");
    
    // logical blocks to play the note and store the data into EEPROM. All operations are within if to avoid garbage notes and data
    val1 = digitalRead(PIN_read1);
      if(val1 == HIGH)
      {
        tone(Speaker, Notes[notes], t);
        delay(t);
        EEPROM.write(addr, notes);
        Serial.println(characters[notes]);
        addr++;
      }
    val2 = digitalRead(PIN_read2);
      if(val2 == HIGH)
      {
        tone(Speaker, Notes[notes+1], t);
        delay(t);
        EEPROM.write(addr, notes+1);
        Serial.println(characters[notes+1]);
        addr++;
      }
    val3 = digitalRead(PIN_read3);
      if(val3 == HIGH)
      {
        tone(Speaker, Notes[notes+2], t);
        delay(t);
        EEPROM.write(addr, notes+2);
        Serial.println(characters[notes+2]);
        addr++;
      }   
}
}
