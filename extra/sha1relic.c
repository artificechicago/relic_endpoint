// Implements the SHA-1 hash algorithm
#include <stdint.h>
#include <stdbool.h>
#include "sha1relic.h"
// Module-level global constant for the hash result
// This means we can only ever have a single hash result
// 'waiting' at one time, and it needs to be copied out
// before hashing another message or it will be lost.
// This is to prevent dynamic memory allocation so we have fixed
// memory usage for this module, and to also obviate the need for
// the caller to provide a memory chunk to catch the hash result,
// which makes it easier for this module to provide utility functions
// to write out the hash in different forms.

// TODO: Refactor the copy operations from the message to the chunk
// buffer as separate functions to de-duplicate the for-loops and
// switch.

static unsigned char hashResultBuffer[20];

uint32_t swap32BitEndianess(uint32_t w) {
	return  ((w >> 24) & 0xff) |
			((w << 8) & 0xff0000) |
			((w >> 8) & 0xff00) |
			((w << 24) & 0xff000000));
}

void _RELIC_SHA1_HASH_(unsigned char * msg, uint8_t len) {
	//Nothing up my sleeve constants to initialize hash values
	uint32_t h0 = 0x67452301;
	uint32_t h1 = 0xEFCDAB89;
	uint32_t h2 = 0x98BADCFE;
	uint32_t h3 = 0x10325476;
	uint32_t h4 = 0xC3D2E1F0;

	uint32_t messageChunk[16];
	uint32_t messageChunkWordBuffer;

	uint8_t rLen = len;
	bool onePad = false;
	bool finishedHashing = false;
	uint8_t index;
	uint8_t indexMax;

	do {
		if (rLen > 64) { // We have a full chunk
			for (index = 0; index < 16; index++) {
				messageChunkWordBuffer = *msg << 24 + 
										*(msg+1) << 16 + 
										*(msg+2) << 8 + 
										*(msg+3);
				messageChunk[index] = swap32BitEndianess(messageChunkWordBuffer);
				msg += 4;
			}
			rLen -= 64;
		} else if (rLen > 55) { // Less than a full chunk, but not enough for len
			indexMax = (rLen - (rLen % 4)) / 4; 
			for (index = 0; index < indexMax; index++) {
				messageChunkWordBuffer = *msg << 24 +
										*(msg+1) << 16 +
										*(msg+2) << 8 +
										*(msg+3);
				messageChunk[index] = swap32BitEndianess(messageChunkWordBuffer);
				msg += 4;
			}
			switch (rLen % 4) {
				case 1:
				messageChunkWordBuffer = *msg << 24 +
										0x80 << 16;
				break;
				case 2:
				messageChunkWordBuffer = *msg << 24 +
										*(msg+1) << 16 +
										0x80 << 8;
				break;
				case 3:
				messageChunkWordBuffer = *msg << 24 +
										*(msg+1) << 16 +
										*(msg+2) << 8 +
										0x80;
				break;
				case 0:
				//fall through to default
				default:
				messageChunkWordBuffer = 0x80 << 24;
			}
			messageChunk[indexMax] = swap32BitEndianess(messageChunkWordBuffer);
			onePad = true;

			for (index = indexMax + 1; index < 16; index++) {
				messageChunk[index] = 0x00000000;
			}
			rLen = 0;
		} else { // enough space to write 64-bit len
			indexMax = (rLen - (rLen % 4)) / 4;
			if (rLen > 0) { 
				for (index = 0; index < indexMax; index++) {
					messageChunk[index] = *msg << 24 +
										*(msg+1) << 16 +
										*(msg+2) << 8 +
										*(msg+3);
					msg += 4;
				}
			}
			if (!onePad) {
				switch (rLen % 4) {
				case 1:
				messageChunk[indexMax] = *msg << 24 +
										0x80 << 16;
				break;
				case 2:
				messageChunk[indexMax] = *msg << 24 +
										*(msg+1) << 16 +
										0x80 << 8;
				break;
				case 3:
				messageChunk[indexMax] = *msg << 24 +
										*(msg+1) << 16 +
										*(msg+2) << 8 +
										0x80;
				break;
				case 0:
				//fall through to default
				default:
				messageChunk[indexMax] = 0x80 << 24;
			}

			for (index = (onePad ? indexMax + 1 : indexMax); index < 15; index ++) {
				messageChunk[index] = 0x00000000;
			}

			messageChunk[15] = ((uint32_t)(len) >> 24) & 0xff;  //enddianess swap
			finishedHashing = true;
			onePad = true;
		}	
	} while (!finishedHashing)