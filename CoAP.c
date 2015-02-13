#include "CoAP.h"
#include <string.h>

static const unsigned char H_VERSION_MASK = 0xC0;   // mask for 11000000
static const unsigned char H_TYPE_MASK = 0x30;      // mask for 00110000
static const unsigned char H_TKL_MASK = 0x0F;       // mask for 00001111

static const unsigned char CODE_CLASS_MASK = 0xe0;  //mask for 11100000
static const unsigned char CODE_DETAIL_MASK = 0x1f; //mask for 00011111

static const uint8_t MAX_TOKEN_LENGTH = 8;  // RFC 7252 defines lengths 9-15 as an error
static const uint8_t COAP_HEADER_SIZE = 4;

static RNG rng = NULL;

void initCoAP(RNG r) {
	rng = r;
}

// CoAP Message Getters

// Getters for the packed binary header

unsigned char CoAP_getVersion (coapMsg * msg)
{
	return msg->header & H_VERSION_MASK >> 6;
}

coapType CoAP_getType (coapMsg * msg)
{
	return (coapType)((msg->header & H_TYPE_MASK) >> 4);
}

uint8_t CoAP_getTokenLength (coapMsg * msg)
{
	return (uint8_t)(msg->header & H_TKL_MASK);
}

// Getters for the packed binary code

coapCodeClass CoAP_getCodeClass (coapMsg * msg)
{
	coapCodeClass codeClass;
	uint8_t bitValue = (msg->code & CODE_CLASS_MASK) >> 5;

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

uint8_t CoAP_getCodeDetail (coapMsg * msg)
{
	// Valid code detail values and semantics depend
	// on the code class value the detail is coupled to,
	// so fuck it, this is just a number.

	uint8_t detail = msg->code & CODE_DETAIL_MASK;
	return detail;
}

coapCode CoAP_getCode (coapMsg * msg)
{
	coapCode code;
	code.codeClass = CoAP_getCodeClass(msg);
	code.codeDetail = CoAP_getCodeDetail(msg);
	return code;
}

// Getters for the other, more difficult, parts

/* Options not implemented for initial beta

unsigned char _GET_COAP_MSG_OPTIONS_ (coapMsg * self)
{
	#error --- Get Option Not Implemented ---
}

*/

uint8_t CoAP_bufferPayload (coapMsg * msg, payloadBuffer * pb)
{
	uint8_t payloadSize = msg->msgSize - (COAP_HEADER_SIZE + CoAP_getTokenLength(msg));
	uint8_t optValue = 0x00;
	uint8_t optLength = 0x00;
	unsigned char * loc = msg->body;
	do {
		optValue += (*loc) >> 4;
		if (optValue == 0x0D || optValue == 0x0E) {
			return 0; // This is an error for now.
		} else if (optValue != 0x0F) {
			optLength = (*loc) & 0x0F;
			loc += optLength;
			payloadSize -= optLength;
		}
		loc++;
		payloadSize--;
	} while ((optValue != 0x0F) && (loc < (msg + msg->msgSize)));

	if (optLength == 0x0F) {
		memcpy(pb->buffer, loc, payloadSize);
	}
	return payloadSize;
}

// CoAP Message Header Setters
// Version is always 1, so there's no setter for now.

void CoAP_setType (coapMsg * msg, coapType t)
{
	unsigned char typeBytes = ((unsigned char)t) << 4;
	msg->header &= ~H_TYPE_MASK;
	msg->header += typeBytes;
}

void CoAP_setMessageID (coapMsg * msg, uint16_t msgId)
{
	msg->msgID = msgId;
}

voidError CoAP_setTokenLength (coapMsg * msg, 
												uint8_t tokenLength)
{
	msg->header &= ~H_TKL_MASK;
	msg->header += (tokenLength < MAX_TOKEN_LENGTH)
					? tokenLength : MAX_TOKEN_LENGTH;
	if (tokenLength > MAX_TOKEN_LENGTH) {
		return true;
	}
	return false;
}

void CoAP_setToken (coapMsg * msg, unsigned char * token)
{
	memcpy(msg->token, token, 8);
}

void CoAP_genToken (coapMsg * msg)
{
	uint8_t i;
	for (i = 0; i < 8; i++) {
		msg->token[i] = (unsigned char)(*rng)();
	}
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

void CoAP_packMessage(packedMsg * dest, coapMsg * msg) {
	memcpy(dest, msg, COAP_HEADER_SIZE + CoAP_getTokenLength(msg));
	memcpy(dest + COAP_HEADER_SIZE + CoAP_getTokenLength(msg), msg->body, msg->msgSize - (COAP_HEADER_SIZE + CoAP_getTokenLength(msg)));
	dest->size = msg->msgSize;
}

void CoAP_unpackMessage(coapMsg * dest, packedMsg * source) {
	if (source->size >=4) {
		dest->header = source->msg[0];
		dest->code = source->msg[1];
		dest->msgID = (source->msg[2] << 8) + source->msg[3];
		memcpy(dest->token, source->msg + COAP_HEADER_SIZE, dest->header & H_TKL_MASK);
		memcpy(dest->body, source->msg + COAP_HEADER_SIZE + (dest->header & H_TKL_MASK), source->size - (COAP_HEADER_SIZE + (dest->header & H_TKL_MASK)));
		dest->msgSize = source->size;
	}
}


