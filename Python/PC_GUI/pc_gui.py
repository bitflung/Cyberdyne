import threading
from lib_gui.max78000_gui import *

baud=115200
# pname = "COM10"

class pc_gui():
    def __init__(self):
        self.gui = max78000_gui("Human/Robot Tester")
        self.ready = False        
        self.baud = baud
        self.adup = self.portScanner()
        if(self.adup):
            print("SUCCESS: connected to the first enumerated ADUP target")            
        else:
            print("ERROR: could not find a connected ADUP target!")
            sys.exit(-1)
            
#         self.launch()
#         self.pname = "COM10"
        
    def portScanner(self):
        _adup = ADUP()
        _adup.serDebug(2)
        useAutoBaud=True
        ports = _adup.getAllPorts() # gets all mbed ports
#         ports = _adup.getMatchPorts("COM10") # gets ports with the given string in the name
        print("------------------------------------------")
        devs = []

        for p in ports:
            dev = ADUP()
            valid=True
            try:
                dev.serOpen(p, baud)
            except:
                valid=False
                
            if(valid):                
                #dev.serDebug(1)
                print("testing dev ["+dev.name()+"] at baud["+str(baud)+"]")
                if useAutoBaud:
                    print("\ttesting autobaud")
                    if dev.autobaud():
                        devs.append(dev)
                        #return dev
                    
        if(len(devs) > 0):
            return devs[0]
        else:
            return None

    def launch(self):
        self.adup.serInit(128)
        self.gui.setAdup(self.adup)
        print("app is ready")
        self.ready = True
        self.gui.looper()


app = pc_gui()
app.launch()

sys.exit(0)
ready = False

#gui = legacy_gui("Legacy Product", in_pump, out_pump)
gui = max78000_gui("Human/Robot Tester")

adup = ADUP()
ports = portScanner(adup)

ports = adup.getAllPorts()
if(len(ports) > 1):
    ports = adup.getMatchPorts("COM10")
    
for p in ports:
    print(p)
    
adup.serOpen(ports[0], 115200)
adup.autobaud()
#adup.serDebug(2)
adup.serInit(128)
# 
# adupRunner = adup_runner(adup)
# adupRunner.setGui(gui)
# 
# gui.setRunner(adupRunner)
gui.setAdup(adup)

print("app is ready")
ready = True
gui.looper()


# def portScanner(_adup):
#     ports = _adup.getAllPorts() # gets all mbed ports
#     ports = _adup.getMatchPorts("COM10") # gets ports with the given string in the name
#     print("------------------------------------------")
#     devs = []
# 
#     for p in ports:
#         dev = ADUP()
#         dev.serOpen(p, baud)
#         dev.serDebug(1)
# 
#         print("testing dev ["+dev.name()+"] at baud["+str(baud)+"]")
#         if useAutoBaud:
#             print("\ttesting autobaud")
#             if dev.autobaud():
#                 devs.append(dev)
#     return devs
