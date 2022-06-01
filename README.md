# Cyberdyne
 
 A collaborative effort to defeat skynet as part of the 2022 ADI GTC Hackathon. 
 
 Team members:
 - Saurabh Jain
 - Abhijit Mahurkar
 - Hema Komarina
 - Erik Maclean
 - Jared Eldredge
 
Firmware dev getting started
- Install the MaximSDK for MAX78000 (support is most mature on Windows)
- Checkout this repo
- Launch the SDK's eclipse variant; I suggest that you start with a fresh workspace but it shouldn't be neccesary
- Click: File -> open Projects from Filesystem
- Top right, click Directory and navigate to the checked out repo: Cyberdyne/Eclipse/Cyberdyne_proj
- Click Select Folder to pick Cyberdyne_proj
- Eclipse will oad the project by default
- Click the hammer icon at top left to build the project
- Expectations: build success; Cyberdyne.elf may now be programmed to the board using either the IDE or CLI tools

PC GUI getting started
- Install python 3.x
- pip install: pyserial, numpy
- launch the GUI after connecting a programmed MCU to the PC via USB-UART serial port
- Expectations: PC GUI will scan all enumerated serial ports for one that responds to the protocol used by this demo; it will connect to the first such serial port after which the GUI may be used to fully control the demo

Status
- Functional GUI
- Adversarial patching
- Trained image classifier (robot/human)
- Trained voice classifier (dog, love, disk)
- Image upload/download from the board/pc
- 
