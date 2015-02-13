#ifndef __RELIC_SHA1_H__
#define __RELIC_SHA1_H__

#include <stdint.h>

// Header for SHA-1 implementation
// Define settings are used to control which utility functions
// for the SHA-1 library are compiled, making it possible to
// cut down the compiled binary by omitting unusued utility functions.
// By default, we should have uint32/16/8 set to true, and unsigned char
// set to false, since it's technically redudant with uint8_t.

// To omit a utility funtion, set the appropriate use DEFINE to false.

#define USE_RELIC_SHA1_UINT32 true
#define USE_RELIC_SHA1_UINT16 true
#define USE_RELIC_SHA1_UINT8  true
#define USE_RELIC_SHA1_UCHAR  false

#if USE_RELIC_SHA1_UINT32
#define EXPAND_RELIC_SHA1_UINT32_FN void (*copyUint32) (_RELIC_SHA1_STRUCT_ *, uint32_t *);
#else
#define EXPAND_RELIC_SHA1_UINT32_FN
#endif

#if USE_RELIC_SHA1_UINT16
#define EXPAND_RELIC_SHA1_UINT16_FN void (*copyUint32) (_RELIC_SHA1_STRUCT_ *, uint32_t *);
#else
#define EXPAND_RELIC_SHA1_UINT16_FN
#endif

#if USE_RELIC_SHA1_UINT8
#define EXPAND_RELIC_SHA1_UINT8_FN void (*copyUint8) (_RELIC_SHA1_STRUCT_ *, uint8_t *);
#else
#define EXPAND_RELIC_SHA1_UINT8_FN
#endif

#if USE_RELIC_SHA1_UCHAR
#define EXPAND_RELIC_SHA1_UCHAR_FN void (*copyUchar) (_RELIC_SHA1_STRUCT_ *, unsigned char *);
#else
#define EXPAND_RELIC_SHA1_UCHAR_FN
#endif

typedef struct _RELIC_SHA1_STRUCT_ {
	void (*hash) (_RELIC_SHA1_STRUCT_ *, unsigned char *, uint8_t *);
	EXPAND_RELIC_SHA1_UINT32_FN
	EXPAND_RELIC_SHA1_UINT16_FN
	EXPAND_RELIC_SHA1_UINT8_FN
	EXPAND_RELIC_SHA1_UCHAR_FN

} sha1;

#if USE_RELIC_SHA1_UINT32
uint32_t * copyUint32();
#endif

#if USE_RELIC_SHA1_UINT16
uint16_t * copyUint16();
#endif

#if USE_RELIC_SHA1_UINT8
uint8_t * copyUint8();
#endif

#if USE_RELIC_SHA1_UCHAR
unsigned char * copyUchar();
#endif

#endif // __RELIC_SHA1_H__