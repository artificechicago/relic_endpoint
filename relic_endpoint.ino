#include <SPI.h>
//#include <RFID.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <string.h>
#include "CoAP.h"
#include "relic_rng.h"
#include "relic_tea.h"

#define DEBUG

#if UDP_TX_PACKET_MAX_SIZE != 64
#error Wrong UDP Packet Size! Change UDP_TX_PACKET_MAX_SIZE to 64 in EthernetUdp.h
// EthernetUdp.h defines UDP_TX_PACKET_MAX_SIZE to be 24 by default,
// but we need at least 64 bytes for our packets, and this code assumes 64.
// Increase the size in the EthernetUdp.h file.
// On Linux Mint, the libraries are in usr/share/arduino/libraries
#endif

int freeRam ()
{
 extern int __heap_start, *__brkval;
 int v;
 return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

static unsigned char mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

static const unsigned int localPort = 5683;

unsigned char incomingBuffer[UDP_TX_PACKET_MAX_SIZE];
unsigned char outgoingBuffer[UDP_TX_PACKET_MAX_SIZE];

EthernetUDP Udp;

#ifdef DEBUG

IPAddress ip(10, 0, 0, 10);

static unsigned char fakeRFID[] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
static unsigned char deviceID[] = {0x01, 0x02, 0x03, 0x04};
static unsigned char fakeHeader[] = {0x00, 0xA0};
static unsigned char fakeIV[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint32_t deviceKey[] = {0, 0, 0, 0};
static coapMsg testMsg;
static packedMsg testPMsg;
static RNG rng;
static Crypto * crypto;

#endif

void setup() 
{
	initializeRNG(A0);
	rng = &rand_val;
	initCoAP(rng);
	crypto = initCrypto(rng, deviceKey);
	Serial.begin(9600);
	Serial.println(freeRam());
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
	Serial.print("Listening on port ");
	Serial.println(localPort);

	testMsg.header = 0x5F;
	testMsg.code = 0x00;
	testMsg.msgID = 0x0001;
	memcpy(testMsg.token, fakeIV, 8);
	memcpy(testMsg.body, deviceID, 4);
	memcpy(testMsg.body + 4, fakeRFID, 16);
	testMsg.msgSize = 46;

	CoAP_packMessage(&testPMsg, &testMsg);
	Serial.println(freeRam());
	#endif
}

void loop() {
	#ifdef DEBUG
	int i;
	for (i=0; i < UDP_TX_PACKET_MAX_SIZE; i++) {
		outgoingBuffer[i] = 0x00;
	}
	memcpy(outgoingBuffer, fakeHeader, 2);
	memcpy(outgoingBuffer + 2, fakeIV, 8);
	Serial.println("Start Crypto Test");
	memcpy(crypto->MessageBuffer, testPMsg.msg, testPMsg.size);
	crypto->encrypt();
	memcpy(testPMsg.msg, crypto->MessageBuffer, testPMsg.size);
	memcpy(outgoingBuffer + 10, testPMsg.msg, testPMsg.size);

	int error = Udp.beginPacket(ip, 5683);
	if (error != 0) {
		error = Udp.write(outgoingBuffer, UDP_TX_PACKET_MAX_SIZE);
		if (error != 0) {
			error = Udp.endPacket();
			if (error != 0) {}
		}
	}
	#endif
	delay(2000);
}
