import math
from lib_comms.lib_MSG import *


class APP_MSG():
    def __init__(self, name):
        self._name = name

        # demo block msg specific stuff
        self.purge()

    def setIdVal(self, id):
        self._id=id
        if self._id > 255:
            self._id = 0

    def id(self):
        return self._id

    def setType(self, type):
       self._type=type

    def getType(self):
        return self._type

    def setCmd(self, cmd):
        self._cmd=cmd

    def setAdr(self, adr):
        self._adr=adr

    def setAdrVal(self, adr):
        str = '{:08x}'.format(adr)
        self.setAdr(str)

    def setLen(self, len):
        self._len=len

    def setPayload(self, payload):
        self._payload=payload
        self.setLen(len(payload))

    def setPayloadVal(self, val):
        str = '{:08x}'.format(val)
        self.setPayload(str)

    def appendPayload(self, payload):
        newpayload = self._payload + payload
        self.setPayload(newpayload)

    def appendPayloadVal(self, val):
        str = '{:08x}'.format(val)
        self.appendPayload(str)

    def getPayload(self):
        return self._payload

    def purge(self):
        self.setIdVal(0)       # optional packet counter
        self.setType('\0')  # 'R?': read, 'W?': wwrite, 'E?': erase
        self.setCmd('\0')   # optional, can be used to select between read/write modes for types that are bidirectional
        self.setAdr(0)      # 32-bit address will be represented as 8-char HEX string
        self.setLen(0)      # payload size in bytes, will be represented as 2-char HEX string
        self.setPayload("") # payload expected to be integer number of 8-char words (32-bit values in HEX strings for flash read/write ops)

    def parseMsg(self, msg):
        # parse a given MSG object's payload to extract parameters
        self.purge()

        buf=msg.payload()

        self.setIdVal(int(buf[0:2], 16))
        self.setType(buf[2:4])
        self.setCmd(buf[4:5])
        self.setAdrVal(int(buf[5:13], 16))

        expected_len = int(buf[13:15], 16)
        buf2 = buf[15:] # extracted payload is from char 15 to the end of the buffer

        # payload should be an integer number of 8-character words
        words = math.floor(self._len / 8)
        chars = words * 8
        if self._len == chars:
            self.setPayload(buf2)
        else:
            print("ERROR: message payload does not contain integer multiple of 32-bit words")
            print("ignoring payload: ["+buf2+"]")
            self.setPayload(buf2)

        return

    def getMsg(self):
        # populate local fields into the PAYLOAD space of a MSG() object
        buf=""
        buf = buf + '{:02x}'.format(self._id) # 2 character ID field
        buf = buf + self._type[0:2] # 2 character TYPE text field
        buf = buf + self._cmd[0:1]  # 1 character CMD field
        buf = buf + self._adr[0:8]  # 8 character ADR field
        buf = buf + '{:02x}'.format(self._len)  # 2 character LEN field
        buf = buf + self._payload  # N character PAYLOAD field

        msg = MSG(self._name)
        msg.setCmd('A')
        msg.setPayload(buf)

        return msg

    def dump(self):
        ret=""
        ret = ret + "[" + self._name + "]\n"
        ret = ret + "\tID   :\t" + '{:02x}'.format(self._id) + "\n"
        ret = ret + "\tTYPE :\t" + self._type + "\n"
        ret = ret + "\tCMD  :\t" + self._cmd + "\n"
        ret = ret + "\tADR  :\t" + self._adr + "\n"
        ret = ret + "\tLEN  :\t" + '{:02x}'.format(self._len) + "\n"
        ret = ret + "\tPLOAD:\t" + self._payload + "\n"
        return ret