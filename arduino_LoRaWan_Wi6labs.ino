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


Description: Arduino LoRaWan application example 
-----------------------------------------------------------------------------*/                                                             
#include "arduino_LoRaWan_includes.h"

void setup() {

  // Uncomment this to suppress traces
  // LoRaWan_verbose = false;

  // Initialize communication with LoRaWan modem
  LoRaWan_init();

  // Uncomment to set the device to Class C. By default the modem is class A
  // LoRaWan_class("C");

  // Start joining procedure to register on the network
    LoRaWan_join ("C0FFEE0000000002", "FF000001FF000001", "00112233445566778899AABBCCDDEEFF");
 
  // Alternative registration procedure: Activation by personalization
 // LoRaWan_personalization("FF000001","00112233445566778899AABBCCDDEEFF", "000102030405060708090A0B0C0D0E0F"); 

}

// Class A example
void loop () {
   byte buf[] = {0,1,2,3};
   byte rx_len;
   short i;

  // Send data buffer
  LoRaWan_send(buf, 4,  1, false);

  // Check if RX received
  rx_len = LoRaWan_get_rx();
  if (rx_len) {
      // Process data from buffer LoRaWan_last_rx[i]
      // Just print it in this example
      for (i=0; i<rx_len; i++){
        Serial.println(LoRaWan_last_rx[i], HEX);
      }
  }
  // Wait 20 seconds
  delay(20000);
}

// Class C example
/*
void loop () {
   byte buf[] = {0,1,2,3};
   byte rx_check;
   byte rx_len;
   short i;

  LoRaWan_send(buf, 4,  1, false);
  delay(1000);

  // Check 10 times if a message is received
  rx_check = 10;
  while (rx_check --) {
    rx_len = LoRaWan_get_rx();

    if (rx_len) {
      Serial.print("Data len:");
      Serial.println(rx_len);
      // Process data from buffer LoRaWan_last_rx[i]
      // Just print it in this example
      for (i=0; i<rx_len; i++){
        Serial.println(LoRaWan_last_rx[i], HEX);
      }
    }

    delay(  1000);
  }
}
*/
