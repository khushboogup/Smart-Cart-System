/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define RST_PIN 9          // Configurable, see typical pin layout above
#define SS_PIN  10  
 //return      // Configurable, see typical pin layout above
#define buzzer 7
#define red 5
#define green 6
#define button  3
#define final_button 4 //final
#define calc_button 2 //limit
//const int buttonPin = 2; // Define the pin for the pushbutton
int buttonState = 0;     // Current state of the button
int lastButtonState = 0; // Previous state of the button
unsigned long lastDebounceTime = 0; // Last time the button was pressed (for debouncing)
unsigned long debounceDelay = 50;   // Debounce time; increase if the output flickers

// Variables to track button press duration
unsigned long buttonPressStartTime = 0;
unsigned long buttonPressDuration = 0;
int buttonPressCount=0;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
struct Product{
  char name[20];
  double price;
  byte accessID[4];
  char productID[20];
  int count_prod;
};
Product products[] = {
  {"Biscuit",30.00,{0x19,0x3A,0x75,0xE4},"236547",0},
  {"Pepsi",50.00,{0x06,0x55,0xCF,0x1B},"785612",0},
  {"Water",20.00,{0x66,0xE8,0x68,0x1A},"524696",0},
};
//byte accessUID[4] = {0x19, 0x3A, 0x75, 0xE4};
//byte accessUID1[4] = {0xB0, 0xB5, 0x0D, 0x1B};
//int P1=0,P2=0;
double total =0;
//int count_prod = 0;
int limit;
int flag = 0;
int limit_val;
String str;
  
void setup() 
{
   Serial.begin(9600);
   lcd.init();
   lcd.backlight(); // turns on the backlight
   lcd.setCursor(2,0);
   pinMode(buzzer,OUTPUT);  
   pinMode(red,OUTPUT);  
   pinMode(green,OUTPUT);  
   pinMode(button,INPUT_PULLUP);
   pinMode(final_button,INPUT_PULLUP);
   pinMode(calc_button,INPUT_PULLUP);
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
   lcd.setCursor(0,0);
   lcd.print("Smart Trolley");
   lcd.setCursor(0,1);
   lcd.print("    System ");
   delay(3000);
   lcd.clear();
   lcd.setCursor(0,0);
   //lcd.print("Enter Your limit");
   lcd.print("  Scan to Add ");
   lcd.setCursor(0,1);
   lcd.print("     item");
   delay(2000);
   lcd.clear();
      
}

int countButtonPress() {
  // Read the state of the button
  int reading = digitalRead(calc_button);

  // Check if the button state has changed
  if (reading != lastButtonState) {
    // Reset the debounce timer
    lastDebounceTime = millis();
  }

  // Check if enough time has passed since the last debounce
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has changed, update the state
    if (reading != buttonState) {
      buttonState = reading;

      // If the button is pressed
      if (buttonState == LOW)
       {
        buttonPressStartTime = millis(); // Record the time when the button is pressed
      } 
      else 
      { // If the button is released
        buttonPressDuration = millis() - buttonPressStartTime; // Calculate the duration the button was pressed
        buttonPressCount++; // Increment the button press count
        // Serial.print("Button pressed for ");
        // Serial.print(buttonPressDuration);
        // Serial.println(" milliseconds");
        // Serial.print("Button press count: ");
        // Serial.println(buttonPressCount);
        lcd.setCursor(0,0);
        lcd.print("Enter Your limit");
        lcd.setCursor(0, 1);
        lcd.print("Push Button= ");
        lcd.print(buttonPressCount*100);
        
        
      }
    }
  }

  // Update lastButtonState
  lastButtonState = reading;

  // Return the button press count
  return buttonPressCount;
}
void loop()
{
 limit = countButtonPress();
 int limit_val = limit*100; 
 
 int a = digitalRead(3);
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
  { 
    Serial.println("button val = " );
    Serial.println(a);
    for(int i=0; i<sizeof(products)/sizeof(products[0]);i++){
      if(memcmp(mfrc522.uid.uidByte,products[i].accessID,sizeof(products[i].accessID)) == 0 && a==1){
        digitalWrite(buzzer,HIGH);
        digitalWrite(green,HIGH);
        delay(1000);
        digitalWrite(buzzer,LOW);
        digitalWrite(green,LOW);
        lcd.clear();
        lcd.print("P_ID = ");
        lcd.print(products[i].productID);
        lcd.setCursor(0, 1);
        lcd.print(products[i].name);
        lcd.print(" = ");
        lcd.print(products[i].price);
        total += products[i].price ;
        products[i].count_prod++;
        if(total>limit_val)
        {
          lcd.clear();
          lcd.print("Limit Reached");
          delay(50);
          digitalWrite(buzzer,1);delay(500);digitalWrite(buzzer,0);
        }
      // Serial.println("total qty");
      // Serial.println(count_prod);      
        delay(3000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Added in Cart");
        lcd.setCursor(0,1);
        lcd.print("Qty=");
        lcd.print(products[i].count_prod);
        lcd.setCursor(7,1);
        lcd.print("Price=");
        lcd.print(total);
        delay(2500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("  Scan to Add");
        lcd.setCursor(0, 1);
        lcd.print("  More Items");

      }
    }

    for(int i=0; i<sizeof(products)/sizeof(products[0]);i++){
      if(memcmp(mfrc522.uid.uidByte,products[i].accessID,sizeof(products[i].accessID)) == 0 && a==0){
        digitalWrite(buzzer,HIGH);
        digitalWrite(red,HIGH);
        delay(300);
        digitalWrite(buzzer,LOW);
        digitalWrite(red,LOW);
        delay(300);
        lcd.clear();
        lcd.print("P_ID = ");
        lcd.print(products[i].productID);
        lcd.setCursor(0, 1);
        lcd.print(products[i].name);
        lcd.print(" = ");
        lcd.print(products[i].price);
        total -= products[i].price ;
        products[i].count_prod--;
        
      // Serial.println("total qty");
      // Serial.println(count_prod);      
        delay(3000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Removed an Item");
        lcd.setCursor(0,1);
        lcd.print("Qty=");
        lcd.print(products[i].count_prod);
        lcd.setCursor(7,1);
        lcd.print("Price=");
        lcd.print(total);
        delay(2500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("  Scan to Add");
        lcd.setCursor(0, 1);
        lcd.print("  More Items");

      }
    }
  }
  if(digitalRead(final_button)==0){
    lcd.clear();
    lcd.print("  Thankyou for");
    lcd.setCursor(0,1);
    lcd.print("   Shopping");
    delay(2000);
    lcd.clear();
    lcd.print("Your Final Bill");
    lcd.setCursor(0,1);
    lcd.print("Pay = ");
    lcd.setCursor(7,1);
    lcd.print(total);
    String bill = String(total);
    Serial.println(bill);
}
  //  if(total>limit_val)
  //     {
  //       Serial.println(limit);
  //       Serial.println("Limit reached");
  //       lcd.clear();
  //       lcd.print("Limit Reached");
  //       delay(50);
  //       //digitalWrite(buzzer,1);delay(500);digitalWrite(buzzer,0);
  //     }
mfrc522.PICC_HaltA();

}