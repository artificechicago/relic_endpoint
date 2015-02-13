#ifndef	__RELIC_VNLN_H__
#define __RELIC_VNLN_H__

#include <Arduino.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif
void vonNeumannNoiseExtractor(uint8_t *, int, int);

#ifdef __cplusplus
}
#endif

#endif