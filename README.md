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
