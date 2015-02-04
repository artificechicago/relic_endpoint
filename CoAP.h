#ifndef __RELIC_COAP_MSG_H__
#define __RELIC_COAP_MSG_H__

#include <stdint.h>
#include "error.h"

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
	int codeDetail;
} coapCode;

typedef struct _COAP_MSG_ {
	unsigned char header;
	unsigned char code;
	uint16_t msgID;
	unsigned char token[8];
	unsigned char body[52];
	uint8_t msgSize;
} coapMsg;

// Functions are separated into a vtable/namespace

typedef struct _COAP_MSG_VTABLE_ {
	coapCodeClass (*codeClass) (struct _COAP_MSG_ *);
	int (*codeDetail) (struct _COAP_MSG_ *);
} CoAP;

// Exposed functions

void parseCoapMsg(coapMsg *, char *, uint8_t);

#endif