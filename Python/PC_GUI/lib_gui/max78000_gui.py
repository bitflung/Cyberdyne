# from lib_enclave.lib_linkedlist import *
import tkinter as tk
from tkinter import ttk
from tkinter import scrolledtext
import threading
import numpy as np
from PIL import ImageTk, Image

from lib_comms.lib_ADUP import *

#from .pump_ctrl import *
#import hashlib
#import random

#from tkinter import scrolledText

class max78000_gui():
    def __init__(self, name):        
        self.win = tk.Tk()
        self.win.title(name)
        self.setup()
        self.RMSG = MSG("run")
        self.CMSG = MSG("cam")
        self.AMSG = MSG("art")
        
        self.RMSG.setCmd('R')
        self.CMSG.setCmd('C')
        self.AMSG.setCmd('A')
        
        self.adup = None
        #self.run()

    def setAdup(self, adup):    
        print("setting up ADUP callback")
        self._adup=adup
        self._adup.reg_callback(self.handler)
        
    def handler(self, msg):
        # msg is a msg_t object
        print("inside adup handler, nothing to do as we are the controller side")
        
    def makeUI(self):
        frm = tk.Frame(master=self.win)
        frm.pack(fill=tk.X)
        
        rowvar = 0
        
        lblImage = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Image",
            width=50,
            #bg="blue",
            #fg="white",
            )
        lblImage.grid(column=0,row=rowvar)#, columnspan=1)
        
        # Generate a left-right gradient 128 px wide by 128 pixels tall
        ba = bytearray((np.arange(128) + np.zeros((128,1))).astype(np.uint8)) 
        
        # Convert bytearray "ba" to PIL Image, 'L' just means greyscale/lightness
        im = Image.frombuffer('L', (128,128), ba, 'raw', 'L', 0, 1)
        
        ##### scale up by 2x #####
        w,h=im.size
        w=w*2
        h=h*2
        im=im.resize((w,h), resample=0)         
        ##########################
        
        # save to disk because that's cool
        im.save('tmp.png')
        img1=ImageTk.PhotoImage(im)        
        #img2=ImageTk.PhotoImage(Image.open("tmp.png"))        
        
        self.imgLbl = tk.Label(frm, image=img1)
        self.imgLbl.grid(column=0, row=1)
        
        frm2 = tk.Frame(master=frm)
        frm2.grid(column=1, row=1, rowspan=2)
        
        btnRun = tk.Button(
            master=frm2,
            text = "Test",
            fg = "black",
            bg = "grey",
            width = 20,
            height = 3,
            command = self.btnRunExec,
            )
        btnRun.grid(column=0, row=0, columnspan=1)
        
        btnCam = tk.Button(
            master=frm2,
            text = "Photo",
            fg = "black",
            bg = "grey",
            width = 20,
            height = 3,
            command = self.btnCamExec,
            )
        btnCam.grid(column=0, row=2, columnspan=1)
        
    
    def btnRunExec(self):
        self._adup.TX(self.RMSG)
        ret = self._adup.RX()
        print(ret.toString())    
        print("ran!")
        
    def btnCamExec(self):
        self._adup.TX(self.CMSG)
        print("TX done")
        
        ret = self._adup.TSOP() # receive a posted msg
        print(ret.toString())
        
        dne = self._adup.RX()
        
        print("RX'd camera data; converting to image")
#         pay = ret.payload()
#         for i in range(0, ret.len(), 2):
#             # for each pair of character in the payload
#             tst = pay[i:i+2]
            
        ba = bytes.fromhex(ret.payload())
        print("converted 32k hex into [" + str(len(ba)) + "] bytes")
		
		#convert from -128+127 format to 0-255
        
        im = Image.frombuffer('L', (128,128), ba, 'raw', 'L', 0, 1)
        
        ##### scale up by 2x #####
        w,h=im.size
        w=w*2
        h=h*2
        im=im.resize((w,h), resample=0)         
        ##########################
        
        # save to disk because that's cool
        im.save('tmp.png')
        img1=ImageTk.PhotoImage(im)        
        #img2=ImageTk.PhotoImage(Image.open("tmp.png"))        
        self.imgLbl.configure(image=img1)
        self.imgLbl.image=img1
        
        self._adup.serDebug(0)
        
        print("cam'd!")
        
    
    def termOutputUpdate(self, txt):
        #self.txtTermOutput.delete("1.0", tk.END)
        # text_widget.get('1.0', 'end-1c')
        length = len(self.txtTermOutput.get("1.0", tk.END))
        if length > 1024*10:
            self.txtTermOutput.delete("1.0", "5.0")            
            #self.txtTermOutput.insert(tk.INSERT, "...trimmed..."+"\n")
        
        self.txtTermOutput.insert(tk.INSERT, txt+"\n")
        self.txtTermOutput.see(tk.END)
       
    def setup(self):
        self.makeUI()

    def looper(self):
        self.win.mainloop()
        # anything else we need to do?
        
    def run(self):
        self.thread = threading.Thread(target = self.looper, args=(), daemon=True)
        self.thread.start()
        print("pc gui app running")
    
    
