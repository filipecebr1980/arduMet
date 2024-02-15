# arduMet

Repository with source code of Arduino based cardboard meteorological unit.

This device reads air temperature, relative humidity and pressure using a GY-BME-280 integrated sensor.

This prototype estimates the air density using the Clayperon equation rewritten to calculate the air density of moist air.

It shows the measurements in a sequencial loop in simple 4 digits 7 segments LCD and also sends the measurements via USB serial interface.
The measurements' sequence in the simple 4 digits LCD is: temperature, % humidity, pressure, density. 

All data is stored in a pre formatted (FAT32) SD card in a tructured text file (data.txt) that can be easily imported in Excel, for instance. 

The necessary libraries for this project are included in the repository (zip files) but updated versions can be dowloaded freely:
 - https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
 - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
 - Real time clock DS3231: https://github.com/jarzebski/Arduino-DS3231
 - SD Card Reader: https://github.com/arduino-libraries/SD
 - SUNMAN SMS0408E2 LDC 7 Segments display: https://github.com/filipecebr1980/Sunman-SMS0408E2


Feel Free to use as you please.

By Filipe Brandao

RELEASE NOTES:

v1.3.0 - Incorporated DS3231 RTC (real time clock) to include a timestamp for dataloging. Incorparated SD Card reader for data storage.
New schematic includes the hardware modifications to include the two new devices and the the possibility of optional use of the SMS0408E2 7 segments LCD.
The datalog can work without the LCD, the slot for the LCD connection can be left open. Data is stored in a SD card and measurements can be seen via USB serial connection.

v1.2.0 - Included function for estimation of air density using the calculation methodology described here:
https://www.omnicalculator.com/physics/air-density. Calculated density is displayed in LCD and sent by USB serial.

v1.1.0 - First hardware/software version. Just shows temperatre (°C), relative humidity (%) and pressure (millibar) in the 7 segments LCD.
Every 1s it alternates the shown measures and sends them via USB serial.
