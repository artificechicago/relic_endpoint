#include "relic_rng.h"
#include "VNLN.h"

// Cryptographically secure pseudorandom number generator
// The RNG is initialized with the number of an unused pin.
// Entropic bits are exctracted from the unused pin by running
// a von Neumann extractor on LSBs of the voltage measurement sequence.
// The entropic bits are used to generate a key, which is used to
// encrypt the counting sequence 0, 1, 2, ... (i.e. we run the cipher
// in CTR mode).  This generates a random 64-bit number in a secure fashion.
// We then treat this 64-bit number as an array of 8 random 8-bit numbers.

static uint8_t randomSource[8];
static uint8_t loc;
static uint32_t counter[2];
static uint32_t keySeed[4];

void generateRandomSet() {
	// increment counter as a 64-bit number
	if (counter[1] < 0xFFFFFFFF) {
		counter[1]++;
	} else {
		counter[0]++;
		counter[1] = 0x00000000;
	}
	memcpy(randomSource, counter, 8); // encrypts acts in-place
	encrypt((uint32_t *)randomSource, keySeed);
}

void initializeRNG(int noisePin) {
	vonNeumannNoiseExtractor((uint8_t *)keySeed, 16, noisePin);
	counter[0] = 0x00000000;
	counter[1] = 0x00000000;
	generateRandomSet();
	loc = 0;
}

uint8_t rand_val() {
	// This function is the actual user interface for getting randoms from
	// the CSPRNG.
	uint8_t randVal;
	if (loc < 8) { // pre-generated random numbers left
		randVal = randomSource[loc];
		loc++;
	} else { // Generate a new set of 8 random numbers 
		generateRandomSet();
		randVal = randomSource[0];
		loc = 1;
	}

	return randVal;
}
