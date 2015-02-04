// Von Neumann Line Noise Module

/* 
Provides a function to generate uniformly distributed
random bytes from an uninitialized pin.

If the pin is given a constant voltage source, this will
cause the system to go into an infinite loop.  Currently,
this is intended behavior to cause a hardware error in the
event of an attacker with physical access to the machine
attempting to manipulate the RNG.
*/

#include "VNLN.h"

void vonNeumannNoiseExtractor (uint8_t * dest, int destSize, int srcPin)
{
	int index;
	int bitIndex;
	uint8_t collector;
	uint8_t pinReading1;
	uint8_t pinReading2;

	for (index = 0; index < destSize; index++) {
		
		for (bitIndex = 0; bitIndex < 8; bitIndex++) {
			do {
				pinReading1 = analogRead(srcPin);
				delay(10);
				pinReading2 = analogRead(srcPin);
			} while (!((pinReading1 & 1) ^ (pinReading2 & 1)));
			collector <<= 1;
			collector += pinReading1 & 1;
		}

		dest[index] = collector;
	}
}