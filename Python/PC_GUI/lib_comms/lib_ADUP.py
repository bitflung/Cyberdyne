import serial
import serial.tools.list_ports
import sys
from lib_comms.lib_MSG import *
from random import choice
from string import ascii_letters, digits
import time

# datetime is used only for profiling the automated testcase
from datetime import datetime

#####
# Public API:
#   getPorts(): returns the name of the only available serial port in the system
#   serOpen(name): open 9600bps connection to the named port
#   serDebug(en): en/dis-able the serial debug trace
#   serInit(maxPayload): re-init the protocol and set a new payload-MTU (num chars in string)
#   serGetMaxLen(): returns the current protocol payload-MTU (default:8)
#   serQuit(errcode): runs serInit(8) and returns errcode to waitExit(errcode)
#
#   RxProtHandler(expcmd): receives multi-packet data and returns as a contiguous string; verifies RX'd cmd matches expcmd
#   TxProtHandler(cmd, val): breaks val into multiple packets and sends with a CMD identifier
#
#   protTest(): debug and test function: iterates over message and payload lengths, sends payloads to MCU and expects mirror response
############################################


class ADUP():

    def __init__(self):
        self.protDebug = 0
        self.pauseBeforeExit = 0
        self.DEFAULT_MAX_PROT_PAYLOAD = 8
        self.mcuStrBufSize = 12000
        self._name = ""
        self.PROT_MAX_PAYLOAD = self.DEFAULT_MAX_PROT_PAYLOAD
        self.listenPending = False
        self._cb = None
        self._debugCB = None
        
    def regDebugCB(self, debugCB):
        self._debugCB = debugCB
        
    def name(self):
        return self._name

    def waitExit(self, ret):
        if (self.pauseBeforeExit == 1):
            print("[press enter to exit]");
            wait = input();
        sys.exit(ret);

    def getAllPorts(self):
        return self.getMatchPorts("")

    def getMatchPorts(self, matchString):
        ports = list(serial.tools.list_ports.comports())
        pnames=[]
        for p in ports:
            check=False

            if matchString in str(p):
                check=True

            if check:
                print("\t" + str(p))
                names = (str(p)).split();
                name = names[0];
                pnames.append(name)

        return pnames

    def getPorts(self):
        ports = list(serial.tools.list_ports.comports())
        if len(ports) > 1:
            print("too many ports in system; edit script to pick one");
            for p in ports:
                print("\t"+str(p))
            #self.waitExit(1);
        if len(ports) < 1:
            print("no serial ports detected; is the board plugged in?");
            #self.waitExit(1);
        else:
            names = (str(ports[0])).split();
            name = names[0];
            print("Identified target port: " + name);
            return name;

    def serDebug(self, val):
        self.protDebug = val;
        return;

    def serOpen(self, name, baud):
        self._name = name
        print("opening " + name + " at " + str(baud) + " baud");
        self.ser = serial.Serial(name, baud, timeout=None);
        print("port opened");
        self.PROT_MAX_PAYLOAD = self.DEFAULT_MAX_PROT_PAYLOAD;
        self.BAUDRATE = baud;
        return self.ser;

    def serRxFifoCnt(self):
        return self.ser.in_waiting

    def serClose(self):
        self.ser.close();
        return;

    def cmd_quit(self):
        qmsg = MSG("quit")
        qmsg.setCmd('Q')
        self.TX(qmsg)

    def cmd_listen(self):
        lmsg=MSG("listen")
        lmsg.setCmd('L')
        self.TX(lmsg)

    def cmd_echo(self, payload):
        emsg = MSG("echo")
        emsg.setCmd('#')
        emsg.setPayload(payload)
        self.TX(emsg)
        ret = self.RX(emsg.cmd())
        return ret

    def autobaud(self):
        print("attempting autobaud...")
        # clear any old data
        while self.serRxFifoCnt() > 0:
            self.ser.read(1)

        # now send carriage returns until we get "OK"
        loop = True

        while loop:
            if self.serRxFifoCnt() == 0:
                self.serWrite('\r')
                time.sleep(0.1)

            else:
                # if we have data in the RX fifo
                while self.serRxFifoCnt() < 2:
                    # make sure we have 2 bytes
                    time.sleep(0.1)

                resp = self.serRead(2)

                if "OK" in resp:
                    print("autobaud success")
                    loop = False
                else:
                    print("autobaud fail ["+resp+"]")

        return True

    def serWrite(self, dout):
        self.ser.write(dout.encode());
        # ser.flush(); # flush is bad...
        if (self.protDebug > 0):
            print("\tser.write(" + dout + ")");
        return;

    def serRead(self, num):
        ret = (self.ser.read(num).decode());
        if (self.protDebug > 0):
            print("\t\tser.read(" + str(num) + ") -> [" + ret + "]");

        if len(ret) == 0:
            print("ERROR: serial read timeout");
            self.waitExit(1);

        return ret;

    def serQuit(self, ret):
        # global DEFAULT_MAX_PROT_PAYLOAD;
        # serInit(PROT_MAX_PAYLOAD);
        self.TxProtHandler('Q', "", False);
        self.waitExit(ret);
        return;

    def serInit(self, length):
        if (length > 254):
            print("ERROR: protocol MAX payload is 254");
            self.serQuit(1);
        elif (length < 2):
            print("ERROR: protocol MIN payload is 2");
            self.serQuit(1);
        else:
            # try to quit operations till bus is silent
            # resetPass=3;
            # while (resetPass>0):
            #    resetPass=TxProtHandler('Q', "");

            # send new protocol payload size in hex (2 chars)
            payload = hex(length)[2:];
            # if(len(payload) < 2):
            #    payload = "0"+payload

            # print("U"+payload);
            self.TxProtHandler('U', payload, True);

            # retain new protocol size locally
            self.PROT_MAX_PAYLOAD = length;
            print("protocol info: Initialized with max packetSize=" + str(length));


    def serGetMaxLen(self):
        return self.PROT_MAX_PAYLOAD;


    ######################################################

    def getHeader(self):
        ack = self.serRead(1);
        rawlen = self.serRead(2);
        acklen = int(rawlen, 16);
        return ack, acklen;

    def putHeader(self, cmd, length):
        self.serWrite(cmd + '{:02x}'.format(length));
        return;

    def protError(self, msg):
        print(msg)
        self.serDebug(0)
        string = "Error: ["
        time.sleep(3)
        while self.serRxFifoCnt()  > 0:
            string = string + self.serRead(1)
            #print(str(self.serRead(1)))
        string = string + "]"
        print(string)
        self.serQuit(1)
        return

    def verAck(self, expcmd, explen):
        ret = 0

        # get the ACK header
        ack, acklen = self.getHeader()

        # if we are NOT waiting on an ACK to a Listen command, then watch for a Listen command coming in
        if not "L" in expcmd:
            if ("L" in ack):
                # this is a listen command, perhaps injected at a time we were not expecting
                self.listenPending = True
                return verAck(expcmd, explen) # recursively call verAck again to fetch the right header
        
        if (ack != expcmd):
            self.protError("ERROR: bad ack/cmd [" + ack + "/" + expcmd + "]");
            ret = 1  # error 1: bad header.ack
        if (acklen != explen):
            self.protError("ERROR: bad len/expected [" + str(acklen) + "/" + explen + "]");
            ret = 2  # error 2: bad header.length

        return ret  # error 0: no error

    def RX(self, cmd='#'):
        msg = MSG("rx_msg")
        cmd, payload = self.RxProtHandler(True)
        msg.setPayload(payload)
        msg.setCmd(cmd)
        return msg

    def TSOP(self, cmd='#'):
        msg = MSG("tsop_msg")
        cmd, payload = self.RxProtHandler(False)
        msg.setPayload(payload)
        msg.setCmd(cmd)
        return msg
    
    def RxProtHandler(self, doAcks=True):
        ret = ""
        retaccum = ""
        getLen = self.PROT_MAX_PAYLOAD + 1
        getCmd = ""
        expcmd = ""
        
        while (getLen > self.PROT_MAX_PAYLOAD):
            getCmd, tmpLen = self.getHeader();
            
            if not expcmd:
                expcmd = getCmd
                
            getLen = tmpLen;

            if (getCmd == "!"):
                print("RX'd error header from MCU!");
                self.serQuit(1);

            if (getCmd == "D"):
                debugStr = str(self.serRead(getLen));
                if(self._debugCB):
                    self._debugCB(debugStr)
                else:
                    print("DEBUG MSG: "+debugStr)
            else:
                if (getCmd != expcmd):
                    print("ERROR in RxProtHandler: unexpected response header");
                    self.serQuit(1);

                if (getLen > self.PROT_MAX_PAYLOAD):
                    ret = str(self.serRead(self.PROT_MAX_PAYLOAD));
                else:
                    if getLen > 0:
                        ret = str(self.serRead(getLen));

                if(doAcks):
                    #print("sending packet ACK")
                    self.putHeader(getCmd, 0);
                    
                retaccum = retaccum + ret;  # accumulate the total return string to pass within python

        return expcmd, retaccum;

    def TX(self, msg):
        self.TxProtHandler(msg.cmd(), msg.payload(), True)

    def POST(self, msg):
        self.TxProtHandler(msg.cmd(), msg.payload(), False)
        
    def TxProtHandler(self, cmd, payload, useAcks):
        length = len(payload);
        ack = "";
        acklen = self.PROT_MAX_PAYLOAD + 1;

        if (length > self.mcuStrBufSize):
            print("WARNING: attempting to send a payload that exceeds the defined MCU buffer size [mcuStrBufSize]");

        if (length == 0):
            self.putHeader(cmd, 0);
            ret = self.verAck(cmd, 0);  # verify ACK from MCU
        else:
            while (length > 0):
                if (length > self.PROT_MAX_PAYLOAD):
                    serLen = self.PROT_MAX_PAYLOAD + 1;  # char put into serial transaction
                    putLen = self.PROT_MAX_PAYLOAD;
                else:
                    serLen = length;
                    putLen = length;

                # common serial write call
                self.putHeader(cmd, serLen);
                self.serWrite(payload[:putLen]);  # write the payload

                payload = payload[putLen:];  # modify the payload for next iteration
                length = len(payload);  # get next payload length

                if useAcks:
                    ret = self.verAck(cmd, 0);  # verify ACK from MCU
                    if (ret != 0):
                        length = 0;  # abort sending on a bad ack

        return ret;


    def listen(self):
        done = False
        while not done:
            done = self.listen(False)

    def listen(self, single):
        done = single        
        _msg = self.RX()
        # TODO: check for deadcomms here and return TRUE if dead
        
        _handled = False
        
        if(_msg.cmd() == "L"):
            done = False
            _handled = True
        if(_msg.cmd() == "Q"):
            done = True
            _handled = True
        if(_msg.cmd() == "U"):
            self.update(_msg)
            _handled = True
        if(_msg.cmd() == "#"):
            self.echo(_msg)
            _handled = True

        if not _handled:
            if self._cb != None:
                _handled = True
                self._cb(_msg)
        
        return done

    def conditionalListen(self):
        if self.listenPending:
            self.listenPending = False
            _msg = MSG("listen ack")
            _msg.setPayload("")
            _msg.setCmd("L")
            self.POST(_msg)
            self.listen()
        else:
            # if we have 3 or more bytes waiting in the UART fifo
            if self.serRxFifoCnt() >= 3:
                self.listen(True) # call on 1 round of self.listen

    def reg_callback(self, handler):
        self._cb = handler
    
    #################################################

    #####
    # used only for testing the protocol
    ##

    def timedXfer(self, payload, packetSize, maxBuf):
        # TX and expect echo RX of that payload 10 times
        startTime = datetime.now();
        sys.stdout.write("\t");
        for k in range(0, 10):
            # print("\t\tTest[%d]: packetSize[%d] msgSize[%d]" % (k, packetSize, len(payload)));
            sys.stdout.write('.');

            ret = self.TxProtHandler("#", payload);
            if (ret != 0):
                print("\nERROR with payload size [" + str(i) + "] during TX");
                self.waitExit(1);

            # print("\tTX'd ["+str(len(payload))+"] chars");

            rxdat = self.RxProtHandler("#");
            if (rxdat != payload):
                print("\nERROR with payload size [" + str(i) + "] during RX");
                self.waitExit(1);

            # print("\tRX'd ["+str(len(rxdat))+"] chars");

        sys.stdout.write('\n');
        endTime = datetime.now();
        delta = endTime - startTime;
        totalChars = (maxBuf * 10 * 2);  # 10 iterations of 2-way transfers

        durTime = delta.total_seconds();
        charTime = durTime / totalChars;
        charsPerSec = (1 / charTime);
        return (durTime, totalChars, charsPerSec);


    def protTest(self, baud):
        comport = self.getPorts()
        self.protTest(comport, baud)


    def protTest(self, comport, baud):
        print("\tbegin automated test");
        self.serOpen(comport, baud);
        #self.serDebug(1);  # 0/1 dis/en-able

        for packetSize in range(54, 255, 20):  # MIN:2, MAX:254
            print("\nTesting with packet size [" + str(packetSize) + "]");
            self.serInit(packetSize);
            payload = "";

            if packetSize < 200:
                maxBuf = 256;  # mcuStrBufSize;
            else:
                maxBuf = self.mcuStrBufSize;
                print("*** Following test REQUIRES non-default linker script on MCU");

            for payloadSize in range(0, maxBuf):
                payload = payload + choice(ascii_letters + digits);  # add one char on each iteration

            print("payload [" + str(payloadSize) + "] chars: " + payload[:16] + " ... ");
            print("\ttesting 10 iterations of full payload echo");

            # run timed transfer
            (durTime, totalChars, charsPerSec) = self.timedXfer(payload, packetSize, maxBuf);
            bitPerSec = charsPerSec * 8;
            print("\tTest duration: " + str(durTime) + " seconds");
            print("\tTotal chars: " + str(totalChars));
            print("\tEffective throughput = [" + str(bitPerSec)[:9] + " bits/second]");
            print("\tEfficiency [@ " + str(self.BAUDRATE) + " baud]: " + str((bitPerSec * 100) / self.BAUDRATE)[:4] + "%");
            print("-----------------------------");

        print("\tend of automated test: PASSED");
