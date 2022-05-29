# from lib_enclave.lib_linkedlist import *
import tkinter as tk
from tkinter import ttk
from tkinter import scrolledtext
import threading
import numpy as np
import PIL
from PIL import ImageTk, Image

from lib_comms.lib_ADUP import *

#from .pump_ctrl import *
#import hashlib
#import random

#from tkinter import scrolledText

class max78000_gui():
    def __init__(self, name):
        self.hbg_c="black"
        self.hbg_w=2
        self.f_padx=5
        self.f_pady=5
        self.buttons = []
        
        self.win = tk.Tk()
        self.win.title(name)
        self.setup()
        self.RMSG = MSG("results")
        self.CMSG = MSG("camera")
        self.VMSG = MSG("voice")
        self.TMSG = MSG("transfer")
        
        self.RMSG.setCmd('R')
        self.CMSG.setCmd('C')
        self.VMSG.setCmd('V')
        self.TMSG.setCmd('T')
        
        self.RMSG.setPayload("")
        self.CMSG.setPayload("")
        self.VMSG.setPayload("")
        self.TMSG.setPayload("")
        
        
        
        self.adup = None
        #self.run()

    def disableAll(self):
        for b in self.buttons:            
            b.configure(state='disable')
            
    def enableAll(self):
        for b in self.buttons:            
            b.configure(state='active')
        
    def setAdup(self, adup):    
        print("setting up ADUP callback")
        self._adup=adup
        self._adup.reg_callback(self.handler)
        
    def handler(self, msg):
        # msg is a msg_t object
        print("inside adup handler, nothing to do as we are the controller side")
        
    def makefrmTopL(self, frm):
        lblImage = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Upscaled Image",
            width=80,
            bg="blue",
            fg="white",
            anchor="w",
            )
        #lblImage.grid(column=0,row=0)#, columnspan=1)
        lblImage.pack()
        
        if(False):
            im = Image.open("default_native.png")
        else:
            # Generate a left-right gradient 128 px wide by 128 pixels tall
            ba = bytearray((np.arange(128) + np.zeros((128,1))).astype(np.uint8)) 
            
            # Convert bytearray "ba" to PIL Image, 'L' just means greyscale/lightness
            im = Image.frombuffer('L', (128,128), ba, 'raw', 'L', 0, 1)
            im.save('tmp_launch.png') # for some reason this doesn't work unless i save the file?!
            
        im = self.imageFormat(im, False)                      
        img1=ImageTk.PhotoImage(im)        
   
        self.imgLbl = tk.Label(frm, image=img1, borderwidth=1, relief="solid")
        self.imgLbl.pack()
                
    def makefrmBotL(self, frm):
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Native",
            width=40,
            bg="blue",
            fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        
        im = PIL.Image.open("default.png")
        img1=ImageTk.PhotoImage(im)        
   
        self.imgLblNative = tk.Label(frm, image=img1, borderwidth=1, relief="solid")
        #self.imgLbl.grid(column=0, row=1)
        self.imgLblNative.pack()
        btn = tk.Button(
            master=frm,
            text = "Upload",
            fg = "black",
            bg = "grey",
            width = 20,
            height = 3*2,
            command = self.btnUploadNative,
            )
        btn.pack(expand=True, fill='both')
        self.buttons.append(btn)
        
    def makefrmBotM(self, frm):
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Patched",
            width=40,
            bg="blue",
            fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        
        im = PIL.Image.open("patched.png")
        img1=ImageTk.PhotoImage(im)        
   
        self.imgLblPatched = tk.Label(frm, image=img1, borderwidth=1, relief="solid")
        #self.imgLbl.grid(column=0, row=1)
        self.imgLblPatched.pack()
        
        btn = tk.Button(
            master=frm,
            text = "Upload",
            fg = "black",
            bg = "grey",
            width = 20,
            height = 3*2,
            command = self.btnUploadPatched,
            )
        btn.pack(expand=True, fill='both')
        self.buttons.append(btn)
        
    def makefrmR(self, frm):
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            height=4,
#             relief="groove",
#             text="Adversarial Patching",
#             width=40,
#             bg="blue",
#             fg="white",
#             anchor="w",
        )
        lbl.pack(expand=True, fill='both')
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Demo",
            width=40,
            bg="blue",
            fg="white",
            anchor="w",
            )        
        lbl.pack()
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Results",
            width=40,
            #bg="blue",
            #fg="white",
            anchor="w",
            )        
        lbl.pack()
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="[result text here]",
            width=40,
            #bg="blue",
            #fg="white",
            anchor="w",
            )        
        lbl.pack()
        self.lblResults = lbl
        
        btn = tk.Button(
            master=frm,
            text = "Execute Demo",
            fg = "black",
            bg = "grey",
            width = 20,
            height = 3,
            command = self.btnExecute,
            )
        #btnRun.grid(column=0, row=2)#, columnspan=1)
        btn.pack(expand=True, fill='both')
        self.buttons.append(btn)
        
        ####################
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Image",
            width=40,
            bg="blue",
            fg="white",
            anchor="w",
            )
        #lbl0.grid(column=0,row=0, columnspan=2)
        lbl.pack(expand=True, fill='both')
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Prompt",
            #width=75,
            #bg="blue",
            #fg="white",
            anchor="w",
            )
        #lbl1.grid(column=0, row=1)
        lbl.pack(expand=True, fill='both')
        #return
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="[prompt text here]",
            #width=75,
            # height=3,
            #bg="blue",
            #fg="white",
            anchor="w",
            )
        #lbl2.grid(column=1, row=1)
        lbl.pack(expand=True, fill='both')
        self.img_prompt=lbl
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Result",
            #width=75,
            #bg="blue",
            #fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="[result text here]",
            #width=75,
            #bg="blue",
            #fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        self.img_result=lbl
        
        ##################################
        btn = tk.Button(
            master=frm,
            text = "Capture",
            fg = "black",
            bg = "grey",
            width = 20,
            height = 3,
            command = self.btnCapture,
            )
        #btnRun.grid(column=0, row=2)#, columnspan=1)
        btn.pack(expand=True, fill='both')
        self.buttons.append(btn)
        
        btn = tk.Button(
            master=frm,
            text = "Classify",
            fg = "black",
            bg = "grey",
            width = 20,
            height = 3,
            command = self.btnClassify,
            )
        #btnRun.grid(column=0, row=2)#, columnspan=1)
        btn.pack(expand=True, fill='both')
        self.buttons.append(btn)
        
        btn = tk.Button(
            master=frm,
            text = "Transfer",
            fg = "black",
            bg = "grey",
            width = 20,
            height = 3,
            command = self.btnTransfer,
            )
        btn.pack(expand=True, fill='both')
        self.buttons.append(btn)
        
        ##############################
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Voice",
            width=40,
            bg="blue",
            fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Prompt",
            #width=75,
            #bg="blue",
            #fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="[prompt text here]",
            #width=75,
            #bg="blue",
            #fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        self.vce_prompt=lbl
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Result",
            #width=75,
            #bg="blue",
            #fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="[result text here]",
            #width=75,
            #bg="blue",
            #fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        self.vce_result=lbl            
        
        #############################
        
        btn = tk.Button(
            master=frm,
            text = "Capture & \nClassify",
            fg = "black",
            bg = "grey",
            width = 20,
            height = 3,
            command = self.btnVoiceDemo,
            )
        btn.pack(expand=True, fill='both')
        self.buttons.append(btn)
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Adversarial Patching",
            width=40,
            bg="blue",
            fg="white",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        
        btn = tk.Button(
            master=frm,
            text = "Apply Patch",
            fg = "black",
            bg = "grey",
            width = 40,
            height = 3,
            command = self.btnApplyPatch,
            )
        btn.pack(expand=True)#, fill='both')
        self.buttons.append(btn)
        
        btn = tk.Button(
            master=frm,
            text = "Load Patch",
            fg = "black",
            bg = "grey",
            width = 40,
            height = 3,
            command = self.btnLoadPatch,
            )
        btn.pack(expand=True)#, fill='both')
        self.buttons.append(btn)
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            height=7,
#             relief="groove",
#             text="Adversarial Patching",
#             width=40,
#             bg="blue",
#             fg="white",
#             anchor="w",
            )
        lbl.pack(expand=True, fill='both')
    
    def makefrmBot(self, frm):
        botL = tk.Frame(master=frm)
        botM = tk.Frame(master=frm)
        botR = tk.Frame(master=frm)
        
        botL.pack(side=tk.LEFT)
        botM.pack(side=tk.LEFT)
        botR.pack(side=tk.LEFT)
        
        self.makefrmBotL(botL)
        self.makefrmBotM(botM)
        #self.makefrmBotR(botR)
        
    
    def makeUI(self):
        #self.win.geometry("320x200")
        
        frmRoot = tk.Frame(master=self.win)
        self.frmRoot = frmRoot
        
        frmRoot.pack()
        
        frmL = tk.Frame(master=frmRoot)
        frmR = tk.Frame(master=frmRoot)        
        frmL.pack(side=tk.LEFT)
        frmR.pack(side=tk.RIGHT)
        
#         frmTop = tk.Frame(master=frmRoot)
#         frmBot = tk.Frame(master=frmRoot)
#         frmTop.pack(side=tk.TOP)
#         frmBot.pack(side=tk.BOTTOM)
        
        frmTL = tk.Frame(master=frmL)
        frmBL = tk.Frame(master=frmL)
        frmTL.pack(side=tk.TOP)
        frmBL.pack(side=tk.BOTTOM)
        
        frmBot = tk.Frame(master=frmBL)
        frmBot.pack()
        
        self.makefrmTopL(frmTL)        
        self.makefrmBot(frmBot)
        self.makefrmR(frmR)
        
        #self.makefrmBotL(frmBL)
#         self.makefrmBotR(frmBR)
    
    def oldmakeUI(self):
        #self.win.geometry("320x200")
        
        frmRoot = tk.Frame(master=self.win)
        frmRoot.pack()
        
        frmTop = tk.Frame(master=frmRoot)
        frmBot = tk.Frame(master=frmRoot)
        frmTop.pack(side=tk.TOP)
        frmBot.pack(side=tk.BOTTOM)
        
        frmTL = tk.Frame(master=frmTop)
        frmTR = tk.Frame(master=frmTop)               
#         frmBL = tk.Frame(master=frmBot)
#         frmBR = tk.Frame(master=frmBot)
        
        frmTL.pack(side=tk.LEFT)
#         frmBL.pack(side=tk.LEFT)
        frmTR.pack(side=tk.RIGHT)
#         frmBR.pack(side=tk.RIGHT)
        
        self.makefrmTopL(frmTL)
        self.makefrmTopR(frmTR)
        self.makefrmBot(frmBot)
        #self.makefrmBotL(frmBL)
#         self.makefrmBotR(frmBR)
    
    def adupTransact(self, msg, name, exp):
        self._adup.TX(self.CMSG)
        ret = self._adup.RX()
        if(ret.payload() == exp):
            print(name+": transaction success")
        else:
            print(name+": transaction fail -> "+ret.toString(ret.payload()))
        return ret
            
    def btnExecute(self):
        # perform IMAGE then VOICE commands, all in a row
        pass
        
    def btnCapture(self):
        self.disableAll()
        threading.Thread(target=self.thrd_btnCapture).start()
        
    def thrd_btnCapture(self):        
        self.img_prompt.configure(text="...working...")
        self.img_prompt.text="...working..."
        self.CMSG.setPayload("0") # capture ues camera subcmd 0
        self.adupTransact(self.CMSG, "capture", "OK")
        self.img_prompt.configure(text="Captured!")
        self.img_prompt.text="Captured!"
        self.enableAll()
        
    
    def btnClassify(self):
        self.disableAll()
        threading.Thread(target=self.thrd_btnClassify).start()
    
    def thrd_btnClassify(self):
        self.CMSG.setPayload("1") # subcmd for classify
        self._adup.TX(self.CMSG)
        ret = self._adup.RX()
        
        self.RMSG.setPayload("0")
        self._adup.TX(self.RMSG)
        ret = self._adup.RX()
        print(ret.toString(ret.payload()))
        self.img_result.configure(text=ret.payload()+"%")
        self.img_result.text=ret.payload()+"%"
        self.enableAll()
        
    def btnTransfer(self):
        self.disableAll()
        threading.Thread(target=self.thrd_btnTransfer).start()
        
    def thrd_btnTransfer(self):
        self.CMSG.setPayload("2") # subcmd for xfering image to PC is 3
        self._adup.TX(self.CMSG)
                
        msg_numMsgs=self._adup.RX() # receive a msg indicating how many data msgs to accept
        numMsgs = int(msg_numMsgs.payload(), 16)
        print(msg_numMsgs.payload())
        print("expecting "+str(numMsgs)+" messages of data...")
        
        ret = MSG("full_image")
        ret.setCmd("C")
        step = 100/numMsgs
        progress = 0
        
        for i in range(numMsgs):
            msgi = self._adup.TSOP()
            ret.appendPayload(msgi.payload())
            progress=progress+step
            print(str(progress)+"%: RX'd ["+str(msgi.len())+"] hex chars")
            
           
        print("received all msgs")
        print(ret.toString())
        
        dne = self._adup.RX() # capture a final DONE message        
        print("RX'd camera data; converting to image")
        
        ba = bytes.fromhex(ret.payload())
        print("converted 32k hex into [" + str(len(ba)) + "] bytes")

        #print("we were supposed to get " + str(4*128*128)+ " bytes!")
        
        # flush stdout
        sys.stdout.flush()
        
        rxdata = np.frombuffer(ba, dtype=np.int8)

        imgdata = np.delete(rxdata, np.arange(0, rxdata.size, 4))
        #change the two lines below where it has 128//4 to 128 when we fix the issue with sending all the data
        #imgdata = np.reshape(imgdata,(3,128,128))
        imgdata = np.reshape(imgdata,(128,128,3))
        #imgdata=np.moveaxis(imgdata,0,2)

        imgdata_unsigned = np.array((3,128,128),dtype = np.uint8)
        imgdata_unsigned = imgdata + 128

        #swap red and blue so they are in the correct order
        imgdata_tmp= np.copy(imgdata_unsigned)
        imgdata_unsigned[:,:,0] = imgdata_unsigned[:,:,2]
        imgdata_unsigned[:,:,2] = imgdata_tmp[:,:,0]

        im=Image.fromarray(imgdata_unsigned.astype('uint8'),'RGB')
        
        im = self.imageFormat(im, True)
        self.updateGuiImage(im)
   
        print("cam'd!")
        self.enableAll()
        
    def btnVoiceDemo(self):
        self.disableAll()
        threading.Thread(target=self.thrd_btnVoiceDemo).start()
        
    def thrd_btnVoiceDemo(self):
        # perform full voice demo
        self.vce_prompt.configure(text="... listening ...")
        self.vce_prompt.text="... listening ..."
        
        self.VMSG.setPayload("1")
        self._adup.TX(self.VMSG)
        ret = self._adup.RX()
        print(ret.toString(ret.payload()))        
        self.vce_prompt.configure(text="Done!")
        self.vce_prompt.text="Done!"
        
        self.RMSG.setPayload("1")
        self._adup.TX(self.RMSG)
        ret = self._adup.RX()
        print(ret.toString(ret.payload()))
        self.vce_result.configure(text=ret.payload())
        self.vce_result.text=ret.payload()
        
        self.enableAll()
    
        
    def btnUploadNative(self):
        self.disableAll()
        threading.Thread(target=self.thrd_btnUploadNative).start()
        # upload the locally stored native image to the board
        pass
    
    def thrd_btnUploadNative(self):
        self.enableAll()
        
    def btnUploadPatched(self):
        self.disableAll()
        threading.Thread(target=self.thrd_btnUploadPatched).start()
        # upload patched image to the board
        pass
    
    def thrd_btnUploadPatched(self):
        self.enableAll()
        
    def btnApplyPatch(self):
        self.disableAll()
        threading.Thread(target=self.thrd_btnApplyPatch).start()
        # apply an adversarial patch to the native image, creating and updating the patched image
        pass
    
    def thrd_btnApplyPatch(self):
        self.enableAll()
        
    def btnLoadPatch(self):
        self.disableAll()
        threading.Thread(target=self.thrd_btnApplyPatch).start()
        # load a patch file from disk to be applied later to captured images
        pass
    
    def thrd_btnLoadPatch(self):
        self.enableAll()
        
    def imageFormat(self, im, save):
        if(save):
            im.save("tmp_native.png")
        
        #im = Image.frombuffer('L', (128,128), ba, 'raw', 'L', 0, 1)
        
        ##### scale up by 2x #####
        defaultScaling = 4
        
        w,h=im.size
        w=w*defaultScaling*1
        h=h*defaultScaling*1
        im=im.resize((w,h), resample=PIL.Image.BICUBIC) # resample==0 # PIL.Image.BICUBIC         
        ##########################
        
        # save to disk because that's cool
        if(save):
            im.save('tmp_scaled.png')
            
        return im
            
    def updateGuiImage(self, im):
        img1=ImageTk.PhotoImage(im)        
        #img2=ImageTk.PhotoImage(Image.open("tmp.png"))        
        self.imgLbl.configure(image=img1)
        self.imgLbl.image=img1
        
        im = Image.open("tmp_native.png")
        img2=ImageTk.PhotoImage(im) 
        self.imgLblNative.configure(image=img2)
        self.imgLblNative.image=img2
        
        im = Image.open("patched.png")
        img3=ImageTk.PhotoImage(im) 
        self.imgLblPatched.configure(image=img3)
        self.imgLblPatched.image=img3
        
        
    
        
    
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
        im = PIL.Image.open("default.png")
        im = self.imageFormat(im, True)
        self.updateGuiImage(im)
        self.win.mainloop()
        # anything else we need to do?
        
    def run(self):
        self.thread = threading.Thread(target = self.looper, args=(), daemon=True)
        self.thread.start()
        print("pc gui app running")
    
    
