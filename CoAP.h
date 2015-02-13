#ifndef __RELIC_COAP_MSG_H__
#define __RELIC_COAP_MSG_H__

#include <stdint.h>
#include "error.h"
#include "relic_rng.h"

typedef uint8_t (*RNG)();

// Basic CoAP type definitions
typedef enum _COAP_MSG_TYPES_ {
	CON,
	NON,
	ACK,
	RST,
} coapType;

typedef enum _COAP_MSG_CODE_CLASS_ {
	EMPTY = 0x00,
	OK  = 0x02,
	CLIENT_ERROR = 0x04,
	SERVER_ERROR = 0x05,
	UNK = 0x07
} coapCodeClass;

typedef struct _COAP_MSG_CODE_ {
	coapCodeClass codeClass;
	uint8_t codeDetail;
} coapCode;

typedef struct _COAP_MSG_ {
	unsigned char header;
	unsigned char code;
	uint16_t msgID;
	unsigned char token[8];
	unsigned char body[34];
	uint8_t msgSize;
} coapMsg;

typedef struct _PACKED_COAP_MSG_ {
	unsigned char msg[46];
	uint8_t size;
} packedMsg;

typedef struct _PAYLOAD_BUFFER_ {
	unsigned char buffer[33];
	uint8_t size;
} payloadBuffer;

#ifdef __cplusplus
extern "C"{
#endif
	
void initCoAP(RNG rng);
coapCodeClass CoAP_getCodeClass(struct _COAP_MSG_ *);
uint8_t CoAP_getCodeDetail(struct _COAP_MSG_ *);

void CoAP_packMessage(packedMsg *, coapMsg *);
voidError parsePacketToCoAPMsg(struct _COAP_MSG_ *, unsigned char *, size_t);


void parseCoapMsg(coapMsg *, char *, uint8_t);
#ifdef __cplusplus
}
#endif // extern "C"
#endif