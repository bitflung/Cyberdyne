/*
*****************************************************************************
* @file:    uart_loop_back.c
* @brief:   File which contain "main" for testing UART Device Driver.
*****************************************************************************

Copyright(c) 2016 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/
/*!
* @file      uart_loop_back.c
*
* @brief     This file contains an example that demonstrates loopback with
*            UART device driver APIs in nonblocking mode.
*
*/

#include "c_uart_inst.h"

C_UART_INST _uart;

mxc_uart_req_t read_req;
mxc_uart_req_t write_req;
#define UART_BUF_SZ 8 // using 8 as that's the size of the FIFO
uint8_t rxbuf[UART_BUF_SZ];
volatile bool writePending=false;
///////////////////////////



#ifdef DMA
void DMA_Handler(void)
{
    MXC_DMA_Handler();
    DMA_FLAG = 0;
}
#else
void UART_Handler(void)
{
    MXC_UART_AsyncHandler(MXC_UART_GET_UART(ADUP_UART));
}
#endif


////////////////

C_UART_INST* C_UART_INST_init(int baud){
  // functions
  _uart.uart_ovr_callback = &c_uart_inst_uart_ovr_callback;
  _uart.uart_ovr_drain = &c_uart_inst_uart_ovr_drain;
  _uart.uart_ovr_cmp = &c_uart_inst_uart_ovr_cmp;
  
//  _uart.adup_deautobaud = &c_uart_inst_adup_deautobaud;
//  _uart.adup_manualBaud = &c_uart_inst_adup_manualBaud;
//
//  _uart.adup_autobaudStart = &c_uart_inst_adup_autobaudStart;
//  _uart.adup_autobaudTest = &c_uart_inst_adup_autobaudTest;
//  _uart.adup_autobaudEnd = &c_uart_inst_adup_autobaudEnd;
//
//  _uart.adup_autobaud = &c_uart_inst_adup_autobaud;
  _uart.adup_uart_init = &c_uart_inst_adup_uart_init;
  //    ADI_UART_HANDLE* (*uart_getdev)(void);
  _uart.uart_read = &c_uart_inst_uart_read;
  _uart.uart_write = &c_uart_inst_uart_write;
  
//  _uart.uart_flush = &c_uart_inst_uart_flush;
  _uart.uart_resync = &c_uart_inst_uart_resync;
  
  _uart.uart_RxChanFlush = &c_uart_inst_uart_RxChanFlush;
  _uart.uart_checkFifo = &c_uart_inst_uart_checkFifo;
  _uart.uart_GetRxFifo = &c_uart_inst_uart_GetRxFifo;
  _uart.uart_GetTxFifo = &c_uart_inst_uart_GetTxFifo;
  
  // static ADI_UART_HANDLE hUartDev; /* Handle for the UART device. */
  // static uint8_t UartDeviceMem[ADI_UART_MEMORY_SIZE]; /* Memory for the UART driver. */
  // static char *ubuf; /* dedicated memory for xfers - single buffer used for both RX and TX */
  // static char *_ubuf; /* dedicated memory for xfers - single buffer used for both RX and TX */
  _uart._baud = baud;
  
  // fields
  // nothing to init here
  
  // function calls
  _uart.adup_uart_init();
  


  return &_uart;
}

void readCallback(mxc_uart_req_t* req, int error){
	// collect the RX'd byte and request another; if RXFIFO has multiple bytes, request them all
	c_uart_inst_uart_ovr_push(read_req.rxData, read_req.rxCnt);
	if(false && read_req.rxCnt>0){
		printf("(ovrBytes=%d) (writePending=%s)\n", _uart.ovrBytes, writePending?"true":"false");
	}
	if(!writePending) MXC_UART_TransactionAsync(&read_req);
}

void drainRXfifo(void){
	while(_uart.mxc_uart->status & 0xF00){
		// while ther are bytes in the RX fifo
		c_uart_inst_uart_ovr_push(_uart.mxc_uart->fifo, 1);
	}
}

void writeCallback(mxc_uart_req_t* req, int error){
	printf("writeCallback executing\n");
	writePending = false;
}

void executeCallback(){
	c_uart_inst_uart_ovr_callback();
}

void c_uart_inst_uart_ovr_push(uint8_t *buf, int sz){
	if(sz==0) return;
	if(_uart.ovrBytes + sz >= _uart.ovrSize) while(1); // sanity check: would draining these bytes overflow our buffer? trap here for debug

	  // actually drain the RX FIFO into our circular buffer
	  for(int i=0; i<sz; i++){
		  //MXC_UART_ReadRXFIFO();

	    _uart.ovrBuf[_uart.ovrIdxH] = buf[i]; // TODO drain a byte from the fifo and store here // (uint8_t)hUartDev->pUARTRegs->RX; // push data into the IdxH element

	    _uart.ovrIdxH++; // increment the ovrIdxH index
	    _uart.ovrBytes++; // increment the bytes counter

	    if(_uart.ovrIdxH == _uart.ovrSize)
	      _uart.ovrIdxH = 0; // wrap around the circular buffer

	    // ovrIdxH == ovrIdxL is expected only if the buffer is full or empty (in this case, full)
	  }

	  // sanity check: if we overflow the buffer (shouldn't happen) then die here in a while(1)
	  if(_uart.ovrBytes > _uart.ovrSize)  while(1); // no error handling yet if the buffer overflows without bounds checking
}

void c_uart_inst_uart_ovr_callback(void){
	// DEPRECATED FOR MAX78000
  //printf(".\n");
//  bool wrongEvent = false;
//  wrongEvent = false; // TODO check if the callback event is NOT as expected // (evt != ADI_UART_EVENT_NO_RX_BUFFER_EVENT);
//
//  if(wrongEvent) return;
//
//  uint32_t nBytes;
//  nBytes = 0;// TODO read the UART RX FIFO count // hUartDev->pUARTRegs->RFC; // bytes available
//
//  nBytes = MXC_UART_GetRXFIFOAvailable(_uart.mxc_uart);
//
//  // sanity check: are there any bytes waiting to be drained?
//  if(nBytes == 0) return; // not sure how we got here if that's the case, but time to get out
//
//
//  if(_uart.ovrBytes + nBytes >= _uart.ovrSize) while(1); // sanity check: would draining these bytes overflow our buffer? trap here for debug
//
//  // actually drain the RX FIFO into our circular buffer
//  for(int i=0; i<nBytes; i++){
//	  //MXC_UART_ReadRXFIFO();
//	  c_uart_inst_uart_ovr_push(MXC_UART_ReadCharacter(_uart.mxc_uart), 1);
//  }
//
//  MXC_UART_ClearFlags(_uart.mxc_uart, 0xFFFFFFFFu); // clear ALL possible flags
//  NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(ADUP_UART));
}

uint16_t c_uart_inst_uart_ovr_drain(uint8_t *const buf, uint32_t nbytes){
  if(_uart.ovrBytes == 0)
    return 0;
  if(nbytes == 0)
    return 0;
  
  // disable RX IRQ
  //	hUartDev->pUARTRegs->IEN &= (uint16_t)~(BITM_UART_IEN_ERBFI);
  
  uint32_t bytesToReturn = nbytes; // plan to return ALL requested bytes
  if(nbytes > _uart.ovrBytes)
    bytesToReturn = _uart.ovrBytes; // but only if we have enough data here
  
  for(int i=0; i<bytesToReturn; i++){
    if(_uart.ovrBytes==0)
      while(1); // no error handling yet if we've drained it all already
    
    buf[i] = _uart.ovrBuf[_uart.ovrIdxL]; // drain a byte and adjust the ovrIdxL pointer
    
    _uart.ovrBytes--; // reduce the byte count in the
    
    _uart.ovrIdxL++;
    if(_uart.ovrIdxL == _uart.ovrSize)
      _uart.ovrIdxL = 0; // wrap the index pointer around
  }
  
  // enable RX IRQ
  //	hUartDev->pUARTRegs->IEN |= (uint16_t)(BITM_UART_IEN_ERBFI);
  if(true){
	  printf("drained [%d] bytes from ADUP circular buffer\n", bytesToReturn);
  }
  return bytesToReturn;
}

bool c_uart_inst_uart_ovr_cmp(char *buf, uint16_t len){
  uint16_t idx = _uart.ovrIdxL;
  
  if(len > _uart.ovrBytes){
    return false;
  }
  else {
    for(int i=0; i<len; i++){
      if(buf[i] != _uart.ovrBuf[idx]){
        return false;
      }
      idx++;
      if(idx == _uart.ovrSize){
        idx=0;
      }
    }
  }
  
  return true;
}

void c_uart_inst_uart_read(void *const buf, unsigned int const size){
  uint8_t *lbuf = buf;
  unsigned int size_tmp = 0;
  
  while(c_uart_inst_uart_GetRxFifo() < size); // wait for our cb func to collect enough data, never use the driver's functions
  size_tmp = _uart.uart_ovr_drain(lbuf, size);
}

void c_uart_inst_uart_write(void *const buf, unsigned int const size){
  uint8_t *lbuf = buf;
//  uint32_t err = 0;
//  bool complete = false;
  // TODO: write data out via UART
  // adi_uart_Write(hUartDev, lbuf, size, UART_USE_DMA, &err);

  unsigned int remain = size;
  int curTX_sz = 0;
  int curTX_offset = 0;

  while(writePending); // wait for any old TXfers to finish?

  writePending=true; // set flag telling readHandler not to restart its xfer
  MXC_UART_AbortAsync(_uart.mxc_uart); // abort ongoing async RX

#if 1
  while(remain>0){
	  if(remain >= UART_BUF_SZ) curTX_sz=UART_BUF_SZ;
	  else curTX_sz = remain;

	  // setup the write_req
	  write_req.txLen=curTX_sz;
	  write_req.txData=&(buf[curTX_offset]);
	  write_req.callback = NULL;

	  writePending=true;
	  if(_uart.mxc_uart->status & 0xF000){
		  // TX fifo isn't empty
		  //printf("trying to send new data but TX fifo still not empty\n");
		  while(_uart.mxc_uart->status & 0xF000); // witing till it IS empty
	  }
	  MXC_UART_Transaction(&write_req); // blocking TX

	  //while(writePending); // wait till the async TX is completed

	  // adjust the offsets and counters
	  curTX_offset+=curTX_sz;
	  remain-=curTX_sz;
  }
#else
  // setup the write_req
  write_req.txLen=1;
  write_req.txData=buf;
  write_req.callback = NULL;

  printf("about to send [%d] bytes\n", remain);

  while(remain>0){
	  MXC_UART_Transaction(&write_req); // blocking TX
	  write_req.txData+=1; // uint8_t ptr increment
	  remain--;
	  printf(".");
  }
  printf("\n");
  printf("TX complete\n");

//  printf("waiting for writeCallback to execute\n");
//  while(writePending); // wait till the async TX is completed
#endif

  writePending=false;
  MXC_UART_TransactionAsync(&read_req); // restarting RX flow

}

void c_uart_inst_uart_resync(){
  char buf[5];
  char prebuf[2]="!";
  while(strcmp(prebuf, "*")!=0){
    _uart.uart_read(prebuf, 1); // drain the incoming data until i see an 'S'
    //prebuf[1]='\0';
  }
  
  _uart.uart_write(prebuf, 1); // echo the prebuf back
  
  _uart.uart_read(buf, 4); // then hunt for 3 more characters
  buf[4]='\0';
  if(strcmp(buf, "SYNC")!=0){
    while(1); // die here; we failed to resync properly
  }
  
  _uart.uart_write(buf, 4);
}

void c_uart_inst_uart_RxChanFlush(){
  //adi_uart_FlushRxFifo(hUartDev);
  uint8_t jnk[1];
  
  // TODO: consider flushing the RX FIFO (TX should drain itself quickly) in hardware

#if 1 // this works even if we are still RX'ing data while clearing...
  // clear out the overflow buffer
  while(_uart.ovrBytes > 0)
    _uart.uart_ovr_drain(jnk, 1);
#else // this only works if the uart doesn't service anymore RX events while i'm clearing. a tiny race condition, but extant
  _uart.ovrIdxL = _uart.ovrIdxH;
  _uart.ovrBytes = 0;
#endif
  
  //	adi_uart_FlushRxChannel(hUartDev);
}

bool c_uart_inst_uart_checkFifo(){
  if(_uart.uart_GetRxFifo() >= 3)
    return true;
  else
    return false;
}

uint16_t c_uart_inst_uart_GetRxFifo(){
  uint16_t ret=0;
  ret+=_uart.ovrBytes;
  return ret;
}

uint16_t c_uart_inst_uart_GetTxFifo(){
  uint16_t ret;

  ret = 0; // TODO: read the TX fifo size; this method may be deprecated? in TX we wait for completion already...
  /*
  if(hUartDev->pChannelTx->pActiveBuffer->bInUse){
    ret += hUartDev->pChannelTx->pActiveBuffer->nCount;
    ret -= hUartDev->pChannelTx->pActiveBuffer->nIndex;
  }
  */

  return ret;
}

/** commenting out all autobaud implementation below: this is ADuCM3/4 specific stuff **/
//
//void c_uart_inst_adup_manualBaud(uint16_t nDivC, uint8_t nDivM,	uint16_t nDivN,	uint8_t nOSR){
//  adi_uart_ConfigBaudRate(
//                          hUartDev,
//                          nDivC,
//                          nDivM,
//                          nDivN,
//                          nOSR
//                            );
//}
//
//void c_uart_inst_adup_deautobaud(){
//  /* Disable autobaud. */
//  if(adi_uart_EnableAutobaud(hUartDev, false, false) != ADI_UART_SUCCESS) {
//    DEBUG_MESSAGE("Failed to disable autobaud");
//  }
//}
//
//void c_uart_inst_adup_autobaudStart(void){
//  //adup_deautobaud(); // kill any prior start of autobaud
//
//  /* Enable autobaud not using the callback. */
//  if(adi_uart_EnableAutobaud(hUartDev, true, false) != ADI_UART_SUCCESS) {
//    DEBUG_MESSAGE("Failed to enable autobaud");
//  }
//}
//
//uint32_t c_uart_inst_adup_autobaudTest(void){
//  static uint32_t nBaudrate;
//  uint32_t nAutobaudError;
//
//  adi_uart_GetBaudRate(hUartDev, &nBaudrate, &nAutobaudError);
//
//  //	if(nAutobaudError != ADI_UART_AUTOBAUD_NO_ERROR)
//  //		return -1;
//  //	else
//  return nBaudrate;
//}
//
//void c_uart_inst_adup_autobaudEnd(void){
//  char jnk[3];
//  uint32_t err;
//  // PC will send 3x autobaud characters at startup.
//  // this is so if PC sends autobaud when MCU is already alive and can RX 3 bytes, recognize 3x autobaud characters, and ignore them
//  // drain the extra 2 autobaud characters now
//
//  // reading directly from the driver? yikes
//  // adi_uart_Read(hUartDev, jnk, 2, UART_USE_DMA, &err);
//  strcpy(jnk, "\r\r"); // TODO: read two chars from the UART RX channel; these are expected to be \r's
//  //	uart_read(jnk, 2);
//  _uart.uart_write("OK", 2);
//
//  // turn off the autobaud feature
//  //	adup_deautobaud();
//}
//
//void c_uart_inst_adup_autobaud(void){
//  static uint32_t nBaudrate;
//  uint32_t nAutobaudError;
//  char jnk[3];
//#if 1
//  /* Enable autobaud using the callback. */
//  if(adi_uart_EnableAutobaud(hUartDev, true, false) != ADI_UART_SUCCESS)
//  {
//    DEBUG_MESSAGE("Failed to enable autobaud");
//    //bFailure = true;
//    //break;
//  }
//#endif
//
//  /* Wait until the baudrate has been detected. */
//  do
//  {
//    adi_uart_GetBaudRate(hUartDev, &nBaudrate, &nAutobaudError);
//    //nAutobaudCounter++;
//
//  }while((nBaudrate == 0));// && (nAutobaudCounter < UART_AUTOBAUD_TIMEOUT));
//
//  _uart.uart_write("OK", 2);
//
//  // PC will send 3x autobaud characters at startup.
//  // this is so if PC sends autobaud when MCU is already alive and can RX 3 bytes, recognize 3x autobaud characters, and ignore them
//  // drain the extra 2 autobaud characters now
//  _uart.uart_read(jnk, 2);
//}

int c_uart_inst_adup_uart_init(void)
{
  //	UartDeviceMem = malloc(sizeof(uint8_t)*ADI_UART_MEMORY_SIZE);
  // this buffer is used to pull in data from the UART RX FIFO when data arrives without a uart_read() running
  _uart.ovrBytes=0; // num bytes valid in the buffer
  _uart.ovrSize=OVR_BUF_ALLOC_BYTES;
  _uart.ovrIdxL=0;
  _uart.ovrIdxH=0;
  
  _uart.mxc_uart = MXC_UART_GET_UART(ADUP_UART);

  /* Variable that keeps track of any errors found. */
  bool bResult = true;
  
#if UART_USE_DMA
    MXC_DMA_ReleaseChannel(0);
    NVIC_SetVector(DMA0_IRQn, DMA_Handler);
    NVIC_EnableIRQ(DMA0_IRQn);
#else
    NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(ADUP_UART));
    NVIC_DisableIRQ(MXC_UART_GET_IRQ(ADUP_UART));
    NVIC_SetVector(MXC_UART_GET_IRQ(ADUP_UART), UART_Handler);
    NVIC_EnableIRQ(MXC_UART_GET_IRQ(ADUP_UART));
#endif

    int error;
    // Initialize the UART
    if((error = MXC_UART_Init(_uart.mxc_uart, _uart._baud, MXC_UART_IBRO_CLK/*MXC_UART_APB_CLK*/)) != E_NO_ERROR) {
        printf("-->Error initializing UART: %d\n", error);
        printf("-->Example Failed\n");
        bResult = false;
        while (1) {}
    }

    MXC_UART_SetClockSource(_uart.mxc_uart, MXC_UART_IBRO_CLK);
    MXC_UART_SetDataSize(_uart.mxc_uart, 8);
    //MXC_UART_SetFlowCtrl(_uart.mxc_uart, MXC_UART_FLOW_DIS, 1);
    MXC_UART_SetFrequency(_uart.mxc_uart, 115200, MXC_UART_IBRO_CLK);
    MXC_UART_SetParity(_uart.mxc_uart, MXC_UART_PARITY_DISABLE);
    MXC_UART_SetRXThreshold(_uart.mxc_uart, 1);
    MXC_UART_SetTXThreshold(_uart.mxc_uart, 1);
    MXC_UART_SetStopBits(_uart.mxc_uart, MXC_UART_STOP_1);



    read_req.uart = MXC_UART_GET_UART(ADUP_UART);
    read_req.rxData = rxbuf;
    read_req.rxLen = 1; // UART_BUF_SZ; // we want 1 byte
    read_req.txLen = 0;
    read_req.callback = readCallback;

    write_req.uart = MXC_UART_GET_UART(ADUP_UART);
    write_req.txData = NULL;
    write_req.txLen = 0; // unknown how large in truth
    write_req.rxLen = 0;
    write_req.callback = writeCallback;
//
//    READ_FLAG = 1;
//    DMA_FLAG = 1;

    MXC_UART_ClearRXFIFO(MXC_UART_GET_UART(ADUP_UART));

#ifdef DMA
    error = MXC_UART_TransactionDMA(&read_req);
#else
    error = MXC_UART_TransactionAsync(&read_req);
#endif

    // sit here and wait for three carriage returns
    uint8_t tmp[3]="AA"; // includes null terminator
    uint8_t ch;
    bool syncing=true;
    volatile int idx=0;
    while(syncing){
    	if(c_uart_inst_uart_ovr_drain(&tmp[idx], 1)>0){
//			printf("[%c]\n", tmp[idx]);
//			scanf("%c", &ch);
//			printf("{%c}\n", ch);
			syncing=false;
			for(int i=0; i<3; i++){
				if(tmp[i]!='\r') syncing=true;
			}
			idx++;
			if(idx>2) idx=0;
    	}
    }
    char resp[3]="OK";
    c_uart_inst_uart_write(resp, 2);
    printf("adup sync complete\n", tmp);

  if(bResult)
    return(0);
  else
    return 1;
}
