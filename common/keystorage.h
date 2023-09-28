/**********************************************************************************************************************
 * \file keystorage.h
 * \copyright Copyright (C) Infineon Technologies AG 2019
 * 
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of 
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 * 
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and 
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all 
 * derivative works of the Software, unless such copies or derivative works are solely in the form of 
 * machine-executable object code generated by a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *********************************************************************************************************************/

#ifndef KEYSTORAGE_H
#define KEYSTORAGE_H

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
/* Macros used to define the Public key */    
#define CY_SI_PUBLIC_KEY_RSA_2048       (0UL)                   /* RSASSA-PKCS1-v1_5-2048 signature scheme */
#define CY_SI_PUBLIC_KEY_RSA_1024       (1UL)                   /* RSASSA-PKCS1-v1_5-1024 signature scheme */
#define CY_SI_PUBLIC_KEY_STRUCT_OFFSET  (8UL)                   /* Offset to public key struct in number of bytes */
#define CY_SI_PUBLIC_KEY_MODULOLENGTH   (256UL)                 /* Modulus length of the RSA key */
#define CY_SI_PUBLIC_KEY_EXPLENGTH      (32UL)                  /* Exponent length of the RSA key */
#define CY_SI_PUBLIC_KEY_SIZEOF_BYTE    (8UL)                   /* Size of Byte in number of bits */

/* For verification of digital-signed application */
#define CY_FB_VERIFYAPP_ADDR            (0x17002040)
#define CY_PUBLIC_KEY_ADDR              (0x17006408)

/*********************************************************************************************************************/
/*-------------------------------------------------Data Structures---------------------------------------------------*/
/*********************************************************************************************************************/
/** Public key definition structure as expected by the Crypto driver */
typedef struct
{
    uint32_t moduloAddr;                                        /* Address of the public key modulus */
    uint32_t moduloSize;                                        /* Size (bits) of the modulus part of the public key */
    uint32_t expAddr;                                           /* Address of the public key exponent */
    uint32_t expSize;                                           /* Size (bits) of the exponent part of the public key */
    uint32_t barrettAddr;                                       /* Address of the Barret coefficient */
    uint32_t inverseModuloAddr;                                 /* Address of the binary inverse modulo */
    uint32_t rBarAddr;                                          /* Address of the (2^moduloLength mod modulo) */
} cy_si_stc_crypto_public_key_t;

/** Public key structure */
typedef struct
{
    uint32_t objSize;                                           /* Public key Object size */
    uint32_t signatureScheme;                                   /* Signature scheme */
    cy_si_stc_crypto_public_key_t publicKeyStruct;              /* Public key definition struct */
    uint8_t  moduloData[CY_SI_PUBLIC_KEY_MODULOLENGTH];         /* Modulo data */
    uint8_t  expData[CY_SI_PUBLIC_KEY_EXPLENGTH];               /* Exponent data */
    uint8_t  barrettData[CY_SI_PUBLIC_KEY_MODULOLENGTH + 4UL];  /* Barret coefficient data */
    uint8_t  inverseModuloData[CY_SI_PUBLIC_KEY_MODULOLENGTH];  /* Binary inverse modulo data */
    uint8_t  rBarData[CY_SI_PUBLIC_KEY_MODULOLENGTH];           /* 2^moduloLength mod modulo data */
} cy_si_stc_public_key_t;

#if defined(__cplusplus)
}
#endif

#endif /* KEYSTORAGE_H */

/* [] END OF FILE */
