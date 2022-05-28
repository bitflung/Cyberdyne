import threading
from lib_gui.max78000_gui import *


ready = False

#gui = legacy_gui("Legacy Product", in_pump, out_pump)
gui = max78000_gui("Human/Robot Tester")

adup = ADUP()
ports = adup.getMatchPorts("COM10")
for p in ports:
    print(p)
    
adup.serOpen(ports[0], 115200)
adup.autobaud()
#adup.serDebug(2)
adup.serInit(128)

gui.setAdup(adup)

print("app is ready")
ready = True
gui.looper()
