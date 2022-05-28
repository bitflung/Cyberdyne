from lib_comms.lib_ADUP import *
from lib_comms.lib_MSG import *

class adupTransactor():
    def __init__(self, adup):
        self.adup = adup
        self.mutex = False
        
    def lock(self):
        if self.mutex:
            return False
        else:
            self.mutex = True
            return True
        
    def unlock(self):
        self.mutex = False
        
    def Xfer(self, msg):
        self.adup.TX(msg)
        ret = self.adup.RX()
        
        return ret
        