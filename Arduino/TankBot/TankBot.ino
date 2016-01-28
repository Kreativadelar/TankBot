/*
**   _   __               _   _                _      _            
**  | | / /              | | (_)              | |    | |           
**  | |/ / _ __ ___  __ _| |_ ___   ____ _  __| | ___| | __ _ _ __ 
**  |    \| '__/ _ \/ _` | __| \ \ / / _` |/ _` |/ _ \ |/ _` | '__|
**  | |\  \ | |  __/ (_| | |_| |\ V / (_| | (_| |  __/ | (_| | |   
**  \_| \_/_|  \___|\__,_|\__|_| \_/ \__,_|\__,_|\___|_|\__,_|_|   
**                                                                 
**
**  Remote controlled robot with Me USB Host and a 2.4GHz Vibration USB Wireless Shock Gamepad
**  to controll the robot,  
**  from Kreativadelar.se, Sweden
**
**  This code is written "quick & dirty" and should not be as a guide
**  in how to program an Arduino. Feel free to change code as you like
**  and share with your friends.
**
**  If you want to share your code changes, please e-mail them to
**  info@kreativadelar.se and we will put them on our web for other
**  customers to download.
**
**  (C) Kreativadelar.se 2015, Sweden, Patrik
**  http://www.kreativadelar.se
**
**  To use this code you need the following libraries: 
**  
**  SoftwareServo which can be
**  downloaded free from http://playground.arduino.cc/ComponentLib/Servo
**  
**  Makeblock Library which can be  
**  downloaded free from https://github.com/Makeblock-official/Makeblock-Libraries/archive/master.zip
**
**  Version 1.0, Initial public release, July 2015
**
**  This example code is in the public domain.
**
*/

#include <EEPROM.h>
#include <SoftwareServo.h>
#include "Wire.h" 
#include "SoftwareSerial.h" 
#include "MeOrion.h" 
 
MeUSBHost joypad(PORT_3); 
MePort port(PORT_6);
MeDCMotor motor1(M1);
MeDCMotor motor2(M2);
MeRGBLed ledStrip(PORT_6, SLOT1, 15);

SoftwareServo  myservo1;  // create servo object to control a servo 
int16_t servo1pin =  port.pin2();//attaches the servo on PORT_3 SLOT1 to the servo object

uint8_t motorSpeed = 100;
int servoPos = 0;
int NUMPIXELS = 15;
bool flashingOn = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600); 
  joypad.init(USB1_0); 
  myservo1.attach(servo1pin);  // attaches the servo on servopin1
  delay(500);
  myservo1.write(servoPos);
  delay(1000);
  buzzerOff();

  ledStrip.setColor(0, 0, 0, 0);
  
}

uint32_t lastBlockTime = 0;
uint32_t lastTime = 0;
void loop() {
  // put your main code here, to run repeatedly:
  if(!joypad.device_online)
  {
    if (millis() - lastTime > 100)
    {
      joypad.probeDevice(); 
    }
  }
  else
  {
    //received data from usb device
    int len = joypad.host_recv();
    if(len>4){
      parseJoystick(joypad.RECV_BUFFER);
    }
  }

  polisFlash(flashingOn);
  ledStrip.show();
  myservo1.write(servoPos);
  SoftwareServo::refresh(); 
}
void parseJoystick(unsigned char * buf)
{
    //debug joystick 
    //int i = 0; 
    //for(i = 0; i < 7; i++) 
    //{ 
      //Serial.print(buf[i]); 
      //Serial.print('-'); 
    //} 
    //Serial.println(buf[7]); 
    //delay(10); 
  
      //parse joystick's data
      uint8_t buttonCode = buf[4]&0xff;
      uint8_t buttonCode_ext = buf[5]&0xff;
      uint8_t joystickCodeL_V = buf[3]&0xff; //top 0 bottom ff
      uint8_t joystickCodeL_H = buf[2]&0xff; //left 0 right ff
      uint8_t joystickCodeR_V = buf[1]&0xff; //top 0 bottom ff
      uint8_t joystickCodeR_H = buf[0]&0xff; //left 0 right ff
      uint8_t directionButtonCode = (buttonCode&0xf);
      uint8_t rightButtonCode = (buttonCode&0xf0)>>4;

      if (joystickCodeL_V == 0){
        motor1.run(-motorSpeed); // value: between -255 and 255.
        motor2.run(-motorSpeed); // value: between -255 and 255.
      }else if(joystickCodeL_V == 255){
        motor1.run(motorSpeed); // value: between -255 and 255.
        motor2.run(motorSpeed); // value: between -255 and 255.
      }else if(joystickCodeL_H == 0){
        motor1.run(-motorSpeed); // value: between -255 and 255.
        motor2.run(motorSpeed); // value: between -255 and 255.
      }else if(joystickCodeL_H == 255){
        motor1.run(motorSpeed); // value: between -255 and 255.
        motor2.run(-motorSpeed); // value: between -255 and 255.
      }else{
        motor1.stop();
        motor2.stop();
      }

      if (joystickCodeR_H == 0 && buttonCode_ext != 128){
        servoPos = servoPos + 5;
        //myservo1.write(0);                  // sets the servo position according to the scaled value 
        //delay(100);
      }else if (joystickCodeR_H == 255 && buttonCode_ext != 128){
        servoPos = servoPos - 5;
        //myservo1.write(180);                  // sets the servo position according to the scaled value 
        //delay(100);
      }else if(buttonCode_ext == 128){
        servoPos = 90;
      }

      if(buttonCode == 31){
        flashingOn = true;
      }else{
        flashingOn = false;
      }
      
      if(buttonCode == 47){
        buzzerOn();
      }else{
        buzzerOff();
      }
 } 


void polisFlash(bool flash){
  static long lastTime = 0;
  static bool last = false;

  if(lastTime + 100 < millis())
  {
    for(int i=0;i<NUMPIXELS;i++){
      if(flash){
        if(last){
          ledStrip.setColorAt(i, 0, 0, 0);
          last = false;
        }else{
          ledStrip.setColorAt(i, 0, 0, 255);
          last = true;
        }
      }else{
        ledStrip.setColor(i, 0, 0, 0);
      }
    }
    lastTime = millis();
  }
}


