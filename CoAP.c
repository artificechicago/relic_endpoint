#include "CoAP.h"
#include <string.h>

static const unsigned char H_VERSION_MASK = 0xC0;   // mask for 11000000
static const unsigned char H_TYPE_MASK = 0x30;      // mask for 00110000
static const unsigned char H_TKL_MASK = 0x0F;       // mask for 00001111

static const unsigned char CODE_CLASS_MASK = 0xe0;  //mask for 11100000
static const unsigned char CODE_DETAIL_MASK = 0x1f; //mask for 00011111

static const uint8_t MAX_TOKEN_LENGTH = 8;  // RFC 7252 defines lengths 9-15 as an error
static const uint8_t COAP_HEADER_SIZE = 4;

// CoAP Message Getters

// Getters for the packed binary header

unsigned char _GET_COAP_MSG_HEADER_VERSION (coapMsg * self)
{
	return self->header & H_VERSION_MASK >> 6;
}

coapType _GET_COAP_MSG_HEADER_TYPE_ (coapMsg * self)
{
	return (coapType)((self->header & H_TYPE_MASK) >> 4);
}

unsigned char _GET_COAP_MSG_HEADER_TOKEN_LENGTH_ (coapMsg * self)
{
	return self->header & H_TKL_MASK;
}

// Getters for the packed binary code

coapCodeClass _GET_COAP_MSG_CODE_CLASS_ (coapMsg * self)
{
	coapCodeClass codeClass;
	uint8_t bitValue = (self->code & CODE_CLASS_MASK) >> 5;

	switch (bitValue) {
		case 0x00:
			codeClass = EMPTY;
			break;
		case 0x02:
			codeClass = OK;
			break;
		case 0x04:
			codeClass = CLIENT_ERROR;
			break;
		case 0x05:
			codeClass = SERVER_ERROR;
			break;
		default:
			codeClass = UNK;
	}

	return codeClass;
}

int _GET_COAP_MSG_CODE_DETAIL_ (coapMsg * self)
{
	// Valid code detail values and semantics depend
	// on the code class value the detail is coupled to,
	// so fuck it, this is just a number.

	int detail = self->code & CODE_DETAIL_MASK;
	return detail;
}

coapCode _GET_COAP_MSG_CODE_ (coapMsg * self)
{
	coapCode code;
	code.codeClass = self->codeClass(self);
	code.codeDetail = self->codeDetail(self);
	return code;
}

// Getters for the other, more difficult, parts

unsigned char _GET_COAP_MSG_OPTIONS_ (coapMsg * self)
{
	#error --- Get Option Not Implemented ---
}

unsigned char _GET_COAP_MSG_PAYLOAD_ (coapMsg * self)
{
	#error --- Get Payload Not Implemented ---
}

// CoAP Message Header Setters
// Version is always 1, so there's no setter for now.

void _SET_COAP_MSG_HEADER_TYPE_ (coapMsg * self, coapType t)
{
	unsigned char typeBytes = ((unsigned char)t) << 4;
	self->header &= ~H_TYPE_MASK;
	self->header += typeBytes;
}

void _SET_COAP_MSG_MESSAGE_ID_ (coapMsg * self, uint16_t msgId)
{
	self->msgID = msgId;
}

voidError _SET_COAP_MSG_HEADER_TOKEN_LENGTH_ (coapMsg * self, 
												uint8_t tokenLength)
{
	self->header &= ~H_TKL_MASK;
	self->header += (tokenLength < MAX_TOKEN_LENGTH)
					? tokenLength : MAX_TOKEN_LENGTH;
	if (tokenLength > MAX_TOKEN_LENGTH) {
		return true;
	}
	return false;
}

void _SET_COAP_MSG_TOKEN_ (coapMsg * self, unsigned char token[8])
{
	memcpy(self->token, token, 8);
}

void _GENERATE_COAP_MSG_TOKEN_ (coapMsg * self)
{
	#error --Token Generation Not Implemented --
}

// Parser for turning packets into coapMsg structs

voidError parsePacketToCOAPMsg (coapMsg * dest, unsigned char * source, size_t packetSize)
{
	uint8_t tokenLength;

	// check for the existence of header information and copy it over
	if (packetSize > sizeof(coapMsg) - 1) return true; // error if packet is too large
	if (packetSize < COAP_HEADER_SIZE) return true; // if there isn't at least a header, return an error
	dest->header = source[0];
	dest->code = source[1];
	dest->msgID = ((uint16_t)source[2] << 8) + (uint16_t)source[3];

	// determine token size and write token to struct
	tokenLength = dest->header & H_TKL_MASK;
	if (packetSize < 4 + tokenLength) return true; // check that the token exists
	memcpy(&(dest->token), source + 4, tokenLength);

	//copy over the body of the message
	if (packetSize > 4 + tokenLength) {
		memcpy(dest->body, 
			source + COAP_HEADER_SIZE + tokenLength,
			packetSize - (COAP_HEADER_SIZE + tokenLength));
	}
	dest->msgSize = packetSize;
}

/*

voidError writeCoapMsgPayload(coapMsg * msg, unsigned char * payload, uint8_t p_size)
{
	memcpy(msg->payload, payload, p_size);
	return false;
}

void genCoapMsgToken(coapMsg * msg)
{
	return;
}

// Parser function for incoming messages

void parseOption(coapMsg * msg, uint8_t loc)
{
	return;
}

coapMsgCode parseCode(uint8_t code_byte)
{
	coapMsgCode code;
	code.class = code_byte >> 5;
	code.detail = (code_byte << 3) >> 3;

	return code;
}

void parseCoapMsg(coapMsg * dest, char * source, uint8_t size)
{
	msg->header->version = *source >> 6;
	msg->header->type = (*source << 2) >> 6;
	msg->header->tkl = (*source << 4) >> 4;
	msg->header->code = parseCode(*(source + 1));
	msg->header->msgID = ;
}

*/