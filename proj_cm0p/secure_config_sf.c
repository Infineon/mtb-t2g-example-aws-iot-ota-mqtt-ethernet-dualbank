/**********************************************************************************************************************
 * \file secure_config_sf.c
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
/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include <stddef.h>
#include "cmsis_compiler.h"
#include "cy_utils.h"
#include "secure_config.h"
#include "keystorage.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
/* CM0+ ARM CPUID[15:4] Reg shifted to [31:20] */
#define CY_SI_CPUID           (0xC6000000UL)

/* Index ID of the CM0+ core */
#define CY_SI_CORE_IDX        (0UL)

/* Flashboot flags */
#define CY_SI_FLASHBOOT_FLAGS ((CY_SI_FLASHBOOT_CLK_100MHZ << CY_SI_TOC_FLAGS_CLOCKS_POS) \
                                | (CY_SI_FLASHBOOT_WAIT_20MS << CY_SI_TOC_FLAGS_DELAY_POS) \
                                | (CY_SI_FLASHBOOT_SWJ_ENABLE << CY_SI_TOC_FLAGS_SWJEN_POS) \
                                | (CY_SI_FLASHBOOT_VALIDATE_DISABLE << CY_SI_TOC_FLAGS_APP_VERIFY_POS) \
                                | (CY_SI_FLASHBOOT_FBLOADER_DISABLE << CY_SI_TOC_FLAGS_FBLOADER_ENABLE_POS))

/* The application size is set 2KB (complete SFlash User data region), can be reduced if applicable */
#define CY_SFLASH_SI_SIZE     (0x00000800UL)

/* CM0+ VT Offset (refers linker symbol) */
#define CY_SF_SI_VT_OFFSET    ((uint32_t)__sflash_si_vector_offset_linker_symbol) \
                               - offsetof(cy_stc_si_appheader_t, core0Vt)

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
/* linker symbols */
extern const char __sflash_si_vector_offset_linker_symbol[];
extern const char __cy_sf_app_header_start__[];

/* TOC2 */
CY_SECTION(".cy_toc_part2") __USED static const cy_stc_si_toc_t cy_toc2 =
{
    .objSize         = CY_SI_TOC2_OBJECTSIZE,               /* Offset+0x00: Object Size (Bytes) excluding CRC */
    .magicNum        = CY_SI_TOC2_MAGICNUMBER,              /* Offset+0x04: TOC2 ID (magic number) */
    .smifCfgAddr     = 0UL,                                 /* Offset+0x08: SMIF config list pointer */
    .cm0pappAddr1    = (uint32_t)__cy_sf_app_header_start__,/* Offset+0x0C: App1 (SFlash User App Object) addr */
    .cm0pappFormat1  = CY_SI_APP_FORMAT_CYPRESS,            /* Offset+0x10: App1 Format */
    .cm0pappAddr2    = 0UL,                                 /* Offset+0x14: App2 - Not done */
    .cm0pappFormat2  = CY_SI_APP_FORMAT_CYPRESS,            /* Offset+0x18: App2 Format */
    .cm4_71appAddr1  = CY_SI_CM471_1stAPP_FLASH_BEGIN,      /* Offset+0x1C: App3 (CM4/CM7_1 1st User App Object) addr */
    .cm4_71appAddr2  = CY_SI_CM471_2ndAPP_FLASH_BEGIN,      /* Offset+0x20: App4 (CM4/CM7_1 2nd User App Object) addr */
    .cm72appAddr1    = CY_SI_CM72_1stAPP_FLASH_BEGIN,       /* Offset+0x24: App5 (CM7_2 1st User App Object) addr */
    .cm72appAddr2    = CY_SI_CM72_2ndAPP_FLASH_BEGIN,       /* Offset+0x28: App6 (CM7_2 1st User App Object) addr */
    .reserved1       = {0UL},                               /* Offset+0x2C-0xFF: Reserved area 212Bytes */
    .shashObj        = 3UL,                                 /* Offset+0x100: Number of verified additional objects (S-HASH)*/
    .sigKeyAddr      = CY_SI_PUBLIC_KEY,                    /* Offset+0x104: Addr of signature verification key */
    .swpuAddr        = CY_SI_SWPU_BEGIN,                    /* Offset+0x108: Addr of SWPU Objects */
    .toc2Addr        = CY_SI_TOC2_BEGIN,                    /* Offset+0x10C: Addr of TOC2 */
    .addObj          = {0UL},                               /* Offset+0x110-0x1F4: Reserved area 232Bytes */
    .tocFlags        = CY_SI_FLASHBOOT_FLAGS,               /* Flashboot flags stored in TOC2 */
    .crc             = 0UL,                                 /* Offset+0x1FC: Reserved area 1Byte */
};

/* Public key */
CY_SECTION(".cy_sflash_public_key") __USED const cy_si_stc_public_key_t cy_publicKey =
{
    .objSize = sizeof(cy_si_stc_public_key_t),
    .signatureScheme = CY_SI_PUBLIC_KEY_RSA_2048,
    .publicKeyStruct =
    {
        .moduloAddr         = CY_SI_PUBLIC_KEY + offsetof(cy_si_stc_public_key_t, moduloData),
        .moduloSize         = CY_SI_PUBLIC_KEY_SIZEOF_BYTE * CY_SI_PUBLIC_KEY_MODULOLENGTH,
        .expAddr            = CY_SI_PUBLIC_KEY + offsetof(cy_si_stc_public_key_t, expData),
        .expSize            = CY_SI_PUBLIC_KEY_SIZEOF_BYTE * CY_SI_PUBLIC_KEY_EXPLENGTH,
        .barrettAddr        = CY_SI_PUBLIC_KEY + offsetof(cy_si_stc_public_key_t, barrettData),
        .inverseModuloAddr  = CY_SI_PUBLIC_KEY + offsetof(cy_si_stc_public_key_t, inverseModuloData),
        .rBarAddr           = CY_SI_PUBLIC_KEY + offsetof(cy_si_stc_public_key_t, rBarData),
    },
    /* using key-pair @ "<project_root>\utils\keys_generated" */
    .moduloData =
    {
        0x21u, 0xE0u, 0x32u, 0x21u, 0x27u, 0x5Au, 0x01u, 0x75u,
        0xE1u, 0x41u, 0x53u, 0xDAu, 0xA6u, 0x8Cu, 0x83u, 0xEAu,
        0x90u, 0xCBu, 0xCCu, 0x89u, 0xECu, 0x87u, 0x52u, 0xBFu,
        0xB6u, 0xA3u, 0xB8u, 0xD3u, 0x0Fu, 0xDBu, 0xC8u, 0xEEu,
        0x5Fu, 0x25u, 0x7Cu, 0x8Fu, 0xFEu, 0xF7u, 0x79u, 0xE1u,
        0x6Bu, 0xC4u, 0x83u, 0x46u, 0x82u, 0xACu, 0x3Du, 0x16u,
        0x79u, 0x59u, 0x1Fu, 0x24u, 0x59u, 0x43u, 0xF7u, 0x29u,
        0x99u, 0x04u, 0xABu, 0xD9u, 0x58u, 0x58u, 0xFAu, 0xBEu,
        0xE5u, 0x2Fu, 0x67u, 0x5Au, 0x19u, 0x0Fu, 0x9Au, 0x99u,
        0xD4u, 0x74u, 0x9Fu, 0x8Cu, 0xA3u, 0x44u, 0x8Fu, 0x61u,
        0xD5u, 0x9Fu, 0x76u, 0x8Cu, 0x51u, 0xBBu, 0xD6u, 0xCEu,
        0x3Cu, 0x11u, 0x3Au, 0xBDu, 0x9Eu, 0xF0u, 0x88u, 0xBBu,
        0xB2u, 0x54u, 0xB9u, 0xDAu, 0x46u, 0x22u, 0x62u, 0xFDu,
        0xF3u, 0x05u, 0x99u, 0x2Au, 0x93u, 0x8Bu, 0x31u, 0x05u,
        0xA6u, 0xFBu, 0x35u, 0x49u, 0x54u, 0xEEu, 0x0Bu, 0xDDu,
        0x18u, 0x0Au, 0x5Bu, 0x01u, 0x09u, 0x5Cu, 0xDAu, 0x4Du,
        0x5Eu, 0x1Cu, 0x21u, 0x0Au, 0xD6u, 0xFAu, 0x1Fu, 0x99u,
        0xA2u, 0x4Du, 0xEEu, 0x2Eu, 0xEDu, 0xEDu, 0x65u, 0xCBu,
        0x61u, 0xC3u, 0x34u, 0xAFu, 0x7Au, 0x27u, 0xCDu, 0xD2u,
        0x28u, 0x58u, 0x34u, 0x1Fu, 0x94u, 0x15u, 0xE7u, 0xD8u,
        0x1Au, 0x1Eu, 0x10u, 0x73u, 0xC8u, 0x65u, 0x57u, 0x64u,
        0x4Bu, 0x87u, 0xE7u, 0x32u, 0x9Bu, 0x5Eu, 0xDFu, 0x8Bu,
        0xF2u, 0x7Au, 0x2Au, 0x48u, 0xCDu, 0x3Du, 0x7Du, 0x11u,
        0x54u, 0x63u, 0x1Du, 0xB1u, 0x62u, 0x04u, 0x64u, 0x1Fu,
        0x1Cu, 0x3Au, 0x6Au, 0x8Cu, 0x16u, 0x1Bu, 0xAEu, 0x5Bu,
        0xD7u, 0x65u, 0x4Bu, 0x12u, 0x1Au, 0xB9u, 0x42u, 0x89u,
        0x72u, 0x16u, 0x8Fu, 0x70u, 0x6Du, 0x9Fu, 0x88u, 0x61u,
        0x80u, 0x90u, 0xBAu, 0x38u, 0x78u, 0xFDu, 0x79u, 0x60u,
        0x54u, 0x11u, 0x8Bu, 0xD4u, 0x20u, 0x4Bu, 0x8Au, 0xF2u,
        0xD5u, 0x29u, 0x4Bu, 0x11u, 0x9Cu, 0x8Fu, 0xBAu, 0x31u,
        0x1Fu, 0x13u, 0x11u, 0xFEu, 0x54u, 0x20u, 0xCAu, 0xEBu,
        0xB5u, 0x15u, 0x65u, 0xE9u, 0x53u, 0x3Eu, 0x95u, 0xD3u,
    },
    .expData =
    {
        0x01u, 0x00u, 0x01u, 0x00u,
    },
    .barrettData =
    {
        0x47u, 0x66u, 0xC3u, 0x2Fu, 0xD5u, 0xC8u, 0x23u, 0xD3u,
        0x3Du, 0x46u, 0xC4u, 0x39u, 0x17u, 0x5Bu, 0x45u, 0x09u,
        0x9Bu, 0x22u, 0x5Du, 0xE6u, 0x26u, 0x70u, 0x43u, 0x26u,
        0xEFu, 0xE6u, 0xE5u, 0xD1u, 0xBFu, 0x3Au, 0x61u, 0xFDu,
        0x93u, 0xE4u, 0x88u, 0xA7u, 0x68u, 0xE4u, 0xEAu, 0x9Au,
        0x1Eu, 0x4Cu, 0x86u, 0x87u, 0x09u, 0x28u, 0x21u, 0x99u,
        0x29u, 0xB2u, 0x6Cu, 0x27u, 0x48u, 0x87u, 0x43u, 0x00u,
        0xE9u, 0x6Du, 0xEDu, 0x89u, 0xD7u, 0xC6u, 0xDEu, 0xBAu,
        0x87u, 0x66u, 0x75u, 0x79u, 0x7Du, 0x49u, 0x68u, 0xC8u,
        0x33u, 0xCEu, 0x49u, 0x62u, 0xACu, 0x70u, 0xBFu, 0x97u,
        0x5Eu, 0x6Bu, 0x64u, 0xEEu, 0xCDu, 0x5Eu, 0x04u, 0xD2u,
        0xC3u, 0x2Bu, 0x4Du, 0x28u, 0x32u, 0x3Du, 0xB4u, 0xEFu,
        0x8Du, 0x06u, 0xB1u, 0x38u, 0xADu, 0x5Bu, 0xEDu, 0xD8u,
        0x36u, 0x39u, 0x35u, 0x34u, 0xD4u, 0xAEu, 0x8Du, 0xF7u,
        0xC0u, 0x19u, 0xFBu, 0x0Au, 0xF0u, 0x94u, 0x02u, 0xC3u,
        0x20u, 0x78u, 0x95u, 0x46u, 0xA6u, 0x12u, 0x69u, 0xEDu,
        0x71u, 0x62u, 0x33u, 0xB8u, 0xC3u, 0x86u, 0xCCu, 0x79u,
        0x7Eu, 0x66u, 0x5Eu, 0xA7u, 0xCEu, 0x61u, 0xB7u, 0xA8u,
        0x39u, 0xE9u, 0x66u, 0x30u, 0x32u, 0xD1u, 0xFAu, 0x47u,
        0xB3u, 0x61u, 0x7Bu, 0x31u, 0x5Eu, 0xB7u, 0xB4u, 0x92u,
        0xECu, 0xC2u, 0x56u, 0x29u, 0xEDu, 0xB3u, 0x19u, 0x1Du,
        0x24u, 0x51u, 0x94u, 0x37u, 0xD0u, 0x18u, 0xA4u, 0x0Eu,
        0x02u, 0x70u, 0xC9u, 0x9Cu, 0x03u, 0xE6u, 0x34u, 0x99u,
        0x5Du, 0xB1u, 0x40u, 0x46u, 0xC1u, 0x70u, 0xA5u, 0x70u,
        0xCBu, 0x79u, 0xF9u, 0x16u, 0xFEu, 0x1Du, 0x13u, 0x61u,
        0x90u, 0x1Cu, 0xF2u, 0x17u, 0x72u, 0xE1u, 0x11u, 0x21u,
        0xAAu, 0x13u, 0x13u, 0x54u, 0x84u, 0x68u, 0x9Au, 0xCBu,
        0x70u, 0x87u, 0xA0u, 0x59u, 0xC6u, 0x9Du, 0x43u, 0x62u,
        0x74u, 0x7Bu, 0xD9u, 0x52u, 0x6Bu, 0xC9u, 0x9Cu, 0x15u,
        0x37u, 0x48u, 0xF3u, 0xA7u, 0x0Cu, 0x80u, 0x78u, 0xEBu,
        0x3Bu, 0x1Fu, 0xDFu, 0x47u, 0x37u, 0x46u, 0xA4u, 0xC0u,
        0x16u, 0xE1u, 0x23u, 0x31u, 0x8Eu, 0xC9u, 0xBDu, 0x35u,
        0x01u, 0x00u, 0x00u, 0x00u,
    },
    .inverseModuloData =
    {
        0x1Fu, 0x5Cu, 0xEBu, 0x74u, 0xD2u, 0x43u, 0x75u, 0xE0u,
        0x68u, 0xABu, 0x5Fu, 0xD6u, 0x0Du, 0xEAu, 0xBAu, 0x34u,
        0x4Au, 0x95u, 0x81u, 0xDAu, 0xFEu, 0x9Au, 0xA2u, 0x77u,
        0x19u, 0xCCu, 0x98u, 0x92u, 0x00u, 0x97u, 0xE9u, 0xBEu,
        0x85u, 0x38u, 0xAFu, 0xA1u, 0xD7u, 0x66u, 0xD3u, 0x3Eu,
        0xBBu, 0x4Bu, 0x41u, 0xDEu, 0x7Du, 0xAEu, 0x9Bu, 0x3Au,
        0x8Au, 0xEEu, 0x06u, 0x93u, 0xB3u, 0xB7u, 0x27u, 0x63u,
        0xEDu, 0x9Cu, 0xB9u, 0x3Fu, 0x09u, 0x1Bu, 0x62u, 0xB6u,
        0x18u, 0x87u, 0x99u, 0x0Eu, 0x15u, 0x60u, 0x19u, 0xD4u,
        0x2Eu, 0x77u, 0xC1u, 0x84u, 0x92u, 0x31u, 0xAEu, 0x1Fu,
        0x2Du, 0xB6u, 0x64u, 0x8Fu, 0xBFu, 0x08u, 0x18u, 0x8Fu,
        0x58u, 0x2Cu, 0x9Bu, 0x68u, 0x85u, 0x46u, 0x89u, 0xEBu,
        0x73u, 0x66u, 0x58u, 0x68u, 0x07u, 0x22u, 0x5Cu, 0x19u,
        0xA5u, 0x57u, 0xCBu, 0x89u, 0x03u, 0xE9u, 0xB3u, 0xA0u,
        0x3Bu, 0x7Du, 0x0Bu, 0xACu, 0xBCu, 0xC4u, 0xFDu, 0x79u,
        0x17u, 0x04u, 0x06u, 0x30u, 0x43u, 0x19u, 0x2Bu, 0x52u,
        0xCCu, 0x93u, 0x13u, 0xAAu, 0x6Au, 0x3Fu, 0x57u, 0xEEu,
        0xB7u, 0x98u, 0x79u, 0x7Fu, 0x2Bu, 0x47u, 0x5Du, 0xF3u,
        0xFFu, 0xDFu, 0x6Fu, 0xC6u, 0xCDu, 0x32u, 0x76u, 0xEAu,
        0x87u, 0x87u, 0x2Du, 0x6Du, 0x84u, 0x31u, 0x9Du, 0xD2u,
        0x78u, 0xB8u, 0xCFu, 0xC6u, 0xDBu, 0xE6u, 0x68u, 0x6Du,
        0xA1u, 0x47u, 0x4Du, 0xD6u, 0x60u, 0x66u, 0x9Du, 0x89u,
        0xD2u, 0xDBu, 0xBFu, 0x54u, 0xFDu, 0x69u, 0xF9u, 0x37u,
        0x5Fu, 0xFAu, 0xDAu, 0xB1u, 0x91u, 0x49u, 0x00u, 0xAEu,
        0x01u, 0x63u, 0xD5u, 0xAFu, 0x8Bu, 0xB7u, 0x9Eu, 0x1Fu,
        0x3Eu, 0x5Au, 0xF0u, 0xA6u, 0xEBu, 0x27u, 0x03u, 0xC1u,
        0xEBu, 0x96u, 0xC7u, 0x35u, 0x03u, 0xA0u, 0xA2u, 0x06u,
        0x3Du, 0x94u, 0xB8u, 0xCBu, 0x9Fu, 0x59u, 0x7Du, 0x57u,
        0x63u, 0xFCu, 0xF4u, 0xA2u, 0x2Du, 0xA8u, 0xC4u, 0x04u,
        0xECu, 0xE6u, 0x7Fu, 0x35u, 0x51u, 0x97u, 0x37u, 0xF1u,
        0x2Bu, 0x46u, 0x7Bu, 0xB4u, 0xFBu, 0xDAu, 0x7Au, 0x5Bu,
        0x44u, 0x9Au, 0x71u, 0xD0u, 0xFCu, 0xBAu, 0xE6u, 0xE8u,
    },
    .rBarData =
    {
        0xDFu, 0x1Fu, 0xCDu, 0xDEu, 0xD8u, 0xA5u, 0xFEu, 0x8Au,
        0x1Eu, 0xBEu, 0xACu, 0x25u, 0x59u, 0x73u, 0x7Cu, 0x15u,
        0x6Fu, 0x34u, 0x33u, 0x76u, 0x13u, 0x78u, 0xADu, 0x40u,
        0x49u, 0x5Cu, 0x47u, 0x2Cu, 0xF0u, 0x24u, 0x37u, 0x11u,
        0xA0u, 0xDAu, 0x83u, 0x70u, 0x01u, 0x08u, 0x86u, 0x1Eu,
        0x94u, 0x3Bu, 0x7Cu, 0xB9u, 0x7Du, 0x53u, 0xC2u, 0xE9u,
        0x86u, 0xA6u, 0xE0u, 0xDBu, 0xA6u, 0xBCu, 0x08u, 0xD6u,
        0x66u, 0xFBu, 0x54u, 0x26u, 0xA7u, 0xA7u, 0x05u, 0x41u,
        0x1Au, 0xD0u, 0x98u, 0xA5u, 0xE6u, 0xF0u, 0x65u, 0x66u,
        0x2Bu, 0x8Bu, 0x60u, 0x73u, 0x5Cu, 0xBBu, 0x70u, 0x9Eu,
        0x2Au, 0x60u, 0x89u, 0x73u, 0xAEu, 0x44u, 0x29u, 0x31u,
        0xC3u, 0xEEu, 0xC5u, 0x42u, 0x61u, 0x0Fu, 0x77u, 0x44u,
        0x4Du, 0xABu, 0x46u, 0x25u, 0xB9u, 0xDDu, 0x9Du, 0x02u,
        0x0Cu, 0xFAu, 0x66u, 0xD5u, 0x6Cu, 0x74u, 0xCEu, 0xFAu,
        0x59u, 0x04u, 0xCAu, 0xB6u, 0xABu, 0x11u, 0xF4u, 0x22u,
        0xE7u, 0xF5u, 0xA4u, 0xFEu, 0xF6u, 0xA3u, 0x25u, 0xB2u,
        0xA1u, 0xE3u, 0xDEu, 0xF5u, 0x29u, 0x05u, 0xE0u, 0x66u,
        0x5Du, 0xB2u, 0x11u, 0xD1u, 0x12u, 0x12u, 0x9Au, 0x34u,
        0x9Eu, 0x3Cu, 0xCBu, 0x50u, 0x85u, 0xD8u, 0x32u, 0x2Du,
        0xD7u, 0xA7u, 0xCBu, 0xE0u, 0x6Bu, 0xEAu, 0x18u, 0x27u,
        0xE5u, 0xE1u, 0xEFu, 0x8Cu, 0x37u, 0x9Au, 0xA8u, 0x9Bu,
        0xB4u, 0x78u, 0x18u, 0xCDu, 0x64u, 0xA1u, 0x20u, 0x74u,
        0x0Du, 0x85u, 0xD5u, 0xB7u, 0x32u, 0xC2u, 0x82u, 0xEEu,
        0xABu, 0x9Cu, 0xE2u, 0x4Eu, 0x9Du, 0xFBu, 0x9Bu, 0xE0u,
        0xE3u, 0xC5u, 0x95u, 0x73u, 0xE9u, 0xE4u, 0x51u, 0xA4u,
        0x28u, 0x9Au, 0xB4u, 0xEDu, 0xE5u, 0x46u, 0xBDu, 0x76u,
        0x8Du, 0xE9u, 0x70u, 0x8Fu, 0x92u, 0x60u, 0x77u, 0x9Eu,
        0x7Fu, 0x6Fu, 0x45u, 0xC7u, 0x87u, 0x02u, 0x86u, 0x9Fu,
        0xABu, 0xEEu, 0x74u, 0x2Bu, 0xDFu, 0xB4u, 0x75u, 0x0Du,
        0x2Au, 0xD6u, 0xB4u, 0xEEu, 0x63u, 0x70u, 0x45u, 0xCEu,
        0xE0u, 0xECu, 0xEEu, 0x01u, 0xABu, 0xDFu, 0x35u, 0x14u,
        0x4Au, 0xEAu, 0x9Au, 0x16u, 0xACu, 0xC1u, 0x6Au, 0x2Cu,
    },
};

/* Secure Application header for the Dual Bank Manager */
CY_SECTION(".cy_sf_app_header") __USED static const cy_stc_si_appheader_t cy_si_sflash_appHeader =
{
    .objSize        = CY_SFLASH_SI_SIZE,
    .appId          = (CY_SI_APP_VERSION(DBM_VERSION_MAJOR, DBM_VERSION_MINOR, DBM_VERSION_BUILD) | CY_SI_APP_ID_SECUREIMG),
    .appAttributes  = 0UL,                            /* Reserved */
    .numCores       = 1UL,                            /* Only CM0+ */
    .core0Vt        = CY_SF_SI_VT_OFFSET,             /* CM0+ VT offset */
    .core0Id        = CY_SI_CPUID | CY_SI_CORE_IDX,   /* CM0+ core ID */
};

/* [] END OF FILE */
