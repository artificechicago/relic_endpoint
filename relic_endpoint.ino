#include <SPI.h>
//#include <RFID.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "CoAP.h"

#define DEBUG

#if UDP_TX_PACKET_MAX_SIZE != 64
#error Wrong UDP Packet Size! Change UDP_TX_PACKET_MAX_SIZE to 64 in EthernetUdp.h
// EthernetUdp.h defines UDP_TX_PACKET_MAX_SIZE to be 24 by default,
// but we need at least 64 bytes for our packets, and this code assumes 64.
// Increase the size in the EthernetUdp.h file.
// On Linux Mint, the libraries are in usr/share/arduino/libraries
#endif

static unsigned char mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

static const unsigned int localPort = 5683;

unsigned char incomingBuffer[UDP_TX_PACKET_MAX_SIZE];
char outgoingBuffer[UDP_TX_PACKET_MAX_SIZE];

EthernetUDP Udp;

#ifdef DEBUG

void printDebug()
{
	return;
}

#endif

void setup() 
{
	Serial.begin(9600);

	#ifdef DEBUG
	Serial.println("System initialized.  Aquiring IP Address.");
	#endif

	if (Ethernet.begin(mac) == 0) {
		#ifdef DEBUG
		Serial.println("Error occured with DHCP");
		#endif
		for (;;) {}
	} else {
		#ifdef DEBUG
		Serial.print("Local IP at: ");
		Serial.println(Ethernet.localIP());
		#endif
	}
	Udp.begin(localPort);
	
	#ifdef DEBUG
	Serial.print("Listenting on port ");
	Serial.println(localPort);
	#endif
}

void loop() 
{
	int packetSize = Udp.parsePacket();
	if (packetSize) {
		Serial.print("Recieved packet of size: ");
		Serial.println(packetSize);
		Serial.print("From ");
		IPAddress remote = Udp.remoteIP();
		for (int i = 0; i < 4; i++) {
			Serial.print(remote[i], DEC);
			if (i < 3) {
				Serial.print(".");
			}
		}
			Serial.print(":");
			Serial.println(Udp.remotePort());

		Udp.read(incomingBuffer, UDP_TX_PACKET_MAX_SIZE);
		Serial.println("Contents");
		int printLen = (packetSize > UDP_TX_PACKET_MAX_SIZE) ? 
			UDP_TX_PACKET_MAX_SIZE : packetSize;
		for (int i = 0; i < printLen; i++) {
			if (incomingBuffer[i] <= 0xF) {
				Serial.print("0");
			}
			Serial.print(incomingBuffer[i], HEX);
			Serial.print(":");
			if ((i > 0) && ((i+1) % 8 == 0)) {
				Serial.println("");
			}
		}
		Serial.println("");

		//Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
		//Udp.write(outgoingBuffer);
		//Udp.endPacket();
	}
	delay(10);
}
