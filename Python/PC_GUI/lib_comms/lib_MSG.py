
class MSG():

    def __init__(self, name):
        self._bsize=1024*128 # max size of the payload, in bytes
        self._buf="" # array to hold the payload
        self._cmd = '\0' # char to hold command
        self._len = 0 # length of payload
        self._name = name # name of this msg object (not exchanged via protocol)

    def getname(self):
        return self._name
    
    def setName(self, name):
        self._name = name
        
    def setPayload(self, str):
        if self._bsize < len(str):
            print("ERROR: cur payload ["+str(self._len)+"] exceeds max size ["+str(self._bsize)+"]; ignoring")
        else:
            self._buf = str
            self._len = len(self._buf)

    def appendPayload(self, str):
        newbuf = self._buf + str
        self.setPayload(newbuf)

    def setCmd(self, cmd):
        self._cmd = cmd

    def cmd(self):
        return self._cmd

    def payload(self):
        return self._buf

    def len(self):
        return self._len
    
    def toString(self):
        ret = ""
        ret = ret + "LABEL[" + self._name + "]\t"
        ret = ret + "CMD [" + self.cmd() + "]\t"
        #ret = ret + "BUF [" + self.payload() + "]"
        ret = ret + "LEN [" + str(self.len()) + "]\t"
        # ret = ret + self.payload()
        
        return ret

    def parseStr(self, str):
        self._cmd = str[0:1]
        self._len = str[2:4]
        self._buf = str[4:]