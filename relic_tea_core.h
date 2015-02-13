#ifndef __RELIC_TEA_CORE_H__
#define __RELIC_TEA_CORE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif
void encrypt(uint32_t *, uint32_t *);
void decrypt(uint32_t *, uint32_t *);

#ifdef __cplusplus
}
#endif

#endif