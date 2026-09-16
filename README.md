# arduMet

Repository with source code of an Arduino based mini meteorological unit.

This device reads air temperature, relative humidity and pressure using a GY-BME-280 integrated sensor.

This prototype estimates the air density using the Clayperon's equation rewritten to calculate the air density of moist air.

It shows the measurements in a sequencial loop on the LCD and also sends the measurements via USB serial interface.

The measurements alternate with a date /time shown on the screen using the RTC as time base. 

The RCT DS 3231 incorporated to the prototype enables time keeping and grants consistent time stamps for the measured ambient temperature, pressure and humidity.

All data is stored in a pre-formatted (FAT32) SD card in a structured text file (data.txt) that can be easily imported by Excel, for instance.

Please follow these guidelines regarding connection and formating the SD card:
https://lastminuteengineers.com/arduino-micro-sd-card-module-tutorial/


The necessary libraries for this project are included in the repository (zip files) but updated versions can be dowloaded freely:
 - https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
 - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
 - Real time clock DS3231: https://github.com/jarzebski/Arduino-DS3231
 - SD Card Reader: https://github.com/arduino-libraries/SD 
 - 1602 I2C LCD: https://github.com/Freenove/Freenove_LCD1602_Starter_Kit/archive/master.zip


Feel Free to use as you please.

Maintained by Filipe Brandao (budegarower@gmail.com).

RELEASE NOTES:

V.1.4.0 - replaced the Sunman SMS0408E2 by a generic 16 x 2 Alphanumeric LCD, main was modified
to better display real teime measurements as well as showing time and date.

v1.3.0 - Incorporated DS3231 RTC (real time clock) to include a timestamp for data logging. Incorporated SD Card reader for data storage.
New schematic includes the hardware modifications to include the two new devices and the the possibility of optional use of the SMS0408E2 7 segments LCD.
The data log can work without the LCD, the slot for the LCD connection can be left open. Data is stored in a SD card and measurements can be seen via USB serial connection.

v1.2.0 - Included function for estimation of air density using the calculation methodology described here:
https://www.omnicalculator.com/physics/air-density. Calculated density is displayed in LCD and sent by USB serial.

v1.1.0 - First hardware/software version. Just shows temperatre (°C), relative humidity (%) and pressure (millibar) in the 7 segments LCD.
Every 1s it alternates the shown measures and sends them via USB serial.
