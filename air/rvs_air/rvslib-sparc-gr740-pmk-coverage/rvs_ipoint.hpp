
/* RVS - C Map Instrumentation library */
/* RVS 3.14 */

/*=============================================================================
* Rapita Systems Ltd. - Rapita Verification Suite (RVS) Source Code License
*
* DOC13027 V1 (2014/01/28)
*
* (c) 2004-2020 Rapita Systems Ltd. All rights reserved
* -----------------------------------------------------------------------------
*
* Subject to the terms and conditions of the RVS License Agreement, Rapita
* Systems Ltd. grants the Licensee a non-exclusive, non-transferable license to
* use the source code contained in this file (the "Licensed Software").
*
* The Licensee may use the Licensed Software for its own use, and may
* translate or modify it or incorporate it into other software. The Licensee 
* may not, however, alter or remove the copyright notice or the text of the
* source code license.
*
* The Licensee may not, however, transfer or sublicense the Licensed Software
* to any third party, in whole or in part, in any form, whether modified or
* unmodified without the prior written consent of Rapita Systems Ltd.
*
* The original and any copies of the Licensed Software, made by the Licensee,
* including translations, compilations, partial copies, modifications,
* and updates, are the property of Rapita Systems Ltd.
*
* The Licensee recognizes that Rapita Systems Ltd. regards the Licensed
* Software as its proprietary information and as confidential trade secrets.
*
* The Licensee agrees not to provide or to otherwise make available in any
* form the Licensed Software, or any portion thereof, to any person other
* than employees of the Licensee without the prior written consent of
* Rapita Systems Ltd.
*
* The Licensee further agrees to treat the Licensed Programs with at least
* the same degree of care with which the Licensee treats its own confidential
* information and in no event with less care than is reasonably required to
* protect the confidentiality of the Licensed Software.
*
* -----------------------------------------------------------------------------
*
* Registered in the UK, number 5011090.
* Registered address:
*   Atlas House, Link Business Park
*   Osbaldwick Link Road
*   York
*   YO10 3JB
*   UK
* Phone: +44 (0) 1904 413 945
* 
* email: support@rapitasystems.com
* http://www.rapitasystems.com/
*
*=============================================================================
*/

#ifndef __RVS_IPOINT_H__
#define __RVS_IPOINT_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __RVS_TYPE_DEFINITIONS__
#define __RVS_TYPE_DEFINITIONS__

/* If changing any of the following types, remember they are declared in three locations: */
/* rvs_ipoint.h; rvs_ipoint.hpp; rvs_types.h */

typedef unsigned char      t_rvs_uint8;
typedef t_rvs_uint8      * t_rvs_uint8_ptr;

typedef unsigned short     t_rvs_uint16;
typedef t_rvs_uint16     * t_rvs_uint16_ptr;

typedef unsigned int       t_rvs_uint32;
typedef t_rvs_uint32     * t_rvs_uint32_ptr;

typedef unsigned long long t_rvs_uint64;
typedef signed long long   t_rvs_int64;

typedef unsigned char      t_rvs_boolean;
/* size_t is a bit complicated, so do our best to work out the correct definition */
#if defined(__SIZE_TYPE__)
typedef __SIZE_TYPE__           t_rvs_size_t;
#elif defined(_WIN64) || defined(_M_X64)
typedef unsigned long long int  t_rvs_size_t;
#elif defined(__x86_64__) || defined(__aarch64__)
typedef unsigned long int       t_rvs_size_t;
#else
typedef unsigned int            t_rvs_size_t;
#endif

typedef t_rvs_uint32       t_rvs_ipoint_id;

typedef t_rvs_uint16       t_rvs_mcdc_cond_idx;
typedef t_rvs_uint32       t_rvs_mcdc_bool;

typedef t_rvs_uint32 t_rvs_rapicover_data;
typedef t_rvs_rapicover_data * t_rvs_rapicover_data_ptr;

typedef t_rvs_uint32 t_rvs_rapitest_data;
typedef t_rvs_rapitest_data *t_rvs_rapitest_data_ptr;

/* Number of 8-bit bytes per byte access (usually 1:1) */
#define RVS_BYTESZ 1

#define RVS_FALSE 0
#define RVS_TRUE (!RVS_FALSE)
#ifdef __cplusplus
#define RVS_NULL 0
#else
#define RVS_NULL ((void *)0)
#endif

#define RVS_UINT32_MAX 0xFFFFFFFF
typedef struct {t_rvs_uint32 T[2];} t_rvs_2_uint32;
typedef struct {t_rvs_uint32 T[4];} t_rvs_4_uint32;

/* These are the largest integers RVS can handle in calculations */
typedef t_rvs_uint64 t_rvs_ulong;
typedef t_rvs_int64  t_rvs_slong;

/* This can be anything up to and including 64 bits */
typedef t_rvs_uint64 t_rvs_test_id;

#endif

/* Macro to indicate that RVS is on */
#define RVS

void RVS_Init(void);
void RVS_Output(void);

void RVS_Begin_Test(t_rvs_test_id test_id, const char *test_description);
void RVS_Next_Test(t_rvs_test_id test_id, const char *test_description);
void RVS_End_Test(void);

/* Coverage map */
/* #define RVS_SEGMENTED */

#ifdef RVS_SEGMENTED
    unsigned short* rvs_map_rapicover_data(t_rvs_uint32 idx);
    #define MAP_ACCESSOR(idx) *rvs_map_rapicover_data(idx)
#else
    extern t_rvs_uint8_ptr rvs_rapicover_data;
    #define MAP_ACCESSOR(idx) rvs_rapicover_data[idx]
#endif

/* Instrumentation macros and functions */
#ifndef RVS_Ipoint
#if RVS_BYTESZ == 1
#define RVS_Ipoint(_I) (MAP_ACCESSOR(_I) = 1)
#else
#define RVS_Ipoint(_I) (MAP_ACCESSOR((_I)/(RVS_BYTESZ)) |= (1 << (((_I)%RVS_BYTESZ)<<3)))
#endif

#define RVS_Ipoint_bp(_B,_O) (MAP_ACCESSOR(_B) |= (1<<(_O)))

#endif
#define RVS_I(_I)     RVS_Ipoint((_I))

#define RVS_I_bp(_B,_O)     RVS_Ipoint_bp((_B),(_O))

t_rvs_mcdc_bool RVS_CM(t_rvs_uint32_ptr k, t_rvs_mcdc_cond_idx idx, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DM(t_rvs_ipoint_id id, t_rvs_uint32_ptr k, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DML(t_rvs_ipoint_id id, t_rvs_uint32_ptr k, t_rvs_mcdc_bool b);

typedef unsigned char t_rvs_key8;
t_rvs_mcdc_bool RVS_CM8(t_rvs_key8 *k, t_rvs_mcdc_cond_idx idx, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DM8(t_rvs_ipoint_id id, t_rvs_key8 *k, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DML8(t_rvs_ipoint_id id, t_rvs_key8 *k, t_rvs_mcdc_bool b);
#define RVS_KEY8_0 0
#define RVS_RESET_KEY8(_K) _K=0

typedef unsigned short t_rvs_key16;
t_rvs_mcdc_bool RVS_CM16(t_rvs_key16 *k, t_rvs_mcdc_cond_idx idx, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DML16(t_rvs_ipoint_id id, t_rvs_key16 *k, t_rvs_mcdc_bool b);
#define RVS_KEY16_0 0
#define RVS_RESET_KEY16(_K) _K=0

typedef unsigned long t_rvs_key32;
t_rvs_mcdc_bool RVS_CM32(t_rvs_key32 *k, t_rvs_mcdc_cond_idx idx, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DML32(t_rvs_ipoint_id id, t_rvs_key32 *k, t_rvs_mcdc_bool b);
#define RVS_KEY32_0 0
#define RVS_RESET_KEY32(_K) _K=0

typedef t_rvs_uint64 t_rvs_key64;
t_rvs_mcdc_bool RVS_CM64(t_rvs_key64 *k, t_rvs_mcdc_cond_idx idx, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DML64(t_rvs_ipoint_id id, t_rvs_key64 *k, t_rvs_mcdc_bool b);
#define RVS_KEY64_0 0
#define RVS_RESET_KEY64(_K) _K=0

typedef struct t_rvs_key128 { unsigned long d[4]; } t_rvs_key128;
t_rvs_mcdc_bool RVS_CM128(t_rvs_key128 *k, t_rvs_mcdc_cond_idx idx, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DML128(t_rvs_ipoint_id id, t_rvs_key128 *k, t_rvs_mcdc_bool b);
#define RVS_KEY128_0 {0,0,0,0}
#define RVS_RESET_KEY128(_K) (_K.d[0]=0,_K.d[1]=0,_K.d[2]=0,_K.d[3]=0)

typedef struct t_rvs_key256 { unsigned long d[8]; } t_rvs_key256;
t_rvs_mcdc_bool RVS_CM256(t_rvs_key256 *k, t_rvs_mcdc_cond_idx idx, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DML256(t_rvs_ipoint_id id, t_rvs_key256 *k, t_rvs_mcdc_bool b);
#define RVS_KEY256_0 {0,0,0,0, 0,0,0,0}
#define RVS_RESET_KEY256(_K) (_K.d[0]=0,_K.d[1]=0,_K.d[2]=0,_K.d[3]=0,_K.d[4]=0,_K.d[5]=0,_K.d[6]=0,_K.d[7]=0)

typedef struct t_rvs_key512 { unsigned long d[16]; } t_rvs_key512;
t_rvs_mcdc_bool RVS_CM512(t_rvs_key512 *k, t_rvs_mcdc_cond_idx idx, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DML512(t_rvs_ipoint_id id, t_rvs_key512 *k, t_rvs_mcdc_bool b);
#define RVS_KEY512_0 {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}
#define RVS_RESET_KEY512(_K) (_K.d[0]=0,_K.d[1]=0,_K.d[2]=0,_K.d[3]=0,_K.d[4]=0,_K.d[5]=0,_K.d[6]=0,_K.d[7]=0,_K.d[8]=0,_K.d[9]=0,_K.d[10]=0,_K.d[11]=0,_K.d[12]=0,_K.d[13]=0,_K.d[14]=0,_K.d[15]=0)

typedef struct t_rvs_key1024 { unsigned long d[32]; } t_rvs_key1024;
t_rvs_mcdc_bool RVS_CM1024(t_rvs_key1024 *k, t_rvs_mcdc_cond_idx idx, t_rvs_mcdc_bool b);
t_rvs_mcdc_bool RVS_DML1024(t_rvs_ipoint_id id, t_rvs_key1024 *k, t_rvs_mcdc_bool b);
#define RVS_KEY1024_0 {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}
#define RVS_RESET_KEY1024(_K) (_K.d[0]=0,_K.d[1]=0,_K.d[2]=0,_K.d[3]=0,_K.d[4]=0,_K.d[5]=0,_K.d[6]=0,_K.d[7]=0,_K.d[8]=0,_K.d[9]=0,_K.d[10]=0,_K.d[11]=0,_K.d[12]=0,_K.d[13]=0,_K.d[14]=0,_K.d[15]=0,_K.d[16]=0,_K.d[17]=0,_K.d[18]=0,_K.d[19]=0,_K.d[20]=0,_K.d[21]=0,_K.d[22]=0,_K.d[23]=0,_K.d[24]=0,_K.d[25]=0,_K.d[26]=0,_K.d[27]=0,_K.d[28]=0,_K.d[29]=0,_K.d[30]=0,_K.d[31]=0)

#define RVS_E(_I,_E)  ((void)RVS_I((_I)),(_E))
#define RVS_C(_I,_C)  (RVS_E((_I),0)?(_C):(_C))
#define RVS_T(_I)     RVS_E((_I),1)
#define RVS_F(_I)     RVS_E((_I),0)

#define RVS_E_bp(_B,_O,_E)  ((void)RVS_I_bp((_B),(_O)),(_E))
#define RVS_C_bp(_B,_O,_C)  (RVS_E_bp((_B),(_O),0)?(_C):(_C))
#define RVS_T_bp(_B,_O)     RVS_E_bp((_B),(_O),1)
#define RVS_F_bp(_B,_O)     RVS_E_bp((_B),(_O),0)

#ifdef __cplusplus
}
#endif

#endif
