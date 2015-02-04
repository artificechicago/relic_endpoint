#ifndef __RELIC_RNG_H__
#define __RELIC_RNG_H__

#include <stdint.h>
#include <string.h>
#include <Arduino.h>
#include "relic_tea_core.h"

void initializeRNG(int);
uint8_t rand_val();

#endif