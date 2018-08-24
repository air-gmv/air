/* This file was generated automatically: DO NOT MODIFY IT ! */

/* Declaration of the functions that have to be provided by the user */

#ifndef __USER_CODE_H_gr740_candriver__
#define __USER_CODE_H_gr740_candriver__

#include "C_ASN1_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

void gr740_candriver_startup();

void gr740_candriver_PI_update();

void gr740_candriver_PI_commands(const asn1SccT_Int32 *);

extern void gr740_candriver_RI_samples(const asn1SccT_Int32 *);

#ifdef __cplusplus
}
#endif


#endif
