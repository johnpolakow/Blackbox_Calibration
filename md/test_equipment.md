

Test Equipment Setup


Equipment Needed
    - Calibration Box (with Raspberry pi installed)
    - Raspberry Pi DUT
    - Yokogawa WT310E
    - HP34401 #1
    - HP34401 #2
    - Agilent E3648 PS
    - USB cable for Yokogawa meter
    - 3x RS232 -> USB adapter cables
    - 3x null modem cables
    - USB Hub


The rough connections of the test equipment are shown below:   

![plot](./md/equipment/pi_box.png)    
not shown in this diagram are:    
  - connections from Agilent PS to the Calibration Box   
  - +5.5V power supply to the Calibration Box    
  - USB->RS232 connections   
  - CCC + IDCA   

Here is the stack of meters used in the test setup. From top to bottom: Yokogawa WT310E, HP34401, HP34401, E3648:    
![plot](./md/equipment/meters.JPG)    

Power supply used with Calibration Box, set to +5.5V. Do not go above +5.5V:    
![plot](./md/equipment/cal_PS.JPG)    

Labeled connections on the back of the Calibration Box:    
![plot](./md/equipment/back_cal_box_labeled.JPG)    

Labeled connections on the top of the Calibration Box:    
![plot](./md/equipment/relay_box_top.JPG)    

Thermocouples attached to isothermal surface:    
![plot](./md/equipment/isothermal.JPG)    

Ethernet cable plugged in to Calibration Box:
![plot](./md/equipment/cal_eth.JPG)  


The pinout for the Pi DB9 Header is shown below:
![plot](./md/equipment/pinout.JPG)    
