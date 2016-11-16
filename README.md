# arduino_LoRaWan_Wi6labs
Arduino Library for Wi6labs LoRaWan shields

USER Guide :

1.1 Scope

LoRaWan is the protocol supported by the LoRa Alliance. It is designed to build public or private LoRa networks. Nowadays many worldwide operators deploy or plan to deploy LoRaWan networks.
Wi6labs provide several solutions to evaluate this technology:
An Arduino shield
An Arduino like board embedding a LoRaWan modem
This document describes how to use the Arduino library for LoRaWan products from Wi6labs.

1.2 References

https://www.arduino.cc/ 
https://www.lora-alliance.org/ 

1.3 Glossary

MAC: Medium Access Control
ISM band: industrial, scientific and medical radio bands

2 General information

2.1 About LoRaWan

LoRaWan is an MAC (Medium Access Control) layer “improved” to access a network. We say improved because it does more than a traditional MAC layer.

2.1.1 Security

LoRaWan includes security features to cypher data exchanges and to authenticate LoRa nodes and networks.
To get this library working, it is mandatory to have valid Adress(es) and Key(s) registered on a network. Of course this network must be reachable in a radio link point of view.
LoRaWan specifies two methods to connect on a network:
Activation By Personalization (ABP)
Over The Air Activation (OTAA)
A given network can implement one of the two methods or both.
Wi6labs LoRaWan supports both activation methods. Please view function LoRaWan_join for OTAA and LoRaWan_personalization for ABP.

2.1.2 Network topology

LoRaWan defines a star network between nodes and a gateway. It is not possible to use this library in another way. For example it is not possible to communicate directly from one device to another.

2.1.3 Duty cycle

LoRaWan communicates over ISM bands. These bands are free to use but impose rules to each device using it.
For example ISM European band impose to do not transmit more than X% of the time. X depends of the sub-band used. On a 1% sub-band, if a device emits a 1 second message at startup, it must wait 99 seconds before sending a new one.
This constraint is handled by the Wi6labs LoRaWan modem. This implies that the Arduino application, developed by customer, cannot send LoRa message every time. This is not a bug; it is ISM specification rules respect.

2.1.4 LoRa datarate and Duty cycle 

Regarding the signal strength of message received by the network gateway, the network can adapt nodes data rate. If the node is close to the gateway, the network will configure it to send data on a high data rate. On the other hand, if the node is far from the gateway, network will configure it with a low data rate.
Lower data rate implies longer message for the same amount of data. The duty cycle is always the same on a sub-band, so on a low data rate the application must wait more time before sending a message.

2.2 Arduino Library Philosophy 

We decided to develop the library in Arduino code. There is no zip or cpp file to install in the IDE. You just have to add the library files in your application sketch.
We choose to do that to allow the user to easily hack it if needed.
There are three function layers in the library:
Low level access to the SPI bus communicating with the LoRaWan modem.
Middle level commands and status functions to immediately send commands or receive status.
High level user functions, easy to use and managing the timing of LoRaWan commands and status.
Basically a user can use only High level functions and access to all LoRaWan functionalities. These functions are described first in this guide. If their behaviors satisfied your needs, there is no need to go beyond and to read how the lower functions work.

The library is also sending traces over Arduino serial port by default. This can be disabled by user.

For several reasons the user may want to modify the library:
Timings imposed by the high level functions are not satisfying for the real time behavior of the application.
Application and library code size is too large for the Arduino.
Too much traces or missing one.
Other reasons we didn’t anticipate
In this case the user can easily modify functions, suppress those which are not needed and create new ones. Of course, hack at your own risk, Wi6labs didn’t guaranty the behavior of the LoRaWan modem driven by a modified library.


3 Library usage

3.1 Downloading the library

You can start playing with the library by downloading it directly ati https://github.com/Wi6labs/arduino_LoRaWan_Wi6labs
Then open the arduino_LoRaWan_wi6labs.ino with your arduino IDE.

3.2 Verbosity

By default LoRaWan library writes traces on the Arduino Serial. If you don’t want traces or if you need the Serial port for another purpose, you can disable traces. To do it, write this line at the beginning of your setup() function:
LoRaWan_verbose = false;

Of course you can use Serial.print() and other Serial function in your code to create your own traces. There is no need to call Serial.begin() because it is already done in LoRaWan_init(), as explain below.

3.3 High level user functions 

3.3.1 void LoRaWan_init() 

This function is mandatory in your sketch. You must call it in the setup function. 
It configures the SPI bus to communicate with the LoRaWan modem.
If LoRaWan_verbose is true (unchanged), LoRaWan_init() initialize the Arduino serial port for traces.

3.3.2 void LoRaWan_class(String class) 

This function select LoRaWan class.
Available class are A or C.
By default the modem is class A.

In Class A, the device only receives during two short receive windows after the transmission. 
In Class C, the device is always receiving.

Example: LoRaWan_class("C");

3.3.3 void LoRaWan_join (String AppEUI, String DevEUI, String AppKey) 

This function is used to register your device on the network. It performs device Over The Air Activation (OTAA). This activation method is more secured than activation by personalization (LoRaWan_personalization). If available on the network, it shall be preferred by the user.
Input parameters are hexadecimal strings without space or any additional characters. AppEUI and DevEUI must be 8 bytes (16 characters) long MSB first. AppKey must be 16 bytes (32 characters) long. 
Example: LoRaWan_join("C0FFEE0000000002","FF000001FF00001","0AB453CD2301674554761032DCFE98BA");
Note: It is mandatory to register on network to get this library working. Use LoRaWan_personalization or LoRaWan_join to do that.

3.3.4 void LoRaWan_personalization (String DevAddr, String NwkSKey, String AppSKey) 

This function is used to register your device on the network. It performs device activation by personalization.
Input parameters are hexadecimal strings without space or any additional characters. DevAddr must be 4 bytes (8 characters) long. NwkSKey and AppSKey must be 16 bytes (32 characters) long. 
Example: LoRaWan_personalization("FF000001","00112233445566778899AABBCCDDEEFF", "000102030405060708090A0B0C0D0E0F");
Note: It is mandatory to register on network to get this library working. Use LoRaWan_personalization or LoRaWan_join to do that.

3.3.5 void LoRaWan_send(String frame, byte port, boolean acknowledge) 

This function sends a frame to the network. 
If there is duty cycle available, the frame is sent immediately. If there is no duty cycle available, the function waits till the frame can be sent. In this case the user is warned by a trace indicating his frame is not sent yet and the time to wait before sending.
If acknowledge is requested, the modem will resend the frame till it receives the acknowledge status from the network. After 8 attempts without acknowledgement the device gives up and stops to repeat the frame. 
Note: The duty cycles rules are also applied when frame is repeated requesting acknowledgement.
The frame parameter is a hexadecimal string without space or any additional characters. Maximum size for a frame is 52 bytes (104 characters).
The port is the data port. Be careful, port 0 is dedicated to MAC commands; you cannot transmit data on it. Allowed port for data are 1 to 223.
The acknowledge parameter is a Boolean indicating if an acknowledgement must be requested to the network.
Example: LoRaWan_send(“0123456789ABCD”, 1, false);
Included receive feature:
LoRaWan class A device opens two reception windows just after sending a frame. The network uses these windows to transmit downlink messages to the device. LoRaWan_send function waits this two reception windows to be over and check if a message is received. If yes the function prints the downlink frame in a trace.
Note: Class C device is listening continuously the network but also opens a reception window after the TX. So LoRaWan_send function behaves the same way as for a class A device. For messages received later on a class C device, it’s up to the user to check periodically if messages are received. This can be done with LoRaWan_get_rx function described below.

3.3.6 void LoRaWan_send(byte *frame, byte len, byte port, boolean acknowledge) 

This function does the same as this described above except it use a byte pointer for the frame parameter.
The len is the length of the data to send.
This function may be easier to use to build user frame than with string parameter.
Example using a byte array:
byte buf[] = {0,1,2,3};
LoRaWan_send(buf, 4,  1, false);

3.3.7 byte LoRaWan_get_rx() 

This function checks if the modem received a message. It returns the number of bytes received. If there is no message received the function returns 0.
When a message is received, its content is stored in global array LoRaWan_last_rx.
In LoRaWan class A mode the user sketch can check if a message is received after sending a message. In this mode it is useless to check RX periodically because the device can only receive after sending a frame.
In LoRaWan class C mode, the user must check periodically if a message has been received by the node.

Example: 
byte rx_len;

rx_len = LoRaWan_get_rx();
if (rx_len) {
// Get last frame in LoRaWan_last_rx and process it
}

3.3.8 void LoRaWan_modem_sleep()

This function set the modem in power saving mode.

Note 1: After going to sleep it must be woken up with LoRaWan_modem_wake_up(). No other command can be sent.
Note 2: It is not possible to make a Class C device sleep.

3.3.9 void LoRaWan_modem_wake_up()

This function wake up the modem when it is sleeping.

3.4 Middle level commands and status functions 

These functions are used in High level functions to request status and information to the modem. They all have a Boolean as input indicating if a trace of the result is requested. They can be used by the application developer in his sketch. 

3.4.1 byte LoRaWan_is_last_tx_sent(boolean print) 

Requests modem if the last frame to transmit has been sent.
0: Not sent
1: Sent

3.4.2 short LoRaWan_tx_wait(boolean print)

Requests the modem, how much time is to wait before it can send the current frame. The return value is a time in seconds.

3.4.3 byte LoRaWan_is_tx_acked(boolean print)

Requests the modem if last transmission has been acked.
0: No information available about Ack message
1: Messag acked
2: No Ack

3.4.4 byte LoRaWan_is_rx_available(boolean print)

Requests the modem if downlink data have been received. The return value is the number of byte received. If there is no message received the function return 0.

3.4.5 byte LoRaWan_rx_read(boolean print)

This function reads downlink frame received byte by byte. It is mandatory to use this function when the frame size is known (using LoRaWan_is_rx_available). When no more data are available, LoRaWan_rx_read returns 0. But it cannot be used as a stop condition because regular data can include zeros.

3.4.6 short LoRaWan_get_frequency(boolean print)

Returns the last frequency used. Return value is a frequency in steps of 100kHz.
For example this function returns 8681 for frequency 868100 kHz.

3.4.7 byte LoRaWan_get_datarate(boolean print) 

Returns the LoRaWan datarate used. The datarate values are described in the LoRaWan specification.

3.4.8 byte LoRaWan_get_status_link_dead(boolean print) 

Return the status of the network connection. If the device has no response from the network for long time, it considers the link as dead. It can continue to send data but there is no guaranty they reach the network.

3.4.9 byte LoRaWan_get_status_personalized(boolean print)

Returns 1 if the device has been activated with personalization method. 

3.4.10 byte LoRaWan_get_status_joined(boolean print) 

Returns 1, if the device has been activated with over the air activation method. This implies that the network replies and agrees this activation. 

3.5 Low level functions for SPI access 

3.5.1 byte LoRaWan_get_status_one_byte (byte cmd) 

This function requests the modem a one byte status, sending it a command.

3.5.2 unsigned short LoRaWan_get_status_one_short (byte cmd) 

This function requests the modem a one short status, sending it a command.

3.5.3 void LoRaWan_send_cmd(String in) 

This function is used to send AT commands to the modem.




