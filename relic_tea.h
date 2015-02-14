#ifndef __RELIC_TEA_H__
#define __RELIC_TEA_H__

#include <stdint.h>
#define ENCRYPTION_MESSAGE_BUFFER_SIZE 64
#if ENCRYPTION_MESSAGE_BUFFER_SIZE % 8 != 0
#error --ENCRYPTION_MESSAGE_BUFFER_SIZE must be a multiple of 8--
#endif

typedef uint8_t (*RNG)();

typedef struct _TEA_CRYPTO {
	RNG rng;
	uint32_t key[4];
	uint32_t MACKey[4];
	uint8_t MACBuffer[8];
	uint8_t MessageBuffer[ENCRYPTION_MESSAGE_BUFFER_SIZE];
	uint8_t IVBuffer[8];
	void (*encrypt)();
	void (*decrypt)(uint32_t *, uint16_t, uint32_t *);
	void (*MAC)(uint32_t *, uint16_t);
	void (*setKey)(uint32_t *);
	void (*setMACKey)(uint32_t *);
} Crypto;

#ifdef __cplusplus
extern "C"{
#endif

Crypto * initCrypto(RNG, uint32_t *);

#ifdef __cplusplus
}
#endif //extern "C"
#endif