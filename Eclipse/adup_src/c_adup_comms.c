
#include "c_adup_comms.h"
//#include "uart_inst.h"
C_COMMS _comms;
extern C_UART_INST _uart;

C_COMMS* C_COMMS_init(int baud){
  // functions
  
  _comms.checkFifo = &c_comms_checkFifo;
  _comms.headerPeek = &c_comms_headerPeek;
  
  _comms.getString = &c_comms_getString;
  _comms.putString = &c_comms_putString;
  
  _comms.resync = &c_comms_resync;
  _comms.flush = &c_comms_flush;
  
//  _comms.autobaudStart = &c_comms_autobaudStart;
//  _comms.autobaudTest = &c_comms_autobaudTest;
//  _comms.autobaudEnd = &c_comms_autobaudEnd;
//
//  _comms.setBaud = &c_comms_setBaud;
  _comms._uart = C_UART_INST_init(UART_BAUD);
  // fields
  _comms.txByteCnt = 0;
  _comms.txTimeStamp = 0;
  
  // function calls
//  _comms.setBaud(baud);
  
  return &_comms;
}

bool c_comms_headerPeek(void){
  return _uart.uart_ovr_cmp("\r\r\r", 3);
}

//bool c_comms_checkFifo(void){
//	return uart_checkFifo();
//}

bool c_comms_checkFifo(uint32_t expected){
  if(expected > _uart.uart_GetRxFifo())
    return false;
  else
    return true;
}

void c_comms_getString(char* line, unsigned int len){
	_uart.uart_read(line, len);
  line[len]='\0'; // sanitize the payload
  _comms.txByteCnt=0;
}

void c_comms_putString(char* line, unsigned int len){
  // if this is the first TX since someone reset the byte count, then we are clear
  // likely was reset by getString()
#if TX_DELAY_FLOW_CTL
  double newTimeStamp;
  newTimeStamp = (double)GET_TIME();
  if(_comms.txByteCnt == 0)
    _comms.txTimeStamp = newTimeStamp;
  
  _comms.txByteCnt+=len;
  
  if(newTimeStamp - _comms.txTimeStamp >= TX_BYTE_TIM_PERIOD){
    _comms.txTimeStamp = newTimeStamp;
    _comms.txByteCnt=len;
  }
  else {
    // we are still inside the period in which we should throttle TX depth
    if(_comms.txByteCnt > TX_BYTE_CNT_PERIOD){
      // and we are now exceeding the threshold where we ought to slow things down
      do{
        newTimeStamp = (double)GET_TIME();
      } while(newTimeStamp - _comms.txTimeStamp < TX_BYTE_TIM_PERIOD);
      
      // we waited long enough, now clear everything
      _comms.txByteCnt = len;
      _comms.txTimeStamp = newTimeStamp;
    }
  }
#endif
  
  _uart.uart_write(line, len);
}

void c_comms_flush(){
	_uart.uart_RxChanFlush();
}
void c_comms_resync(){
	_uart.uart_resync();
  //adup_deautobaud();
  //adup_autobaud();
}

//void c_comms_autobaudStart(void){
//  adup_autobaudStart();
//}
//
//uint32_t c_comms_autobaudTest(void){
//  return adup_autobaudTest();
//}
//
//void c_comms_autobaudEnd(void){
//  adup_autobaudEnd();
//}

//void c_comms_setBaud(int baud){
//  // EVERYTHING HERE assumes a 26MHz PCLK
//
//  // default values represent 921,600 baud
//  uint16_t           nDivC = 1;
//  uint8_t            nDivM = 1;
//  uint16_t           nDivN = 1563;
//  uint8_t            nOSR = 2;
//
//  if(baud<0){
//    return;
//  }
//  else if(baud==0) {
//    adup_autobaud();
//  }
//  else{
//    //		adup_deautobaud(); // disable autobaud functionality
//
//    /* handle some specific baud rates
//    Table 21-2: Baud Rate Examples Based on 26 MHz PCLK
//    Baud Rate OSR COMDIV DIVM DIVN
//    9600        3 24     3    1078
//    57600       3 4      3    1078
//    115200      3 4      1    1563
//    921,600     2 1      1    1563
//    1,000,000   2 1      1    1280
//    1,500,000   2 1      1    171
//    */
//    switch(baud){
//      case(9600):
//        nOSR = 3;
//        nDivC = 24;
//        nDivM = 3;
//        nDivN = 1078;
//        break;
//        case(57600):
//          nOSR = 3;
//          nDivC = 4;
//          nDivM = 3;
//          nDivN = 1078;
//          break;
//          case(115200):
//            nOSR = 3;
//            nDivC = 4;
//            nDivM = 1;
//            nDivN = 1563;
//            break;
//            case(921600):
//              nOSR = 2;
//              nDivC = 1;
//              nDivM = 1;
//              nDivN = 1563;
//              break;
//              case(1000000):
//                nOSR = 2;
//                nDivC = 1;
//                nDivM = 1;
//                nDivN = 1280;
//                break;
//		case(1500000):
//                  nOSR = 2;
//                  nDivC = 1;
//                  nDivM = 1;
//                  nDivN = 171;
//                  break;
//    default:
//      while(1); // die here for unsupported baud rates
//      break;
//    }
//
//    adup_manualBaud(nDivC, nDivM, nDivN, nOSR);
//
//
//  }
//}
