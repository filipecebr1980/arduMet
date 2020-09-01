/* Meteorological Temp, humid and Pressure Unit
*Created by Filipe Brandao Using
*Sparkfun GY-BME280 Library
*SMS0408E2 Library
*Using examples from Adafruit and Sparkfun Libraries.
*
   The GY-BME280 is using address 0x76 (jumper closed)

  Hardware connections:
  BME280 -> Arduino
  GND -> GND
  3.3 -> 3.3
  SDA -> A4
  SCL -> A5

  SUNMAN SMS0408E2
  LCD   -> Arduino
  VDD   -> 12
  DI    -> 11
  VSS   -> 10
  CLK   -> 9
  
 REQUIRES the following Arduino libraries:
 - https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
 - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
 - SUNMAN SMS0408E2 LDC 7 Segments display: https://github.com/filipecebr1980/Sunman-SMS0408E2
*/

#include <Adafruit_Sensor.h>
#include <Sms0408.h>
#include <Wire.h>
#include "SparkFunBME280.h"

uint32_t delayMS=2000;

int VDD_PIN=12;
int DI_PIN=11;
int VSS_PIN=10;
int CLK_PIN=9;
int BLA_PIN=8;
int BLK_PIN=7;

//create an LCD object
Sms0408 myLCD(DI_PIN,CLK_PIN,BLK_PIN);

//create sensor object
BME280 mySensor;

void setup() {
  
  //Configure Pins as OUTPUT
  pinMode(VSS_PIN,OUTPUT);
  pinMode(VDD_PIN,OUTPUT);
  pinMode(CLK_PIN,OUTPUT);
  pinMode(DI_PIN,OUTPUT);
  pinMode(BLA_PIN,OUTPUT);
  pinMode(BLK_PIN,OUTPUT);

  //powers up SUNMAN SMS0408
  digitalWrite(VDD_PIN, HIGH); 
  digitalWrite(VSS_PIN, LOW);
  digitalWrite(BLA_PIN, HIGH);
  digitalWrite(BLK_PIN, LOW);
  
  Serial.begin(9600);
  Serial.println("System started");
  
  Wire.begin();
  mySensor.setI2CAddress(0x76); //Connect to sensor
  if(mySensor.beginI2C() == false) {
      Serial.println("Sensor B connect failed!");
      myLCD.displayError();
      myLCD.adjust();
      while(1){}
    }

  Serial.println("Sensor conection OK!");

  //tests lcd (all segments ON for 1 second
  testLcd();
  delay(delayMS);
}

void loop() {
  //shows temperature
  displayTemp(mySensor.readTempC());
  delay(delayMS);
  sendSerial();
  
  //shows humidity
  displayHumid(mySensor.readFloatHumidity());
  delay(delayMS);
  sendSerial();
  
  //shows pressure
  displayPressure(mySensor.readFloatPressure()/100.0);
  delay(delayMS);
  sendSerial();
}

void displayTemp(float temp){

  if (temp>0.0){

  myLCD.clearLCD();  
  myLCD.displayInt((int)temp);
  myLCD.codig(18,0);
  myLCD.adjust();
  }
  else {
    
    }
  
}

void displayHumid(float humid){

  if (humid >100.0){
    humid=99.0;    
    }

  myLCD.clearLCD(); 
  myLCD.showColumn();
  myLCD.codig(17,0);
  myLCD.codig(16,0);
  myLCD.displayInt((int)humid);
  myLCD.adjust();
 
}

void displayPressure(float pressure){

  myLCD.clearLCD(); 
  
  if(pressure < 1000.0){
    myLCD.displayFloatAuto(pressure);    
  }
  else
  {
    myLCD.displayInt((int)pressure);
  }
  myLCD.adjust();
}

void testLcd(){
  //fills LCD (all segments on- good to test if has any 
  //bad segment
  myLCD.fillLCD();    
  delay(1000);
  myLCD.clearLCD();
}

void sendSerial(){
  Serial.println((String)mySensor.readTempC()+";" + (String)mySensor.readFloatHumidity()+ ";" + (String)mySensor.readFloatPressure());
}
