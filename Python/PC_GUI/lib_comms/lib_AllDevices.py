from lib_comms.lib_DemoBlockComms import *

class AllDevices():
    baud = 921600;

    def __init__(self):
        self._dcomms = []
        adup = ADUP()
        ports = adup.getAllPorts()
        # print("------------------------------------------")
        devs = []
        for p in ports:
            dev = ADUP()
            dev.serOpen(p, AutoDcomms.baud)
            if dev.autobaud():
                devs.append(dev)

        # print("connected to all devices that ACK'd autobaud: " + str(len(devs)))
        # print("------------------------------------------")

        # dcomms = []
        for dev in devs:
            self._dcomms.append(DemoBlockComms(dev))

    def getDevs(self):
        return self._dcomms