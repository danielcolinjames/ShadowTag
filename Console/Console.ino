#include <XBee.h>
#include <SoftwareSerial.h>


// ---------------------------------------------------------//
// ------------  Game state and logic variables ------------//
// ---------------------------------------------------------//

// 90 for blue, 91 for red, 92 for purple/white

int stateArray[] = { 90, 90, 90, 90, 90, 90, 90, 90, 90, 90 };

uint8_t suitID;
uint8_t taggerID;

// admin messages are sent at the start of a new game
// and they tell each suit which colour it starts as

uint8_t suitAdminID;
uint8_t taggerAdminID;

int gameMode = 0;


// ---------------------------------------------------------//
// ---------------------  XBee variables  ------------------//
// ---------------------------------------------------------//

SoftwareSerial xbeeSerial (2, 3);

SoftwareSerial debugSerial (9, 8); //rx, tx

XBee xbee = XBee();

uint8_t payload[] =  {0, 0, 0};

Tx16Request tx = Tx16Request(0x2, payload, sizeof(payload));
TxStatusResponse txStatus = TxStatusResponse();

Rx16Response rx16 = Rx16Response();

uint8_t startBit = 99;

unsigned char colourChangeInstruction = 0;

int tempSuitState = 0;

boolean confirmation = false;

// ---------------------------------------------------------//
// ------------------------  Setup  ------------------------//
// ---------------------------------------------------------//
void setup() {
  
  Serial.begin(9600);
  //xbeeSerial.begin(9600);
  
  xbee.setSerial(Serial);
  
  debugSerial.begin(9600);
  debugSerial.println("Starting...");
  
  delay(10);
  
  gameMode = 1;
  
  startGame();
}


// ---------------------------------------------------------//
// -------------------------  Loop  ------------------------//
// ---------------------------------------------------------//
void loop() {
  lookForMessages();
}






