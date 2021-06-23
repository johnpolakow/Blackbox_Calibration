# Blackbox_Calibration

This program runs on the Relay Calibration Box (on a Raspberry Pi). This program performs the following tasks:
searches local Ethernet network for attached Raspberry Pis to calibrate
connects to and controls the following test equipment
  - HP34401A x 2
  - Yokogawa WT310E
  - Agilent E3648A

Calibration can be manual or Automated
The following metrics may be calibrated:
  - AC Cooler Current
  - DC Volts to Cooler
  - Diode Volts
  - Diode Current
  - Load Resistor Voltage
  - Load Resistor Current

After calibration runs have been performed, the datalog files will be compiled into lookup tables. The data is sorted, filtered, and averaged to produce accurate LUT points. C code header files are created which contain all the LUT points. After all LUTs have been created, the LUT files are sent via SSH to the Pi being calibrated. Then the Pi DAQ firmware is compiled, with the new LUT files. Finally, the DAQ Server process on the Pi is started.

## Downloading Code and Compiling
to use this code, first make a dir for the repo, then clone the repo with:  
mkdir /home/pi/Cal_Station  
cd /home/pi/Cal_Station  
git clone https://github.com/johnpolakow/Blackbox_Calibration.git  


there is a makefile present, to compile the code just type 'make':
make

a number of modules will be succesively compiled. If you're doing it from the Pi, it will take 2-3 minutes to finish compiling. Required libraries to compile this code are:
   - m (math)
   - bcm2835 (raspberry pi gpio library)
   - ssh (libssh, a library for ssh access. See folder 'libssh_install_notes' for instructions on compiling libssh on the pi from source

after the calibration program has built successfully, a creatively named binary, '**cal**', is produced. 
to execute the code, simply type: ./cal  
Note: do not use sudo, it is not necessary, and will look in the wrong directory for ssh keys if the program is run as root user.

Required Equipment Connections:
  HP34401  RS232->USB  
  HP34401  RS232->USB  
  Yokogawa WT310E -> USB  
  Agilent E3648A RS232->USB  
  

A USB Hub can be used to connect all these USB devices, then the Pi can be connected to the hub.

## Pi DUT connection:
the Pi unit to be calibrated needs to be connected to the Pi residing in the calibration box, via Ethernet cable. The subnet for this network is 192.168.123.XXX
The default IP for the calibration box Pi is 192.168.123.7

## Program Start:
the application will try to connect to the 4 pieces of test equipment:
   - 2x HP34401  
   - E3648A  
   - Yokogawa power meter  
If it is unable to find all 4, the program quits. Something to keep in mind:
the application identifies the HP meters and Agilent Power Supply via their individual ID#. This is the value returned by executing the SCPI query: \*IDN?
over RS232 or gpib. The point of this is: if you use different meters or power supply, they will not automatically be identified correctly. This application at startup does list the ID strings of the attached equipment however. To use different meters (same model of meter, just not exact same physical item) read the ID strings at program startup, and then adjust the following strings in Cal_Parameters.cpp:
const char* HP_A_ID = "0,11-5-2";  
const char* HP_B_ID = "0,2-1-1";  
const char* AGILENT_PS_ID = "0,1.7-5.0-1.0";  

Also, if the Raspberry pi inside the Relay calibration box is changed, the MAC Address of the replacement Pi needs to updated in the same Cal_Parameters.cpp. Edit this parameter:
const char* CAL_STATION_MAC_ADDR = "B8:27:EB:2E:B7:3A";  

This is used during auto detection of pis connected to the network. It is necessary to filter out the PI in the calibration box, so it is not thought of as a potential calibration candidate.

There are other string definitions in the Cal_Parameters.cpp file, it is probably best to avoid making changes. The lcoations of various directories are defined.
DC Voltage Points are also defined for the diode calibration, and for calibration of the PI DAQ output voltage measurement (to CCC).

In Cal_Parameters.h are some parameters that adjust how data is filtered and grouped into lookup tables. Some of these apply to different LUTs that are created. 

## PROGRAM OPERATION

Menus:

There is the option to perform calibration automatically or manually. Manual calibration is somewhat a misnomer, the calibration is still automatic is just entails selecting the calibration items manually to run. There is no manual data collection necessary. 






