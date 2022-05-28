/*
*****************************************************************************
* @file:    uart_loop_back.h
* @brief:   File which contain "main" for testing UART Device Driver.
*****************************************************************************

Copyright(c) 2016 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/
#ifndef C_UART_INST_H
#define C_UART_INST_H

// -----------------------------------------------------
// // from max78000 uart example

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "uart.h"
#include "dma.h"
#include "nvic_table.h"


/***** Definitions *****/
// #define DMA

#define UART_BAUD           230400 //115200
//#define BUFF_SIZE           1024

/***** Globals *****/
//volatile int READ_FLAG;
//volatile int DMA_FLAG;

#if defined (BOARD_EVKIT_V1)
#define ADUP_UART			0
#define READING_UART        1
#define WRITING_UART        2
#elif defined (BOARD_FTHR_REVA)
#define ADUP_UART			0
//#define READING_UART        2
//#define WRITING_UART        3
#else
#error "This example has been written for the MAX78000 Ev Kit or FTHR board."
#endif

// ----------------------------------------------------------

#include "c_adup_lib.h"

#define UART_USE_DMA false // JTE: warning, don't turn this on! our callback scheme doesn't handle DMA
#define OVR_BUF_ALLOC_BYTES 128+3 // largest packet, plus 3 byte header


  typedef struct C_UART_INST C_UART_INST;
struct C_UART_INST {
    // functions
    void (*uart_ovr_callback)(void);
    uint16_t (*uart_ovr_drain)(uint8_t *const buf, uint32_t nbytes);
    bool (*uart_ovr_cmp)(char *buf, uint16_t len);
    
    void (*adup_deautobaud)(void);
    void (*adup_manualBaud)(uint16_t nDivC, uint8_t nDivM,	uint16_t nDivN,	uint8_t nOSR);
    
    void (*adup_autobaudStart)(void);
    uint32_t (*adup_autobaudTest)(void);
    void (*adup_autobaudEnd)(void);
    
    void (*adup_autobaud)(void);
    int (*adup_uart_init)(void);
//    ADI_UART_HANDLE* (*uart_getdev)(void);
    void (*uart_read)(void *const buf, unsigned int const size);
    void (*uart_write)(void *const buf, unsigned int const size);
    
    void (*uart_flush)(void);
    void (*uart_resync)(void);
    
    void (*uart_RxChanFlush)(void);
    bool (*uart_checkFifo)(void);
    uint16_t (*uart_GetRxFifo)(void);
    uint16_t (*uart_GetTxFifo)(void);
    
    // static ADI_UART_HANDLE hUartDev; /* Handle for the UART device. */
    // static uint8_t UartDeviceMem[ADI_UART_MEMORY_SIZE]; /* Memory for the UART driver. */
    // static char *ubuf; /* dedicated memory for xfers - single buffer used for both RX and TX */
    // static char *_ubuf; /* dedicated memory for xfers - single buffer used for both RX and TX */

    // fields
    uint8_t ovrBuf[512];
    uint16_t ovrSize;
    volatile uint16_t ovrBytes;
    volatile uint16_t ovrIdxL;
    volatile uint16_t ovrIdxH;
    
    uint32_t _baud;
    mxc_uart_regs_t *mxc_uart;
};
  

C_UART_INST* C_UART_INST_init(int baud);

/* ----- */

void c_uart_inst_uart_ovr_push(uint8_t *buf, int sz);
void c_uart_inst_uart_ovr_callback(void);
uint16_t c_uart_inst_uart_ovr_drain(uint8_t *const buf, uint32_t nbytes);
bool c_uart_inst_uart_ovr_cmp(char *buf, uint16_t len);

void c_uart_inst_adup_deautobaud(void);
void c_uart_inst_adup_manualBaud(uint16_t nDivC, uint8_t nDivM,	uint16_t nDivN,	uint8_t nOSR);

void c_uart_inst_adup_autobaudStart(void);
uint32_t c_uart_inst_adup_autobaudTest(void);
void c_uart_inst_adup_autobaudEnd(void);

void c_uart_inst_adup_autobaud(void);
int c_uart_inst_adup_uart_init(void);
//  ADI_UART_HANDLE* c_uart_inst_uart_getdev(void);
void c_uart_inst_uart_read(void *const buf, unsigned int const size);
void c_uart_inst_uart_write(void *const buf, unsigned int const size);

//void c_uart_inst_uart_flush(void);
void c_uart_inst_uart_resync(void);

void c_uart_inst_uart_RxChanFlush(void);
bool c_uart_inst_uart_checkFifo(void);
uint16_t c_uart_inst_uart_GetRxFifo(void);
uint16_t c_uart_inst_uart_GetTxFifo(void);


#endif
