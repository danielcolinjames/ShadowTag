// ---------------------------------------------------------//
// ------  Send suitID and taggerID to the console  ------//
// ---------------------------------------------------------//
void sendIWasTagged() {
  
  payload[0] = taggedByte;
  payload[1] = suitID;
  payload[2] = taggerID;
  
  xbee.send(tx);
  confirmDelivery(taggedByte, 1);
  
  if (messageReceived == false) {
    xbee.send(tx);
    confirmDelivery(taggedByte, 2);
  }
  
  if (messageReceived == false) {
    xbee.send(tx);
    confirmDelivery(taggedByte, 3);
  }

  if (messageReceived == true) {
    lookForInstruction();
  }
}

// ---------------------------------------------------------//
// ----  Look for a response to "I was tagged" message  ----//
// ---------------------------------------------------------//
void lookForInstruction() {
  // if this suit recently sent a message to the console,
  // look for a response with instructions
  if (xbee.readPacket(300)) {
    
    debugSerial.print("Instruction found.");
    
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
      xbee.getResponse().getRx16Response(rx16);
      
      uint8_t packetType = rx16.getData(0);
      
      debugSerial.print(" Packet type = ");
      debugSerial.print(packetType);
      debugSerial.println(".");
      
      if (packetType != 96) {
        
        // if it's a 97, change to the instruction's colour
        if (packetType == 97) {
          instruction = rx16.getData(1);
          
          setColour(instruction);
          changeColour(rVal, gVal, bVal);

          debugSerial.print("Setting colour to ");
          debugSerial.println(instruction);
          rfiduino.successSound();
          delay(1000);
        }
        
        // game over command
        else if (packetType == 95) {
          
          changeColour(rVal, gVal, bVal);
          rfiduino.successSound();
          delay(2000);
          
          changeColour(255, 255, 255);
          
          digitalWrite(rfiduino.led1, LOW); // red off
          digitalWrite(rfiduino.led2, LOW); // green off
          
          gameOver();
        }
        
        // game start command, next byte in payload is
        // going to be the starting colour
        else if (packetType == 98) {
          
          uint8_t colour = rx16.getData(1);
          debugSerial.print("Starting colour received: ");
          debugSerial.print(colour);
          debugSerial.println(".");
          
          setColour(colour);
          activateSuit(rVal, gVal, bVal);
        }
      }
      else {
        // if the message WAS 96
        changeColour(rVal, gVal, bVal);
        debugSerial.println("Keeping colour the same.");
        rfiduino.errorSound();
        delay(1000);
      }
    }
  }
}


// ---------------------------------------------------------//
// --------  Look for messages addressed to this suit ------//
// ---------------------------------------------------------//
void lookForMessages() {
  
  // look for 98 or 95 (game reset/start, or game over)
  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable()) {
    debugSerial.print("Packet found.");
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
      xbee.getResponse().getRx16Response(rx16);
      
      uint8_t packetType = rx16.getData(0);
      
      debugSerial.print(" Packet type = ");
      debugSerial.print(packetType);
      debugSerial.println(".");

      // game over
      if (packetType == 95) {

        changeColour(rVal, gVal, bVal);
        rfiduino.successSound();
        delay(2000);
        
        changeColour(255, 255, 255);
        
        digitalWrite(rfiduino.led1, LOW); // red off
        digitalWrite(rfiduino.led2, LOW); // green off

        gameOver();
      }

      // game start command, next byte in payload is
      // going to be the starting colour
      else if (packetType == 98) {
        uint8_t colour = rx16.getData(1);
        debugSerial.print("Starting colour received: ");
        debugSerial.print(colour);
        debugSerial.println(".");
        
        setColour(colour);
        activateSuit(rVal, gVal, bVal);      
      }
    }
  }
}


// ---------------------------------------------------------//
// -------  Confirms reception of transmitted packet  ------//
// ---------------------------------------------------------//
void confirmDelivery(uint8_t packetType, uint8_t attempt) {
  messageReceived = false;
  if (xbee.readPacket(300)) {
    
    if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
      TxStatusResponse txStatus = TxStatusResponse();
      xbee.getResponse().getTxStatusResponse(txStatus);
      
      if (txStatus.getStatus() == SUCCESS) {
        debugSerial.print(packetType);
        debugSerial.print(" sent successfully to console on attempt ");
        debugSerial.print(attempt);
        debugSerial.println(".");
        
        messageReceived = true;
      }
    } else {
        debugSerial.print(packetType);
        debugSerial.print(" sent unsuccessfully to console on attempt ");
        debugSerial.print(attempt);
        debugSerial.println(".");
    }
  } else if (xbee.getResponse().isError()) {
    debugSerial.println("Error reading packet: ");
    debugSerial.println(xbee.getResponse().getErrorCode());
  } else {
      debugSerial.print(packetType);
      debugSerial.print(" message to console timed out on attempt ");
      debugSerial.print(attempt);
      debugSerial.println(".");
  }
}

