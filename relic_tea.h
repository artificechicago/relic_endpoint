#ifndef __RELIC_TEA_H__
#define __RELIC_TEA_H__

#include <stdint.h>
#define ENCRYPTION_MESSAGE_BUFFER_SIZE 60

typedef uint8_t (*RNG)();

typedef struct _TEA_CRYPTO {
	RNG rng;
	uint32_t key[4];
	uint32_t MACKey[4];
	uint8_t MACBuffer[8];
	uint8_t MessageBuffer[ENCRYPTION_MESSAGE_BUFFER_SIZE];
	uint8_t IVBuffer[8];
	void (*encrypt)(uint32_t *, uint16_t);
	void (*decrypt)(uint32_t *, uint16_t, uint32_t *);
	void (*MAC)(uint32_t *, uint16_t);
} Crypto;

Crypto * initializeCrypto(RNG, uint32_t *);
void cryptoSetNewKey(uint32_t *);

#endif