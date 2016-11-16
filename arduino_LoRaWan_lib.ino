/*--------------------------------------------------------------------------

              j]_                   .___                                   
._________    ]0Mm                .=]MM]=                                  
M]MM]MM]M]1  jMM]P               d]-' NM]i                                 
-~-~   4MM1  d]M]1              d]'   jM]'                                 
       j]MT .]M]01       d],  .M]'    d]#                                  
       d]M1 jM4M]1  .,  d]MM  d]I    .]M'                                  
       ]0]  M/j]0(  d]L NM]f d]P     jM-                                   
       M]M .]I]0M  _]MMi -' .]M'                                           
       M]0 jM MM]  jM-M>   .]M/                                            
       ]0F MT ]M]  M>      d]M1        .,                                  
      j0MT.]' M]M j]1 .mm .]MM ._d]_,   J,                                 
      jM]1jM  ]01 =] .]M/ jM]Fd]M]MM]   .'                                 
      j]M1#T .M]1.]1 jM]' M]0M/^ "M]MT  j         .",    .__,  _,-_        
      jMM\]' J]01jM  M]M .]0]P    ]0]1  i         1 1   .'  j .'  "1       
      j]MJ]  jM]1]P .]M1 jMMP     MM]1  I        J  t   1   j J    '       
      =M]dT  jM]q0' dM]  M]MT     ]MM  j        j   j  j    J 1            
      ]M]M`  j]0j#  ]MF  ]M]'    .M]P  J       .'   j  J  .J  4_,          
      M]0M   =MM]1 .M]'  MM]     jM](  1       r    j  1  --,   "!         
      ]0MT   ]M]M  jM@   ]M]     M]P  j       J     j j     4     1        
      MM]'   M]0P  j]1  .M]M    j]M'  J      j'     ",?     j     1        
     _]M]    M]0`  jM1 .MNMM,  .]M'   1     .'       11     1    j'        
     jM]1   jM]@   j]L_]'?M]M__MP'    \     J        1G    J    .'         
     j]0(   jM]1   "M]P'  "N]M/-      "L__J L________'?L__- *__,'          
     "-'    "--                                                            
                                                                           
----------------------------------------------------------------------------

Copyright (c) <2014>, <Wi6labs>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the wi6labs nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL WI6LABS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Description: Arduino SPI interface implementation 
-----------------------------------------------------------------------------*/                                                             

#include <SPI.h>

//---------------------------------------------------
//
// Verbosity:
// By default LoRaWan library writes traces on the 
// Arduino Serial. If you don’t want traces or if you 
// need the Serial port for another purpose, you can 
// disable traces. To do it, write this line at the 
// beginning of your setup() function:
// LoRaWan_verbose = false;
// 
// Of course you can use Serial.print() and other 
// Serial function in your code to create your own 
// traces. There is no need to call Serial.begin() 
// because it is already done in LoRaWan_init()
//
//---------------------------------------------------
boolean LoRaWan_verbose = true;

byte LoRaWan_last_rx[64];
byte LoRaWan_last_rx_size = 0;

const byte SPIslaveSelectPin = 10;
const byte ResetPin          = 5; 
const byte WakeupPin         = 7;
//////////////////////////////////////////////////////
//
//  LOW LEVEL FUNCTIONS FOR SPI ACCESS 
//
//////////////////////////////////////////////////////

//---------------------------------------------------
//
// This function is used to send AT commands to the modem.
//
//---------------------------------------------------
void LoRaWan_send_cmd(String in) {

  int len = in.length();

 digitalWrite(SPIslaveSelectPin,LOW);

  delay(1);
  //  send data
  for (int i=0; i<len; i++) {
    SPI.transfer(in.charAt(i));
  delay(1);
  }
  
  // take the SS pin high to de-select the chip:
  digitalWrite(SPIslaveSelectPin,HIGH); 
 
  delay(100);

}

//---------------------------------------------------
//
// This function requests the modem a one byte status,
// sending it a command.
//
//---------------------------------------------------
byte LoRaWan_get_status_one_byte (byte cmd) {
  byte status;

  // Chip select
  digitalWrite(SPIslaveSelectPin,LOW);
  delay(1);
  SPI.transfer(cmd);
  delay(4); // Wait commend to be processed
  status = SPI.transfer(0x00); 
  delay(1);
   // take the SS pin high to de-select the chip:
  digitalWrite(SPIslaveSelectPin,HIGH); 
 
  delay(100);

  return status;
}

//---------------------------------------------------
//
// This function requests the modem a one short status,
// sending it a command.
//
//---------------------------------------------------
unsigned short LoRaWan_get_status_one_short (byte cmd) {
  unsigned short status;

  // Chip select
  digitalWrite(SPIslaveSelectPin,LOW);
  delay(1);
  SPI.transfer(cmd);
  delay(4); // Wait commend to be processed
  status = SPI.transfer(0x00) << 8; 
  delay(2);
  status |= SPI.transfer(0x00); 
  delay(2);
   // take the SS pin high to de-select the chip:
  digitalWrite(SPIslaveSelectPin,HIGH); 
 
  delay(100);

  return status;
} 

//////////////////////////////////////////////////////
//
//  MIDDLE LEVEL COMMANDS AND STATUS FUNCTIONS 
//
//////////////////////////////////////////////////////

//---------------------------------------------------
//
// Returns one if the device has been activated with
// over the air activation method. This implies that 
// the network replies and agrees this activation. 
//
//---------------------------------------------------
byte LoRaWan_get_status_joined(boolean print) {
  byte status;

  status = LoRaWan_get_status_one_byte(LORAWAN_JOIN_STATUS_CMD);
   
  if (print){
    if (status != 0){
      Serial.println(F("LW: Network JOINED"));
    }
    else {
      Serial.println(F("LW: Network NOT JOINED"));
    }
  }

  return status;
}

//---------------------------------------------------
//
// Returns one if the device has been activated with
// personalization method. 
//
//---------------------------------------------------
byte LoRaWan_get_status_personalized(boolean print) {
  byte status;

  status = LoRaWan_get_status_one_byte(LORAWAN_PERSO_STATUS_CMD);
   
  if (print){
    if (status != 0){
      Serial.println(F("LW: Stack PERSONALIZED"));
    }
    else {
      Serial.println(F("LW: Stack NOT PERSONALIZED"));
    }
  }

  return status;
}

//---------------------------------------------------
//
// Return the status of the network connection. If the
// device has no response from the network for long 
// time, it considers the link as dead. It can continue
// to send data but there is no guaranty they reach the
// network.
//
//---------------------------------------------------
byte LoRaWan_get_status_link_dead(boolean print) {
  byte status;

  status = LoRaWan_get_status_one_byte(LORAWAN_LINK_DEAD_STATUS_CMD);
   
  if (print){
    if (status != 0){
      Serial.println(F("LW: Link DEAD"));
    }
    else {
      Serial.println(F("LW: Link NOT DEAD"));
    }
  }

  return status;
}

//---------------------------------------------------
//
// Returns the LoRaWan datarate used. The datarate 
// values are described in the LoRaWan specification.
//
//---------------------------------------------------
byte LoRaWan_get_datarate(boolean print) {
  byte status;

  status = LoRaWan_get_status_one_byte(LORAWAN_DATARATE_STATUS_CMD);
   
  if (print){
      Serial.print(F("LW: datarate: ") );
      Serial.println(status);
  }

  return status;
}

//---------------------------------------------------
//
// Returns the last frequency used. Return value is a
// frequency in steps of 100kHz.
//
//---------------------------------------------------
short LoRaWan_get_frequency(boolean print){
  short status;

  status = LoRaWan_get_status_one_short (LORAWAN_FREQ_STATUS_CMD);
  if (print){
      Serial.print(F("LW: last frequency: ") );
      Serial.print(status);
      Serial.println(F("00000 Hz"));
  }

  return status;
  
}

//---------------------------------------------------
//
// Requests modem if the last frame to transmit has been sent.
//
//---------------------------------------------------
byte LoRaWan_is_last_tx_sent(boolean print) {
  byte status;
 
  status = LoRaWan_get_status_one_byte(LORAWAN_LAST_TX_SENT_STATUS_CMD);
  if (print){
    if (status)
      Serial.println(F("LW: last TX SENT") );
    else
      Serial.println(F("LW: last TX NOT SENT") );
  }

  return status;
}

//---------------------------------------------------
//
// Requests the modem, how much time is to wait before
// it can send the current frame. The return value is 
// a time in seconds.
//
//---------------------------------------------------
short LoRaWan_tx_wait(boolean print) {
  unsigned short status;

  status = LoRaWan_get_status_one_short (LORAWAN_TX_WAIT_STATUS_CMD);

  if (print && status){
    Serial.print(F("LW: TX delayed : "));
    Serial.print(status);
    Serial.println(F(" s"));
  }

  return status;

}

//---------------------------------------------------
//
// Requests the modem if last transmission has been acked.
//
//---------------------------------------------------
byte LoRaWan_is_tx_acked(boolean print){
  byte status;
  
  status = LoRaWan_get_status_one_short (LORAWAN_TX_ACK_STATUS_CMD);

  if (print){
    if (status == LORAWAN_TX_ACK_STATUS_NO_INFO) {
      Serial.println(F("LW: TX ACK info not available yet"));
    }
    else if (status == LORAWAN_TX_ACK_STATUS_ACK) {
      Serial.println(F("LW: TX ACK"));
    }
    else if (status == LORAWAN_TX_ACK_STATUS_NO_ACK) {
      Serial.println(F("LW: TX NO ACK"));
    }
  }
  return status;
}

//---------------------------------------------------
//
// Requests the modem if downlink data have been received.
// The return value is the number of byte received.
// If there is no message received the function return 0.
//
//---------------------------------------------------
byte LoRaWan_is_rx_available(boolean print){
  byte status;

  status = LoRaWan_get_status_one_short (LORAWAN_RX_AVAILABLE_CMD);
  
  if (print){
    Serial.print(F("LW: RX available: "));
    Serial.println(status);
  }

  return status;
}

//---------------------------------------------------
//
// This function reads downlink frame received byte by
// byte. It is mandatory to use this function when the
// frame size is known (using LoRaWan_is_rx_available).
// When no more data are available, LoRaWan_rx_read 
// returns 0. But it cannot be used as a stop condition
// because regular data can include zeros.
//
//---------------------------------------------------
byte LoRaWan_rx_read(boolean print){
  byte status;

  status = LoRaWan_get_status_one_short (LORAWAN_RX_READ_CMD);
  if(print){
    Serial.println(status, HEX);
  }
  return status;
}

//////////////////////////////////////////////////////
//
//  HIGH LEVEL USER FUNCTIONS
//
//////////////////////////////////////////////////////

//---------------------------------------------------
//
// This function is mandatory in your sketch. You 
// must call it in the setup function. 
// It configures the SPI bus to communicate with the 
// LoRaWan modem. If LoRaWan_verbose is true (unchanged),
// LoRaWan_init() initialize the Arduino serial port for traces.
//
//---------------------------------------------------
void LoRaWan_init() {
  // Init SPI NSS
  pinMode (SPIslaveSelectPin, OUTPUT);
  digitalWrite(SPIslaveSelectPin,HIGH); 


  // initialize SPI:
  SPI.begin(); 
  SPI.setDataMode(SPI_MODE0) ;
  SPI.setClockDivider(SPI_CLOCK_DIV128);

  // Wait SPI to be ready
  delay(500);


  // Reset The LoRaWan modem
  pinMode (ResetPin, OUTPUT);
  digitalWrite(ResetPin,HIGH); 
  delay(100);
  digitalWrite(ResetPin,LOW); 
  delay(50);
  digitalWrite(ResetPin,HIGH); 
  // Let it wake up
  delay(2000);

  // Setup serial line for traces
  if (LoRaWan_verbose) {
    Serial.begin(9600);
    Serial.println(F("LW: Initialized"));
  }
  delay(1000);
}


//---------------------------------------------------
//
// This function is used to register your device on the
// network. It performs device activation by personalization.
// Input parameters are hexadecimal strings without space
// or any additional characters. DevAddr must be 4 bytes 
// (8 characters) long. NwkSKey and AppSKey must be 16
// bytes (32 characters) long. 
//
// Example: LoRaWan_personalization("FF000001",
// "00112233445566778899AABBCCDDEEFF", "000102030405060708090A0B0C0D0E0F");
//
// Note: It is mandatory to register on network to get this library
// working. Use LoRaWan_personalization or LoRaWan_join to do that.
//
//---------------------------------------------------
void LoRaWan_personalization (String DevAddr, String NwkSKey, String AppSKey) {
  String cmd;
  byte status = 0;
  String error_str = F("LW: ERROR personalization: Invalid ");

  do {
    if (DevAddr.length() != 8) {
      if (LoRaWan_verbose) {
        Serial.print(error_str);
        Serial.println(F("DevAddr size") );
        }
      while(1){}
    }
    if (NwkSKey.length() != 32) {
      if (LoRaWan_verbose) {
        Serial.print(error_str);
        Serial.println(F("NwkSKey size"));
      }
      while(1){}
    }
    if (AppSKey.length() != 32) {
      if (LoRaWan_verbose) {
        Serial.print(error_str);
        Serial.println(F("AppSKey size"));
      }
      while(1){}
    }

    cmd = "AT%PERSO";
    cmd += " 00000000"; // fake netid, not useful in personalization mode
    cmd += " " + DevAddr;
    cmd += " " + NwkSKey;
    cmd += " " + AppSKey;

    LoRaWan_send_cmd(cmd);

    delay (2000);

    status = LoRaWan_get_status_personalized(LoRaWan_verbose);
  } while (status == 0);

}


//---------------------------------------------------
//
// This function is used to register your device on the
// network. It performs device over the air activation.
// This activation method is more secured than activation
// by personalization (LoRaWan_personalization). 
// If available on the network, it shall be preferred by the user.
// Input parameters are hexadecimal strings without space
// or any additional characters. AppEUI and DevEUI must be
// 8 bytes (16 characters) long. AppKey must be 16 bytes 
// (32 characters) long. 
//
// Example: LoRaWan_join("0200000000EEFFC0","010000FF010000FF",
// "AB89EFCD2301674554761032DCFE98BA");
// Note: It is mandatory to register on network to get this
// library working. Use LoRaWan_personalization or LoRaWan_join to do that.
//
//---------------------------------------------------
void LoRaWan_join (String AppEUI, String DevEUI, String AppKey) {
  String cmd;
  byte status = 0;
  String error_str = F("LW: ERROR Join: Invalid ");

    if (AppEUI.length() != 16) {
      if (LoRaWan_verbose) {
        Serial.print(error_str);
        Serial.println(F("AppEUI size") );
        }
      while(1){}
    }
     if (DevEUI.length() != 16) {
      if (LoRaWan_verbose) {
        Serial.print(error_str);
        Serial.println(F("DevEUI size") );
        }
      while(1){}
    }
    if (AppKey.length() != 32) {
      if (LoRaWan_verbose) {
        Serial.print(error_str);
        Serial.println(F("AppKey size"));
      }
      while(1){}
    }

    cmd = "AT%JOIN";
    cmd += " " + AppEUI;
    cmd += " " + DevEUI;
    cmd += " " + AppKey;

    LoRaWan_send_cmd(cmd);

  do {
    delay (2000);

    status = LoRaWan_get_status_joined(LoRaWan_verbose);

  } while (status == 0);

  delay (4000);
}

//---------------------------------------------------
//
// This function sends a frame to the network. 
// If there is duty cycle available, the frame is sent
// immediately. If there is no duty cycle available, the
// function waits till the frame can be sent. In this case
// the user is warned by a trace indicating his frame is not
// sent yet and the time to wait before sending.
//
// If acknowledge is requested, the modem will resend the
// frame till it receives the acknowledge status from the 
// network. After 8 attempts without acknowledgement the 
// device gives up and stops to repeat the frame. 
//
// Note: The duty cycles rules are also applied when frame 
// is repeated requesting acknowledgement.
//
// The frame parameter is a hexadecimal string without space
// or any additional characters. Maximum size for a frame
// is 52 bytes (104 characters).
// 
// The port is the data port. Be careful, port 0 is dedicated
// to MAC commands; you cannot transmit data on it.
//
// The acknowledge parameter is a Boolean indicating if an
// acknowledgement must be requested to the network.
//
// Example: LoRaWan_send(“0123456789ABCD”, 1, false);
//
// Included receive feature:
// LoRaWan class A device opens two reception windows just 
// after sending a frame. The network uses these windows to
// transmit downlink messages to the device. LoRaWan_send 
// function waits this two reception windows to be over and
// check if a message is received. If yes the function prints
// the downlink frame in a trace.
//
// Note: Class C device is listening continuously the network
// but also opens a reception window after the TX. So
// LoRaWan_send function behaves the same way as for a 
// class A device. For messages received later on a class C 
// device, it’s up to the user to check periodically if 
// messages are received. This can be done with 
// LoRaWan_get_rx function described below.
//
//---------------------------------------------------
void LoRaWan_send(String frame, byte port, boolean acknowledge) {
  short i;
  short invalid_frame = 0;
  String cmd;
  unsigned long wait;
  byte ack_status;
  byte avail;

  if (port == 0) {
    if (LoRaWan_verbose) {
      Serial.println(F("LW: WARNING: LoRaWan_send: port 0 is dedicated to MAC commands"));
    }
  }

  if (frame.length() > 104){
    if (LoRaWan_verbose) {
      Serial.print(F("LW: ERROR: LoRaWan_send: data too long: "));
      Serial.print(frame.length()/2);
      Serial.println(F(". Reduce to 52 bytes"));
    }

    frame[52] ='\0';

  }

  for (i=0; i<frame.length() && invalid_frame == 0 ; i++) {
    if (frame[i] == '0'
        || frame[i] == '1'
        || frame[i] == '2'
        || frame[i] == '3'
        || frame[i] == '4'
        || frame[i] == '5'
        || frame[i] == '6'
        || frame[i] == '7'
        || frame[i] == '8'
        || frame[i] == '9'
        || frame[i] == 'A'
        || frame[i] == 'B'
        || frame[i] == 'C'
        || frame[i] == 'D'
        || frame[i] == 'E'
        || frame[i] == 'F'
        || frame[i] == 'a'
        || frame[i] == 'b'
        || frame[i] == 'c'
        || frame[i] == 'd'
        || frame[i] == 'e'
        || frame[i] == 'f' ) {
          // char ok, do nothing
        }
    else {
      if (LoRaWan_verbose) {
        Serial.print(F("LW: ERROR: LoRaWan_send: invalid data: "));
        Serial.print(frame[i]);
        Serial.println(F(" , replaced by 0. Only hexadecimal allowed"));
      }
      frame[i] = '0';
    }

  }

  cmd = "AT%SEND ";
  cmd += frame;
  cmd += " ";
  cmd += String(acknowledge);
  cmd += " ";
  cmd += String(port);

  if(LoRaWan_verbose) {
    Serial.print(F("LW: Trying to send: "));
    Serial.print(frame);
    Serial.print(F(" port: "));
    Serial.print(port);
    if (acknowledge) {
      Serial.println(F(" ACK"));
    }
    else {
      Serial.println(F(" NO ACK"));
    }
  }
  
  LoRaWan_send_cmd(cmd);

  delay (4000);

  // Manage post sending status
  while ( !LoRaWan_is_last_tx_sent(false) ) {
    if (!acknowledge){
      wait = LoRaWan_tx_wait(false);

      if (wait == 0) {
        // Maybe we are receiving a long RX, wait 1 more second
        delay(1000);
      }
      else {
        if (LoRaWan_verbose) {
          Serial.print(F("LW: No duty cycle available. Wait: "));
          Serial.print(wait);
          Serial.println(F(" s"));
        }
        wait = wait*1000 + 4000;
        delay (wait);
      }

    }
    else {
      // acknowledge == true
      ack_status = LoRaWan_is_tx_acked(false);
      if (ack_status == LORAWAN_TX_ACK_STATUS_NO_INFO) {
        wait = LoRaWan_tx_wait(false);

        if (wait == 0) {
          // Maybe we are receiving a long RX, wait 1 more second
          delay(1000);
        }
        else {
          if (LoRaWan_verbose) {
            Serial.print(F("LW: No duty cycle available. Wait: "));
            Serial.print(wait);
            Serial.println(F(" s"));
          }
          wait = wait*1000 + 4000;
          delay (wait);
        }
      }
      else if (ack_status == LORAWAN_TX_ACK_STATUS_NO_ACK) {
        wait = LoRaWan_tx_wait(false);
        if (LoRaWan_verbose) {
          Serial.print(F("LW: TX sent but NO ACK received. Resend in : "));
          Serial.print(wait);
          Serial.println(F(" s"));
        }
        wait = wait*1000 + 4000;
        delay (wait);
      }

    }

  }

  if (LoRaWan_verbose) {
    Serial.println(F("LW: TX SENT"));
    if (acknowledge){
      LoRaWan_is_tx_acked(true);
    }
  }
  
  // Check if we received a message
  avail = LoRaWan_is_rx_available(LoRaWan_verbose);

  LoRaWan_last_rx_size = avail;

  if (avail) {
    i = 0;
    while(avail) {
      LoRaWan_last_rx[i] = LoRaWan_rx_read(LoRaWan_verbose);
      i++;
      avail--;
    }
  }
}

//---------------------------------------------------
//
// This function does the same as this described above
// except it use a byte pointer for the frame parameter.
// The len is the length of the data to send.
// This function may be easier to use to build user 
// frame than with string parameter.
//
// Example using a byte array:
// byte buf[] = {0,1,2,3};
// LoRaWan_send(buf, 4,  1, false);
//
//---------------------------------------------------
void LoRaWan_send(byte *frame, byte len, byte port, boolean acknowledge) {
  String str="";
  short i;
  String temp;

  for (i=0; i<len; i++){
    temp  = String(frame[i], HEX);
    if (temp.length() == 1){
      temp = '0' + temp;
    }
    str = str + temp;

  }

  LoRaWan_send(str, port, acknowledge);

}

//---------------------------------------------------
//
// This function checks if the modem received a message.
// It returns the number of bytes received. If there is
// no message received the function returns 0.
//
// When a message is received, its content is stored in
// global array LoRaWan_last_rx.
//
// In LoRaWan class A mode the user sketch can check if 
// a message is received after sending a message. In this
// mode it is useless to check RX periodically because 
// the device can only receive after sending a frame.
// In LoRaWan class C mode, the user must check periodically 
// if a message has been received by the node.
//
// Example: 
// byte rx_len;
//
// rx_len = LoRaWan_get_rx();
// if (rx_len) {
//     // Get last frame in LoRaWan_last_rx and process it
// }
//
//---------------------------------------------------
byte LoRaWan_get_rx() {
  byte res;
  byte avail;
  byte i;

  if (LoRaWan_last_rx_size){
    // RX data already stored in LoRaWan_last_rx during TX procedure
    res = LoRaWan_last_rx_size;
    LoRaWan_last_rx_size = 0;
  }
  else {
    // Check if we received a message
    avail = LoRaWan_is_rx_available(LoRaWan_verbose);

    LoRaWan_last_rx_size = 0;
    res = avail; 
    if (avail) {
      i = 0;
      while(avail) {
        LoRaWan_last_rx[i] = LoRaWan_rx_read(LoRaWan_verbose);
        i++;
        avail--;
      }
    }


  }

  return res;
}

//---------------------------------------------------
//
// This function select LoRaWan class.
// Available class are A or C.
// By default the modem is class A.
// 
// Example: LoRaWan_class("C");
//
//---------------------------------------------------
void LoRaWan_class(String in) {
  String cmd = "AT%CLASS ";

  if (in != "A" && in != "C" && in != "a" && in != "c") {
    if (LoRaWan_verbose) {
      Serial.println (F("LW:Unsuported class"));
    }
    return;
  }

  cmd += in;

  LoRaWan_send_cmd(cmd);

  delay (2000);
}

//---------------------------------------------------
//
// This function set the modem in power saving mode.
// Note 1: After going to sleep it must be woken up
// with LoRaWan_modem_wake_up(). No other command can
// be sent.
// Note 2: It is not possible to make a Class C device 
// sleep.
//
//---------------------------------------------------
void LoRaWan_modem_sleep(){
  delay(500);
  // init wakeup pin
  pinMode (WakeupPin, OUTPUT);
  digitalWrite(WakeupPin,LOW);
  LoRaWan_get_status_one_byte (LORAWAN_MODEM_SLEEP);
}

//---------------------------------------------------
//
// This function wake up the modem when it is sleeping
//
//---------------------------------------------------
void LoRaWan_modem_wake_up(){
  digitalWrite(WakeupPin,HIGH);
  // Wait the modem to wake up
  delay(2000);
}

