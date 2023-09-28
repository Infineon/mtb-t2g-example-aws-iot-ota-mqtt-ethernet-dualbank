/******************************************************************************
** __DISCLAIMER_START__                                                      */
/******************************************************************************
* Project Name      : TVII_8M_Secure_FOTA
* File Name         : startup_sflash.S
* Version           : 1.0
* Device Used       : CYT4BFBCHE
* Software Used     : MTB3.0
* Compiler Used     : GCC
* Related Hardware  : KIT_T2G-B-H_EVK
******************************************************************************/
/* __DISCLAIMER_END__                                                        */
/*****************************************************************************/
.syntax unified
.arch    armv6-m

/**************************************************************************/
/** Local definitions */
/**************************************************************************/

/* Set a sufficient startup stack size for correct operation of C startup code (startup.c)*/
#define STARTUP_STACK_SIZE_DOUBLE_WORDS     128

#define VTOR                                0xe000ed08
#define CPUSS_RAM0_CTL0                     0x40201300
#define CPUSS_RAM1_CTL0                     0x40201380
#define CPUSS_RAM2_CTL0                     0x402013a0

#define GPIO_PRT0_CFG                       0x40310044
#define GPIO_PRT0_OUT_CLR                   0x40310004
#define GPIO_PRT0_OUT_SET                   0x40310008
#define GPIO_PRT2_CFG                       0x40310144
#define GPIO_PRT2_OUT_CLR                   0x40310104
#define GPIO_PRT2_OUT_SET                   0x40310108

/* Macros used for flash configurations */
#define MAGIC_WORD                          0xAAAAAAAA
#define ADDR_MAKER_ON_WORK                  0x14012000
#define FLASHC_FLASH_CTL                    0x40240000

/* Macros used for application authentication */

#define CY_FB_VERIFYAPP_ADDR                0x17002040
#define CY_PUBLIC_KEY_ADDR                  0x17006408
#define CY_APP_START_ADDR_LB                0x10000000

/* Macro to enable application authentication by Dual Bank Manager */
#define AUTHENTICATION_ENABLE               0x00000001

/* Macro to disable application authentication by Dual Bank Manager */
#define AUTHENTICATION_DISABLE              0x55555555

/* The following macros should be set by user based on the device and requirement */

/* Set AUTHENTICATION_CHECK to AUTHENTICATION_ENABLE or AUTHENTICATION_DISABLE */
#define AUTHENTICATION_CHECK                AUTHENTICATION_ENABLE

/* Mid-Point of the Large sectors in single bank mode for Traveo II 1M device. This value shall be modified based on the Target Device.*/
/* Application intended to be executed out of Map B shall be placed at this starting address.*/

#define CY_APP_START_ADDR_UB                0x103F8000

/* Macro used for the flash boundary check. This macro has the value ( Map A/B Large sector end address in single bank mode - size of the digital signature (256 bytes))for Traveo II 1M device.*/
/* These values shall be modified based on the Target Device */

#define CY_APP_END_ADDR_LB                  0x1000FF00    /* 0x10078000 - 0x100 */
#define CY_APP_END_ADDR_UB                  0x10407F00    /* 0x100F0000 - 0x100 */

/* For CYT2B78CAE the user reset handler is _startcontd which is continuation of the SFlash _start */

#define User_Reset_Handler                  _startcontd

/* Macros used to skip the bank check. If Dual Bank Manager detects the user button of the kit specified below pushed, it selects the lower bank to boot. */
#define ADDR_GPIO_PRT_CFG                   0x403102C4    /* port5 */
#define ADDR_GPIO_PRT_IN                    0x40310290    /* port5 */
#define GPIO_PRT_BIT                        3             /* bit3 */

                .global  __mapboot_end__
                .global  Reset_Handler

/**************************************************************************/
/** Vector Table */
/**************************************************************************/
/* This is the vector table required for CySAF of the Dual Bank Manager. Only the first 4 entries are used, others are filled with zero.*/
        .section .sflashintvec, "a"

__sflashvector_table:
        .word     __StackTop
        .word     start_sflash
        .word     Exception_handler
        .word     Exception_handler
        .word     0
        .word     0
        .word     0
        .word     0
        .word     0
        .word     0
        .word     0
        .word     0
        .word     0
        .word     0
        .word     0
        .word     0

        /* External interrupts              PowerMode    Description */

        .word     0                        /* DeepSleep    CPU User Interrupt #0 */
        .word     0                        /* DeepSleep    CPU User Interrupt #1 */
        .word     0                        /* DeepSleep    CPU User Interrupt #2 */
        .word     0                        /* DeepSleep    CPU User Interrupt #3 */
        .word     0                        /* DeepSleep    CPU User Interrupt #4 */
        .word     0                        /* DeepSleep    CPU User Interrupt #5 */
        .word     0                        /* DeepSleep    CPU User Interrupt #6 */
        .word     0                        /* DeepSleep    CPU User Interrupt #7 */

        /* These IRQs can only be triggered by SW via NVIC regs */
        .word     0                        /* Active       CPU User Interrupt #8 */
        .word     0                        /* Active       CPU User Interrupt #9 */
        .word     0                        /* Active       CPU User Interrupt #10 */
        .word     0                        /* Active       CPU User Interrupt #11 */
        .word     0                        /* Active       CPU User Interrupt #12 */
        .word     0                        /* Active       CPU User Interrupt #13 */
        .word     0                        /* Active       CPU User Interrupt #14 */
        .word     0                        /* Active       CPU User Interrupt #15 */

/* SRAM section to reserve space for SRAM function needed for flash bank configurations */
/*        .section .ramprog*/
/*        .align 8*/

/* address and size for to be used as region of ramprog */
#define _flashbankswitchramaddr        _flashbankswitch
#define _flashbankswitchsize           (_flashbankswitchend - _flashbankswitch)
#define _flashbankswitchromaddr        __mapboot_end__

#define SYST_CSR            0xE000E010
#define SYST_RVR            0xE000E014
#define SYST_CVR            0xE000E018

/**************************************************************************/
/** Start-up Code */
/**************************************************************************/

/* The code is generated only for the CYT2B78CAE with FOTA macro defined ('tviibe1m-complete' project)*/

        .thumb
        .section .mapboot, "ax"
        .align 4

/* For CYT2B78CAE device, the SFlash application will be placed in the SFlash user data region and program in NORMAL_PROVISIONED mode.*/
/* The FlashBoot will launch this application, the reset handler of this application checks if authentication of CM0+ CodeFlash application */
/* is valid , copies _flashbankswitch function to SRAM and and jumps to SRAM function which does the flash bank operation and jumps to */
/* reset handler of the CM0+ CodeFlash application where additional startup is continued */

        .thumb_func
        .type    start_sflash, %function
        .globl   start_sflash
        .extern  __cmsis_start
        .fnstart
start_sflash:
        /* Configure & Set low-level for P0_2/P0_3/P2_0/P2_1 */
        LDR  r4, =0x00006600
        LDR  r1, =GPIO_PRT0_CFG
        STR  r4, [r1]
        LDR  r4, =0x00000066
        LDR  r1, =GPIO_PRT2_CFG
        STR  r4, [r1]
        LDR  r4, =0x00000004
        LDR  r1, =GPIO_PRT0_OUT_SET
        STR  r4, [r1]

        /* Start counting processing cycles using SysTick */
        ldr  r1, =SYST_RVR
        movs r0, #1
        lsls r0, r0, #24
        subs r0, 1
        str  r0, [r1]
        ldr  r1, =SYST_CVR
        str  r0, [r1]
        ldr  r1, =SYST_CSR
        movs r0, #5
        str  r0, [r1]

/*Disable global interrupts */
        CPSID I

/* Update Vector Table Offset Register with address of SFlash vector table */

        LDR  r0, =__sflashvector_table
        LDR  r1, =VTOR
        STR  r0, [r1]
        DSB

/* CM0+ bus width is 32-bit, but SRAM is built with 64-bit based ECC on Traveo II parts with CM7 core */
/* Set CPUSS->RAMx_CTL0.ECC_CHECK_DIS bits to avoid causing unintentional ECC faults during startup while SRAM ECC has not been initialized yet */
/* Done for all SRAM macros as the stack/function could be placed any where based on linker script, this could also be modified only to specific SRAM macros */

        MOVS r4, #1
        LSLS r4, r4, #19
        LDR  r1, =CPUSS_RAM0_CTL0
        LDR  r2, [r1]
        ORRS r2, r4
        STR  r2, [r1]
        LDR  r1, =CPUSS_RAM1_CTL0
        LDR  r2, [r1]
        ORRS r2, r4
        STR  r2, [r1]
        LDR  r1, =CPUSS_RAM2_CTL0
        LDR  r2, [r1]
        ORRS r2, r4
        STR  r2, [r1]

/* Initialize ECC of startup stack */
        MOVS r0, #0 /* clear value */
        MOVS r1, #0 /* clear value */
        LDR  r2, flashmarkers+28
		LDR  r3, =__StackTop

startup_stack_ecc_init_loop:
        STM  r2!, {r0, r1}
        CMP  r2, r3
        BNE  startup_stack_ecc_init_loop

/* For Traveo II devices with 64-bit based ECC SRAM, initialize ECC of the region where SRAM function shall be copied later from SFlash */

        LDR  r2, flashmarkers
/* Subtract 1 from R2 due to Thumb alignment */
        SUBS r2, 1
        LDR  r3, flashmarkers+4

/* loop to clear the SRAM */
SRAM_function_ecc_init_loop:
        STR  r1, [r2,r0]
        ADDS r0, #8
        CMP  r0, r3
        BCC  SRAM_function_ecc_init_loop

/* Clear CPUSS->RAMx_CTL0.ECC_CHECK_DIS bits to enable ECC check/correction; r4 already has the bit position.*/

        LDR  r1, =CPUSS_RAM0_CTL0
        LDR  r2, [r1]
        BICS r2, r4
        STR  r2, [r1]
        LDR  r1, =CPUSS_RAM1_CTL0
        LDR  r2, [r1]
        BICS r2, r4
        STR  r2, [r1]
        LDR  r1, =CPUSS_RAM2_CTL0
        LDR  r2, [r1]
        BICS r2, r4
        STR  r2, [r1]

/* Load the address for FLASHC_FLASH_CTL */
        LDR  r4, flashmarkers+8

/* Set FLASHC_FLASH_CTL.WORK_ERR_SILENT */
/* Reading the Work Flash in erased state can cause bus fault when FLASHC_FLASH_CTL.WORK_ERR_SILENT is 0 */
        MOVS r1, #1
        LSLS r1, r1, #22
        ORRS r0, r1
        STR  r0, [r4]
        LDR  r0, [r4]

SkipMagicWordCheck:
        LDR  r3, flashmarkers+20        /* Load the flag for authentication check */
        LDR  r4, flashmarkers+24        /* Load the AUTHENTICATION_DISABLE for comparison */
        MOVS r2, #GPIO_PRT_BIT          /* Make the bit value of the port */
        LSLS r2, r2, #2                 /* Generate the bit value within the CFG register */
        ADDS r2, #3
        MOVS r1, #1
        LSLS r1, r1, r2
        LDR  r2, flashmarkers+36        /* Load the address of PRTx_CFG register */
        LDR  r0, [r2]                   /* Load the value to r0 */
        ORRS r0, r1
        STR  r0, [r2]                   /* Set configuration of the port to input mode */
        LDR  r2, flashmarkers+40        /* Load the address of PRTx_IN register */
        LDR  r0, [r2]                   /* Load the value to r0 */
        MOVS r2, #GPIO_PRT_BIT          /* Make the bit value of the port again */
        MOVS r1, #1
        LSLS r1, r1, r2
        ANDS r0, r1                     /* Check the button state */
        BEQ  Check_MapA_LB              /* if pushed (==0), skip the magic word check and use lower bank */

MagicWordCheck:

/* Check if the magic word is present in Work Flash address and branch to appropriate label */

        LDR  r2, flashmarkers+12        /* Load the address of WorkFlash marker */
        LDR  r0, [r2]                   /* Load the marker value to r0 */
        LDR  r1, flashmarkers+16        /* Load the magic word */
        CMP  r0, r1
        BNE  Check_MapA_LB

Check_MapB_UB:
/* Check if the authentication check is disabled. If disabled directly jump to Auth_Skip_MapB. If enabled, perform the authentication.*/
        CMP  r3, r4
        BEQ  Auth_Skip_MapB

/* Check if the result of authentication of upper bank image is successful, if not jump to Check_MapB_LB and check lower bank */
        BL   _checkUB
        CMP  r0, #1
        BNE  Check_MapB_LB

Auth_Skip_MapB:
/* Jump to the FlashtoRAM_copy function with register r6 set */
        MOVS r6, #1
        B    FlashtoRAM_copy

Check_MapB_LB:
/* Check if the result of authentication of lower bank image is successful, if not jump to ImageInvalid, and no application is launched.*/
        BL   _checkLB
        CMP  r0, #1
        BNE  ImageInvalid

/* Jump to the FlashtoRAM_copy copy function with register r6 cleared */
        MOVS r6, #0
        B    FlashtoRAM_copy

Check_MapA_LB:

/* Check if the authentication check is disabled. If disabled directly jump to Auth_Skip_MapA. If enabled, perform the authentication.*/
        CMP  r3, r4
        BEQ  Auth_Skip_MapA

/* Check if the result of authentication of lower bank image is successful, if not jump to Check_MapA_UB and check upper bank */
        BL   _checkLB
        CMP  r0, #1
        BNE  Check_MapA_UB

Auth_Skip_MapA:
/* Jump to the FlashtoRAM_copy function with register r6 cleared */
        MOVS r6, #0
        B    FlashtoRAM_copy

Check_MapA_UB:
/* Check if the result of authentication of upper bank image is successful, if not jump to ImageInvalid, no application is launched.*/
        BL   _checkUB
        CMP  r0, #1
        BNE  ImageInvalid

/* Jump to the FlashtoRAM_copy function with register r6 set */
        MOVS r6, #1
        B    FlashtoRAM_copy
/* Enters this handler when both banks have invalid images */
        .fnend
        .size    start_sflash, . - start_sflash

ImageInvalid:
        B    ImageInvalid

/* Enters this handler when HardFault happens during the Dual Bank Manager execution */
Exception_handler:
        B    Exception_handler

        .align 8
        .thumb_func
        .type    FlashtoRAM_copy, %function
FlashtoRAM_copy:

/* Copy the _flashbankswitch function to SRAM and Dual Bank Manager jumps to SRAM where the dual bank configuration is done */

/* Load the register with the source address, destination address and size */
/*        ADR  r3, _flashbankswitch*/
        LDR  r3, flashmarkers+32
        LDR  r2, flashmarkers
        LDR  r1, flashmarkers+4

/* Subtract 1 from R2 due to Thumb alignment */
        SUBS r2, #1

/* R0 is used for compare operation */
        MOVS r0, #0
        B    compare

/* loop to copy the complete SFlash function to SRAM */
copyloop:
        LDR  r4, [r3,r0]
        STR  r4, [r2,r0]
        ADDS r0, #4
compare:
        CMP  r0, r1
        BCC  copyloop

/* Jump to the _flashbankswitch copy function in SRAM with valid r6 */
        LDR  r7, flashmarkers
        BX   r7
/*debug	B _flashbankswitch */

/* Markers for flash copy operation */
.align 4
flashmarkers:
        .word    _flashbankswitchramaddr                                    /* flashmarkers */
        .word    _flashbankswitchsize                                       /* flashmarkers+4 */
        .word    FLASHC_FLASH_CTL                                           /* flashmarkers+8 */
        .word    ADDR_MAKER_ON_WORK                                         /* flashmarkers+12 */
        .word    MAGIC_WORD                                                 /* flashmarkers+16 */
        .word    AUTHENTICATION_CHECK                                       /* flashmarkers+20 */
        .word    AUTHENTICATION_DISABLE                                     /* flashmarkers+24 */
        .word    (__StackTop - (STARTUP_STACK_SIZE_DOUBLE_WORDS * 8))       /* flashmarkers+28 */
        .word    _flashbankswitchromaddr                                    /* flashmarkers+32 */
        .word    ADDR_GPIO_PRT_CFG                                          /* flashmarkers+36 */
        .word    ADDR_GPIO_PRT_IN                                           /* flashmarkers+40 */

/* Performs authentication of the lower Bank */
LowerBankCheck:
.align 4

_checkLB:

        PUSH {LR}

/* Check if the Application is valid with Cy_FB_VerifyApplication()*/

/* Load the registers with the lower bank parameters for Cy_FB_VerifyApplication() function */
        LDR  r0, addrmarker+4        /* address of the lower bank image */
        LDR  r1, [r0]                /* length of the lower bank image */
        ADDS r2, r0, r1              /* address of the digital signature */
        BCS  skip_LB_check           /* carry bit detected, boundary violation */
        LDR  r4, addrmarker+16       /* end address for the lower bank */
        CMP  r2, r4                  /* boundary condition check */
        BHI  skip_LB_check           /* skip Cy_FB_VerifyApplication */
        LDR  r3, addrmarker+8        /* address of the public key */

/* Get the address of Cy_FB_VerifyApplication() from SFlash marker */
        LDR  r4, addrmarker
        LDR  r7, [r4]

/* Call Cy_FB_VerifyApplication() function */
        BLX  r7

skip_LB_check:
        POP  {PC}

/* Performs authentication of the upper Bank */
UpperBankCheck:
.align 4

_checkUB:

        PUSH {LR}

/* Check if the Application is valid with Cy_FB_VerifyApplication()*/

/* Load the registers with the upper bank parameters for Cy_FB_VerifyApplication() function */
        LDR  r0, addrmarker+12       /* address of the upper bank image */
        LDR  r1, [r0]                /* length of the upper bank image */
        ADDS r2, r0, r1              /* address of the digital signature */
        BCS  skip_UB_check           /* carry bit detected, boundary violation */
        LDR  r4, addrmarker+20       /* end address for the upper bank */
        CMP  r2, r4                  /* boundary condition check */
        BHI  skip_UB_check           /* skip Cy_FB_VerifyApplication */
        LDR  r3, addrmarker+8        /* address of the public key */

/* Get the address of Cy_FB_VerifyApplication() from SFlash marker */
        LDR  r4, addrmarker
        LDR  r7, [r4]

/* Call Cy_FB_VerifyApplication() function */
        BLX  r7

skip_UB_check:
        POP  {PC}

/* Markers used by the LowerBankCheck and UpperBankCheck functions */
.align 4
addrmarker:
        .word    CY_FB_VERIFYAPP_ADDR      /* addrmarker */
        .word    CY_APP_START_ADDR_LB      /* addrmarker+4 */
        .word    CY_PUBLIC_KEY_ADDR        /* addrmarker+8 */
        .word    CY_APP_START_ADDR_UB      /* addrmarker+12 */
        .word    CY_APP_END_ADDR_LB        /* addrmarker+16 */
        .word    CY_APP_END_ADDR_UB        /* addrmarker+20 */

        .section .ramprog, "ax"
.align 4
/* Flash copy for the dual bank configuration function */
/* This function does the dual bank configuration based on r6 register, invalidates the cache and jumps to the reset handler of the CM0+ application.*/
        .thumb_func
        .type    _flashbankswitch, %function
_flashbankswitch:

/* Load the address for FLASHC_FLASH_CTL */
        LDR  r4, ramdatamarker

/* Set FLASHC_FLASH_CTL.MAIN_BANK_MODE for dual bank mode */
        LDR  r0, [r4]
        MOVS r1, #1
        LSLS r1, r1, #12
        ORRS r0, r1
        STR  r0, [r4]
        LDR  r0, [r4]

/* Set '1' for bit corresponding to FLASHC_FLASH_CTL.MAIN_MAP in register R1 */
        MOVS r1, #1
        ADDS r1, #255

/* Check if register R6 is set */
        CMP  r6, #1
        BNE  MapA

/* Map B is valid, set the bit corresponding to FLASHC_FLASH_CTL.MAIN_MAP */
MapB:
        ORRS r0, r1
        B    common

/* Map A is valid, clear the bit corresponding to FLASHC_FLASH_CTL.MAIN_MAP */
MapA:
        BICS r0, r1

common:

/* configure FLASHC_FLASH_CTL.MAIN_MAP */
        STR  r0, [r4]

/* read back FLASHC_FLASH_CTL */
        LDR  r0, [r4]

/* instruction barrier operation */
        ISB  SY

/* invalidate cache and buffers */
        LDR  r0, [r4,8]
        MOVS r1, #1
        ORRS r0, r1
        STR  r0, [r4,8]

/* wait till cache and buffers are invalidated */
cache_clearloop:
        LDR  r0, [r4,8]
        LSLS r0, r0, #31
        BMI  cache_clearloop

/* jump to the _startcontd function which is the reset handler of the CM0+ application placed at 0x10000000 based on CySAF */
        LDR  r5, ramdatamarker+4         /* Load the starting address of CySAF */
        LDR  r1, [r5,16]
        ADDS r1, #16
        ADDS r5, r1
        LDR  r7, [r5,4]
        BX   r7

/* Markers used by the RAM function */
.align 4
ramdatamarker:
        .word    FLASHC_FLASH_CTL          /* ramdatamarker */
        .word    CY_APP_START_ADDR_LB      /* ramdatamarker+4 */

_flashbankswitchend:

/**************************************************************************/
/** File end */
/**************************************************************************/

        .END

