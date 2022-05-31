# from lib_enclave.lib_linkedlist import *
import tkinter as tk
from tkinter import ttk
from tkinter import scrolledtext
import threading
import numpy as np
import PIL
from PIL import ImageTk, Image

from lib_comms.lib_ADUP import *
from patch import *
import binascii

#from .pump_ctrl import *
#import hashlib
#import random

#from tkinter import scrolledText

class max78000_gui():
    def __init__(self, name):
        self.locked = False
        self.lockCount=0
        self.popWidgets = {'none':0}
        self.consoleOpen=False
        
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

    def lock(self, num=1):
        self.locked=True
        self.lockCount=self.lockCount+num
        
    def unlock(self, num=1):
        self.lockCount=self.lockCount-num
        if(self.lockCount <= 0):
            self.lockCount=0
            self.locked=False
        
    def disableAll(self):
        for b in self.buttons:            
            b.configure(state='disable')
            
    def enableAll(self):
        if(self.locked):
            print("not enabling widgets yet; more tasks running")
        else:
            for b in self.buttons:            
                b.configure(state='active')
        
    def createConsole(self, hght, nam = "Close console to continue"):
        if(self.consoleOpen):
            print("console is already open")
        else:
            # create the popup window                        
            self.popWidgets.clear()
            
            pop=tk.Toplevel(self.win)
            pop.protocol("WM_DELETE_WINDOW", self.onConsoleClose)
            pop.geometry("+1000+20")
            pop.title("Console")
            pop.config(bg="white")
            self.popWidgets['pop']=pop
            
            lbl = tk.Label(pop,
                           text=nam,
                           bg="white",
                           fg="black",
                           font=('Aerial', 12),
                           anchor="w",
                           width=40,
                           )
            lbl.grid(column = 0,row = 0)
            self.popWidgets['lbl']=lbl
            
            text=tk.scrolledtext.ScrolledText(pop,
                                      wrap = tk.WORD,
                                      width = 80,
                                      height = hght,
                                      bg="black",
                                      fg="green",
                                      font = ("Terminal", 10),
                                      )
            text.grid(column = 0, pady = 10, padx = 10)
            self.popWidgets['text']=text        
            self.consoleOpen=True
        
    def onConsoleClose(self):
        if(self.locked):
            print("not closing console; it is locked")
        else:
            if(len(self.popWidgets)>0):
                print("console closing...")            
                self.consoleOpen=False
                self.popWidgets['pop'].destroy();
                self.popWidgets.clear()
            else:
                print("console not open; nothing to close")
                
        
    def waitConsole(self):
        if(self.locked):
            print("not waiting on console to close; it is locked")
            return        
        while(self.consoleOpen):
            pass
        self.enableAll()
        
    def setAdup(self, adup):    
        print("setting up ADUP callback")
        self._adup=adup
        self._adup.reg_callback(self.handler)
        self._adup.regDebugCB(self.debugCB)
        
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
            anchor="w",
            )        
        lbl.pack()
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="[result text here]",
            width=40,
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
        lbl.pack(expand=True, fill='both')
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Prompt",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        #return
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="[prompt text here]",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        self.img_prompt=lbl
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="Result",
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="[result text here]",
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
        btn.pack(expand=True, fill='both')
        self.buttons.append(btn)
        
        btn = tk.Button(
            master=frm,
            text = "Download",
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
            anchor="w",
            )
        lbl.pack(expand=True, fill='both')
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            relief="groove",
            text="[prompt text here]",

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
        btn.pack(expand=True)
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
        #btn.pack(expand=True)
        self.buttons.append(btn)
        
        lbl = tk.Label(
            master=frm,
            borderwidth = 3,
            height=7,
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

    
    def makeUI(self):
        frmRoot = tk.Frame(master=self.win)
        self.frmRoot = frmRoot
        
        frmRoot.pack()
        
        frmL = tk.Frame(master=frmRoot)
        frmR = tk.Frame(master=frmRoot)        
        frmL.pack(side=tk.LEFT)
        frmR.pack(side=tk.RIGHT)

        frmTL = tk.Frame(master=frmL)
        frmBL = tk.Frame(master=frmL)
        frmTL.pack(side=tk.TOP)
        frmBL.pack(side=tk.BOTTOM)
        
        frmBot = tk.Frame(master=frmBL)
        frmBot.pack()
        
        self.makefrmTopL(frmTL)        
        self.makefrmBot(frmBot)
        self.makefrmR(frmR)

     
    def debugCB(self, dstr):
        if(self.consoleOpen):
            text = self.popWidgets['text']
            text.insert(tk.INSERT, dstr)
            text.see("end")
        else:
            print("app debug: "+dstr, end='')        
    
    
    def btnExecute(self):
        # perform IMAGE then VOICE commands, all in a row
        self.disableAll()
        self.lock()
        self.createConsole(70)
        threading.Thread(target=self.thrd_btnExecute).start()
    
    def thrd_btnExecute(self):        
        self.thrd_btnCapture()
        self.thrd_btnClassify()
        #self.thrd_btnTransfer() # should we transfer by default?
        self.thrd_btnVoiceDemo()
        
        # fetch final results and pupulate the GUI
        self.RMSG.setPayload("2")
        self._adup.TX(self.RMSG)
        res = self._adup.RX()        
        self.lblResults.configure(text=res.payload())
        self.lblResults.text=res.payload()
        
        self.unlock()        
        self.waitConsole()
        
        
    def btnCapture(self):        
        threading.Thread(target=self.thrd_btnCapture).start()
        
    def thrd_btnCapture(self):
        self.disableAll()
        self.lock()
        self.createConsole(30)
        self.debugCB("Taking your picutre in ")
        for i in range(3,0,-1):            
            self.debugCB("{:d} ".format(i))
            time.sleep(1)        
        self.debugCB("\n*NOW*\n")
        
        self.img_prompt.configure(text="...working...")
        self.img_prompt.text="...working..."
        self.CMSG.setPayload("0") # capture ues camera subcmd 0
        self._adup.TX(self.CMSG)
        ret = self._adup.RX() # OK response
        self.img_prompt.configure(text="Captured!")
        self.img_prompt.text="Captured!"
        time.sleep(1)
        self.unlock()
        self.onConsoleClose()
        self.waitConsole()
        
    
    def btnClassify(self):        
        threading.Thread(target=self.thrd_btnClassify).start()
    
    def thrd_btnClassify(self):
        self.disableAll()
        self.lock()
        self.createConsole(70)
        self.CMSG.setPayload("1") # subcmd for classify
        
        self._adup.TX(self.CMSG)        
        ret = self._adup.RX()

        self.RMSG.setPayload("0")
        self._adup.TX(self.RMSG)
        ret = self._adup.RX()
        print(ret.toString(ret.payload()))
        self.img_result.configure(text=ret.payload()+"%")
        self.img_result.text=ret.payload()+"%"
        self.unlock()
        self.waitConsole()

        
    def btnTransfer(self):
        threading.Thread(target=self.thrd_btnTransfer).start()
        
    def thrd_btnTransfer(self):
        self.disableAll()
        self.lock()
        self.createConsole(30, "Downloading image...")
        
        self.CMSG.setPayload("2") # subcmd for xfering image to PC is 3
        self._adup.TX(self.CMSG)
                
        msg_numMsgs=self._adup.RX() # receive a msg indicating how many data msgs to accept
        numMsgs = int(msg_numMsgs.payload(), 16)
        print(msg_numMsgs.payload())
        self.debugCB("Requesting image data\n")
        
        ret = MSG("full_image")
        ret.setCmd("C")
        step = 100/numMsgs
        progress = 0
        
        for i in range(numMsgs):
            msgi = self._adup.TSOP()
            ret.appendPayload(msgi.payload())
            progress=progress+step
            #print(str(progress)+"%: RX'd ["+str(msgi.len())+"] hex chars")
            self.debugCB("{:.2f}%\n".format(progress))
            #self.debugCB(str(progress)+"%\n")#: RX'd ["+str(msgi.len())+"] hex chars\n")
        
        self.debugCB(ret.toString("Downloaded image stats")+"\n")
        
        dne = self._adup.RX() # capture a final DONE message        
        self.debugCB("RX'd camera data; converting to image\n")
        
        ba = bytes.fromhex(ret.payload())
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
        
        
        self.unlock()
        #self.debugCB("close the window\n")
        self.onConsoleClose()
        self.waitConsole()
        
    def btnVoiceDemo(self):
        threading.Thread(target=self.thrd_btnVoiceDemo).start()
        
    def thrd_btnVoiceDemo(self):
        self.disableAll()
        self.lock()
        self.createConsole(70)
        self.debugCB("What do you prefer?\n\tDog\n\tLove\n\tDisk\n")
        
        # perform full voice demo
        self.vce_prompt.configure(text="... listening ...")
        self.vce_prompt.text="... listening ..."
        
        self.VMSG.setPayload("{:x}".format(10))
        self._adup.TX(self.VMSG)
        self.debugCB("sent voice cmd\n")
        
        ret = self._adup.RX()
        self.debugCB("received ack: {:s}\n".format(ret.toString(ret.payload())))
        
        #self._adup.serDebug(2)
        ret = self._adup.RX()
        self.debugCB("voice recog done: {:s}\n".format(ret.toString(ret.payload())))
        
        self.vce_prompt.configure(text="Done!")
        self.vce_prompt.text="Done!"
        
        self.debugCB("fetching voice recog results\n")
        self.RMSG.setPayload("1")
        self._adup.TX(self.RMSG)
        ret = self._adup.RX()
        print(ret.toString(ret.payload()))
        
        self.vce_result.configure(text=ret.payload())
        self.vce_result.text=ret.payload()
        self.debugCB("-----\nYour fate is sealed, please close the console\n-----\n")
        self.unlock()        
        self.waitConsole()
    
        
    def btnUploadNative(self):
        threading.Thread(target=self.thrd_btnUploadNative).start()
        # upload the locally stored native image to the board
        pass
    
    def uploadImage(self, imgbytes):
        maxMsgLen = (8*1024)
        
        self.debugCB("Uploading an image of [{:d} d] bytes\n".format(len(imgbytes)))
        impay = binascii.hexlify(imgbytes).decode('utf-8')
        
        imgMsg = MSG("image_to_upload")        
        imgMsg.setPayload(impay)
        self.debugCB("ascii hex payload = [{:d} d] bytes\n".format(imgMsg.len()))
#         self.debugCB("first 8 words of hex ascii bytes:\n")
#         for i in range(0, 8*8, 8):
#             self.debugCB("\t"+impay[i:i+8]+"\n")
                     
        
        payload = imgMsg.payload() # get the payload string from above        
        numMsgs = imgMsg.len()//(maxMsgLen)
        
        #print("breaking img of ["+str(imgMsg.len())+"] hex ascii chars into ["+str(numMsgs)+"] msgs")
        
        # send a msg to MCU indicating start of image upload and how many msgs it will require
        self.TMSG.setPayload("{:x}".format(numMsgs))
        self._adup.TX(self.TMSG)
        ack = self._adup.RX()
        
        # iterate over the imgMsg payload, striding by 8KB on each iteration
        partialMsg = MSG("image_chunk")
        partialMsg.setCmd('T') # indicate that this is a transfer msg type
        
        #curMsgCnt=0
        step = 100/numMsgs
        progress = 0
        for i in range(0, imgMsg.len(), maxMsgLen):
            
            # NOTE: this differs from the code used to transfer an imnage from MCU to PC
            # in THAT code I use POST() rather than TX()
            # here i'm using TX(), which adds protocol overhead but mitigates possible buffer overflows on the MCU
            #print("TX'ing msg ["+str(curMsgCnt)+"] of ["+str(numMsgs-1)+"]")
            partialMsg.setPayload(payload[i:i+maxMsgLen])            
            self._adup.TX(partialMsg) # transfer this chunk
            ack = self._adup.RX()
            #curMsgCnt=curMsgCnt+1
            #print("\tRX'd ack for: "+ack.payload())
            progress+=step
            self.debugCB("{:.2f}%\n".format(progress))
        
        #self._adup.serDebug(2)        
        dne = self._adup.RX()
        print(dne.toString(dne.payload()))
        print("data TX is done")
        
    def thrd_btnUploadNative(self):
        self.disableAll()
        self.lock()        
        self.createConsole(30, "Uploading image...")
        img_path = "tmp_native.png"
        img = load_image(img_path)
        imgbytes = img.tobytes()
        
        self.debugCB("uploading native image\n")        
        self.uploadImage(imgbytes)        
        self.unlock()
        self.onConsoleClose()
        self.waitConsole()
        
    def btnUploadPatched(self):      
        threading.Thread(target=self.thrd_btnUploadPatched).start()
        # upload patched image to the board
        pass
    
    def thrd_btnUploadPatched(self):
        self.disableAll()
        self.lock()        
        self.createConsole(30, "Uploading image...")
        img_path = "tmp_patched.png"
        img = load_image(img_path)
        imgbytes = img.tobytes()
        
        self.debugCB("uploading patched image\n")        
        self.uploadImage(imgbytes)        
        self.unlock()
        self.onConsoleClose()
        self.waitConsole()

        
    def btnApplyPatch(self):
        threading.Thread(target=self.thrd_btnApplyPatch).start()
        # apply an adversarial patch to the native image, creating and updating the patched image
        pass
    
    def thrd_btnApplyPatch(self):
        self.disableAll()
        self.lock()        
        self.createConsole(30, "Applying a patch...")
        # TODO: insert code to actually apply a patch to an image
        
        hack("tmp_native.png", "robotpatch.png");
#         
#         im = make_patched_img("tmp_native.png", "robotpatch.png")
#         
#         imgfile=Image.fromarray(im.astype('uint8'),'RGB')
#         imgfile.save("tmp_patched.png")
        im = Image.open("tmp_patched.png")
        img3=ImageTk.PhotoImage(im) 
        self.imgLblPatched.configure(image=img3)
        self.imgLblPatched.image=img3
        
        #self.updateGuiImage()
        
        
        self.debugCB("patched\n")
        self.unlock()
        self.onConsoleClose()
        self.waitConsole()
        
    def btnLoadPatch(self):
        threading.Thread(target=self.thrd_btnLoadPatch).start()        
        pass
    
    def thrd_btnLoadPatch(self):
        self.disableAll()
        self.lock()        
        self.createConsole(30, "Loading a patch file...")
        self.debugCB("this button doesn't do anything yet\nplease close the console\n")
        # TODO: insert code to load patch from disk (pick from multiple patches)
        self.unlock()
        self.onConsoleClose()
        self.waitConsole()
        
    def imageFormat(self, im, save):
        if(save):
            im.save("tmp_native.png")
            im.save("tmp_patched.png")
                           
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
        
        im = Image.open("tmp_patched.png")
        img3=ImageTk.PhotoImage(im) 
        self.imgLblPatched.configure(image=img3)
        self.imgLblPatched.image=img3

       
    def systemExit(self):
        print("exiting gracefully")
        self._adup.serClose()
        self.win.destroy()
        quit()
        #sys.exit(0)
        
    def setup(self):
        self.win.geometry('+20+20')
        self.win.protocol("WM_DELETE_WINDOW", self.systemExit)
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
    
    
