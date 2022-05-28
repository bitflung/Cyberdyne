from lib_comms.lib_APP_MSG import *
from lib_comms.lib_ADUP import *

class ApplicationComms():
    def __init__(self, adup):
        self._appmsg = APP_MSG("dblock")
        self._adup = adup
		
    def read(self, adr):
        self._appmsg.setCmd('R')
        self._appmsg.setPayload("")
        self._appmsg.setType("RR")
        self._appmsg.setAdrVal(adr)
        self._appmsg.setIdVal(self._appmsg.id()+1) # increment the ID field

        msg = self._appmsg.getMsg()
        self._adup.TX(msg)
        resp = self._adup.RX(msg.cmd())

        self._appmsg.parseMsg(resp)
        dat = []

        if self._appmsg.getType() == "OK":
            str = self._appmsg.getPayload()
            plen = len(str)

            for i in range(0, plen, 8):
                # for each word in payload
                current_word = str[i:i+8]
                dat.append(int(current_word, 16))
            return dat
        else:
            print("ERROR: MCU response indicates failed transaction")
            print(self._appmsg.dump())
            return dat

    def write(self, adr, dat):
        """
        :param adr: 32 bit starting address (int)
        :param dat: array of 32 bit values (ints)
        :return: True on success, False on fail
        """
        self._appmsg.setCmd('W')
        self._appmsg.setPayload("")
        self._appmsg.setType("WW")
        self._appmsg.setAdrVal(adr)
        self._appmsg.setIdVal(self._appmsg.id() + 1)  # increment the ID field

        for d in dat:
            self._appmsg.appendPayloadVal(d)

        msg = self._appmsg.getMsg()
        self._adup.TX(msg)
        resp = self._adup.RX(msg.cmd())
        self._appmsg.parseMsg(resp)


        if self._appmsg.getType() == "OK":
            # optional: check the return payload to see if it matches the value sent
            return True

        else:
            print("ERROR: MCU response indicates failed transaction")
            print(self._appmsg.dump())
            return False

    def erase(self, adr):
        """
        :param adr: 32 bit target address
        :return:
        """
        self._appmsg.setCmd('E')
        self._appmsg.setPayload("")
        self._appmsg.setType("EE")
        self._appmsg.setAdrVal(adr)
        self._appmsg.setIdVal(self._appmsg.id() + 1)  # increment the ID field

        msg = self._appmsg.getMsg()
        self._adup.TX(msg)
        resp = self._adup.RX(msg.cmd())
        self._appmsg.parseMsg(resp)


        if self._appmsg.getType() == "OK":
            # optional: check the return payload to see if it matches the value sent
            return True

        else:
            print("ERROR: MCU response indicates failed transaction")
            print(self._appmsg.dump())
            return False