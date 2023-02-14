#include <max17320.h>
#include <SBS.h>
#include <max17215.h>
#include <ds2782.h>

#define CHARGE 5
#define DISCHARGE 4

#define MIN_CURRENT 250
//#define MIN_CURRENT 500
#define MIN_CAPACITY 3

SBS stnd;
MAX17215 wire215; 
MAX17320 wire320;
DS2782 DS;

String str = "";
bool start = false;
bool charging = false;
bool discharging = false;
bool c_charging = false;
bool c_discharging = false;
bool sen = true;
uint8_t n = 0;
uint8_t type = 0; //=1 false =0
bool cells = true; //true = 4 cells

uint16_t MAX_VOLTAGE = 16550;
uint16_t MIN_VOLTAGE = 14260;

void(* resetFunc) (void) = 0;

void setup() {
  TWBR=144;
  Serial.begin(9600);
  pinMode(CHARGE, OUTPUT);
  pinMode(DISCHARGE, OUTPUT);
  digitalWrite(CHARGE, LOW);
  digitalWrite(DISCHARGE, LOW);
  if(stnd.scan() == 0){
    if(wire215._readType() == 0x50){
      type = 1;
    }else if(wire215._readType() == 0x90){
      type = 2;
    }else{
      delay(100);
      resetFunc();
      type = 3;
    }
  }else{
    if(DS._readAddress() == 104){
      type = 4;
    }
  }
  //Serial.print("Type: ");
  //Serial.println(type);
  cellNumber();
  Serial.println(MAX_VOLTAGE);
  Serial.println(MIN_VOLTAGE);
  Serial.println(MIN_CURRENT);
}

void loop() {

  switch (type){
    case 0:
    {
      str = "@" + stnd.GetData()+"," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#";
    }
      delay(400);
      break;
    case 1:
    {
      str = "@" + wire215.GetData()+"," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#";
    }
      delay(400);
      break;
    case 2:
    {
      str = "@" + wire320.GetData()+"," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#";
    }
      delay(400);
      break;
    case 4:
    {
      str = "@" + DS.GetData()+"," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#";
    }
  }  

  if (charging == true) {
    switch (type){
      case 0:
        if ((stnd.GetVol() >= MAX_VOLTAGE) && (stnd.GetCur() <= MIN_CURRENT)) {  //4 элемента
          digitalWrite(CHARGE, LOW);
          charging = false;
        }
        break;
      case 1:
        if ((wire215._readVoltage() >= MAX_VOLTAGE) && (wire215._readCurrent() <= MIN_CURRENT)) {  //4 элемента
          digitalWrite(CHARGE, LOW);
          charging = false;
        }
        break;
      case 2:
        if ((wire320._readVoltage() >= MAX_VOLTAGE) && (wire320._readCurrent() <= MIN_CURRENT)) {  //4 элемента
          digitalWrite(CHARGE, LOW);
          charging = false;
        }
      case 4:
        if ((DS._readVoltage() >= MAX_VOLTAGE) && (DS._readCurrent() <= MIN_CURRENT)) {  //4 элемента
          digitalWrite(CHARGE, LOW);
          charging = false;
        }
        break;
    }
  }

  if (discharging == true) {
    switch(type){
      case 0:
        if ((stnd.GetVol() <= MIN_VOLTAGE)) { //||(stnd.GetCap()<=1)) {  //4 элемента
          digitalWrite(DISCHARGE, LOW);
          discharging = false;
        }
        break;
      case 1:
        if ((wire215._readVoltage() <= MIN_VOLTAGE)) { //||(stnd.GetCap()<=1)) {  //4 элемента
          digitalWrite(DISCHARGE, LOW);
          discharging = false;
        }
        break;
      case 2:
        if ((wire320._readVoltage() <= MIN_VOLTAGE)) { //||(stnd.GetCap()<=1)) {  //4 элемента
          digitalWrite(DISCHARGE, LOW);
          discharging = false;
        }
      case 4:
        if ((DS._readVoltage() <= MIN_VOLTAGE)) { //||(stnd.GetCap()<=1)) {  //4 элемента
          digitalWrite(DISCHARGE, LOW);
          discharging = false;
        }
        break;
    }
  }

  if (start == true) {
    if (n < 7) {
      switch(type){
        case 0:
          if ((stnd.GetVol() < MAX_VOLTAGE) && (stnd.GetCur() <= MIN_CURRENT) && (c_discharging == false)) { //4 элемента        
            c_charging = true;
            c_discharging = false;
          } else if ((stnd.GetVol() >= MAX_VOLTAGE) && (stnd.GetCur() <= MIN_CURRENT)) { //4 элемента
            c_charging = false;
            c_discharging = true;
          }
          if ((stnd.GetVol() > MIN_VOLTAGE) && (stnd.GetCap() >= MIN_CAPACITY) && (c_charging == false)) {  //4 элемента
            c_charging = false;
            c_discharging = true;
          } else if ((stnd.GetVol() <= MIN_VOLTAGE) && (stnd.GetCap() <= MIN_CAPACITY)) { //4 элемента
            c_discharging = false;
            c_charging = true;
          }
          break;
        case 1:
          if ((wire215._readVoltage() < MAX_VOLTAGE) && (wire215._readCurrent() <= MIN_CURRENT) && (c_discharging == false)) { //4 элемента        
            c_charging = true;
            c_discharging = false;
          } else if ((wire215._readVoltage() >= MAX_VOLTAGE) && (wire215._readCurrent() <= MIN_CURRENT)) { //4 элемента
            c_charging = false;
            c_discharging = true;
          }
          if ((wire215._readVoltage() > MIN_VOLTAGE) && (wire215._readCapacity() >= MIN_CAPACITY) && (c_charging == false)) {  //4 элемента
            c_charging = false;
            c_discharging = true;
          } else if ((wire215._readVoltage() <= MIN_VOLTAGE) && (wire215._readCapacity() <= MIN_CAPACITY)) { //4 элемента
            c_discharging = false;
            c_charging = true;
          }
          break;
        case 2:
          if ((wire320._readVoltage() < MAX_VOLTAGE) && (wire320._readCurrent() <= MIN_CURRENT) && (c_discharging == false)) { //4 элемента        
            c_charging = true;
            c_discharging = false;
          } else if ((wire320._readVoltage() >= MAX_VOLTAGE) && (wire320._readCurrent() <= MIN_CURRENT)) { //4 элемента
            c_charging = false;
            c_discharging = true;
          }
          if ((wire320._readVoltage() > MIN_VOLTAGE) && (wire320._readCapacity() >= MIN_CAPACITY) && (c_charging == false)) {  //4 элемента
            c_charging = false;
            c_discharging = true;
          } else if ((wire320._readVoltage() <= MIN_VOLTAGE) && (wire320._readCapacity() <= MIN_CAPACITY)) { //4 элемента
            c_discharging = false;
            c_charging = true;
          }
        case 4:
          if ((DS._readVoltage() < MAX_VOLTAGE) && (DS._readCurrent() <= MIN_CURRENT) && (c_discharging == false)) { //4 элемента        
            c_charging = true;
            c_discharging = false;
          } else if ((DS._readVoltage() >= MAX_VOLTAGE) && (DS._readCurrent() <= MIN_CURRENT)) { //4 элемента
            c_charging = false;
            c_discharging = true;
          }
          if ((DS._readVoltage() > MIN_VOLTAGE) && (DS._readCapacity() >= MIN_CAPACITY) && (c_charging == false)) {  //4 элемента
            c_charging = false;
            c_discharging = true;
          } else if ((DS._readVoltage() <= MIN_VOLTAGE) && (DS._readCapacity() <= MIN_CAPACITY)) { //4 элемента
            c_discharging = false;
            c_charging = true;
          }
          break;          
        }      
    } else {
      c_charging = false;
      c_discharging = false;
      digitalWrite(CHARGE, LOW);
      digitalWrite(DISCHARGE, LOW);
      start = false;
    }
    c_cycling();
    d_cycling();
  }
  
  while (Serial.available() > 0) {
    int input = Serial.read();
    switch (input) {
      case 'c':
        sen = false;
        if (charging == false) {
          charging = true;
          discharging = false;
          c_charging = false;
          c_discharging = false;
          charge();
        } else {
          digitalWrite(CHARGE, LOW);
          charging = false;
          c_charging = false;
          c_discharging = false;
        }
        sen = true;
        break;
      case 'd':
        sen = false;
        if (discharging == false) {
          discharging = true;
          charging = false;
          c_charging = false;
          c_discharging = false;
          discharge();
        } else {
          digitalWrite(DISCHARGE, LOW);
          discharging = false;  
          c_charging = false;
          c_discharging = false;        
        }
        sen = true;
        break;
      case 'y':
        if (start == false) {
          start = true;
          charging = false;
          discharging = false;
        } else {
          start = false;
          c_charging = false;
          c_discharging = false;
          charging = false;
          discharging = false;
          digitalWrite(CHARGE, LOW);
          digitalWrite(DISCHARGE, LOW);
          n = 0;
        }
        break;
      case 's':
        if(type == 0){
          sen = false;
          setSerial();
        }
        break;
      case 'e':
        if(type == 0){
          sen = false;
          setDate();
        }
        break;
      case 'f':
        if(type == 0) stnd.FullAccess();
        else if(type == 2) wire215._fReset();       
        break;
      case 'u':
        if(type == 0) stnd.Unseal();
        break;
      case 'l':
        if(type == 0) stnd.Seal();
        break;
      case 'x':
        start = false;
        c_charging = false;
        c_discharging = false;
        charging = false;
        discharging = false;
        digitalWrite(CHARGE, LOW);
        digitalWrite(DISCHARGE, LOW);
        resetFunc();
        break;
      case 'k':
        if(sen == true){
          //sendData();
          Serial.println(str);
          Serial.flush();
        }
        break;            
    }
  }
  //if (sen){
  //  Serial.println(str);
  //  Serial.flush();
  //}
}

void sendData() {
  switch(type){
    case 0:
      {
        String str = "@" + stnd.GetData();
        Serial.print(str);    
      }
      {
        String str2 = "," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#";
        Serial.println(str2);
      }
      //String str = "@" + stnd.GetData()+"," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#";
      //Serial.println("@" + stnd.GetData()+"," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#");
      //Serial.flush();
      Serial.flush(); 
      break;
    case 1:
      {
        String str = "@" + wire215.GetData();
        Serial.print(str);
      }
      {
        String str2 = "," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#";
        Serial.println(str2);
      }
      Serial.flush(); 
      break;
    case 2:
      {
        String str = "@" + wire320.GetData();
        Serial.print(str);
      }
      {
        String str2 = "," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#";
        Serial.println(str2);
      }
      Serial.flush(); 
    case 4:
      {
        String str = "@" + DS.GetData();
        Serial.print(str);
      }
      {
        String str2 = "," + String(c_charging) + "," + String(c_discharging) + "," + String(charging) + "," + String(discharging) + "#";
        Serial.println(str2);
      }
      Serial.flush(); 
      break;      
  }
  while(Serial.available()) Serial.read();
}

void charge() {
  digitalWrite(CHARGE, HIGH);
  digitalWrite(DISCHARGE, LOW);
}

void discharge() {
  digitalWrite(DISCHARGE, HIGH);
  digitalWrite(CHARGE, LOW);
}

void c_cycling() {
  if ((c_charging == true) && (digitalRead(CHARGE) == LOW)) {
    digitalWrite(CHARGE, HIGH);
    digitalWrite(DISCHARGE, LOW);
    n = n + 1;
  } else if ((c_charging == false) && (digitalRead(CHARGE) == HIGH)) {
    digitalWrite(CHARGE, LOW);
  }
}

void d_cycling() {
  if ((c_discharging == true) && (digitalRead(DISCHARGE) == LOW)) {
    digitalWrite(DISCHARGE, HIGH);
    digitalWrite(CHARGE, LOW);
    n = n + 1;
  } else if ((c_discharging == false) && (digitalRead(DISCHARGE) == HIGH)) {
    digitalWrite(DISCHARGE, LOW);
  }
}

void setSerial() {
  Serial.println("Setting serial");
  int res = 0;
  String str = "";
  while (Serial.available() > 0) {
    if (Serial.available() > 0) {
      //char c = Serial.read();
      //str += c;
      String c = Serial.readString();
      str += c;
    }
  }
  if (str.length() > 0) {
    res = str.substring(0, 5).toInt();
  }
  stnd.SetSerial((res));
  //Serial.flush();
  str = "";
  sen = true;
}

void setDate() {
  Serial.println("Setting date");
  int res = 0;
  String str = "";
  while (Serial.available() > 0) {
    //delay(3);
    if (Serial.available() > 0) {
      //char c = Serial.read();
      //str += c;
      String c = Serial.readString();
      str += c;
    }
  }
  if (str.length() > 0) {
    res = str.substring(0, 5).toInt();
  }
  stnd.SetDate((res));
  //Serial.flush();
  str = "";  
  sen = true;
}

void cellNumber(){
  switch(type){
    case 0:
      if(stnd.readCellsNum() == 2){
        Serial.println("2 cells");
        MAX_VOLTAGE = 8250;
        MIN_VOLTAGE = 5900;
      }else if(stnd.readCellsNum() == 3){
        Serial.println("4 cells");
      // MAX_VOLTAGE = 12350;
      // MIN_VOLTAGE = 11970;      
      }
      break;
    case 1: 
      if(wire215._cellNum() == 1){
        Serial.println("1 cells");
        MAX_VOLTAGE = 4200;
        MIN_VOLTAGE = 2950;      
      }else if(wire215._cellNum() == 2){
        Serial.println("2 cells");
        MAX_VOLTAGE = 8250;
        MIN_VOLTAGE = 5900;      
      }else if(wire215._cellNum() == 3){        
        Serial.println("3 cells");
      // MAX_VOLTAGE = 16550;
      // MIN_VOLTAGE = 14260;
      }else if(wire215._cellNum() == 4){
        Serial.println("4 cells");
        // MAX_VOLTAGE = 16550;
      // MIN_VOLTAGE = 14260;
      }
      break;
    case 2:    
      if(wire320._cellNum() == 2){      
        Serial.println("2 cells");
        MAX_VOLTAGE = 8250;
        MIN_VOLTAGE = 5900;      
      }else if(wire320._cellNum() == 3){        
        Serial.println("3 cells");
      // MAX_VOLTAGE = 16550;
      // MIN_VOLTAGE = 14260;
      }else if(wire320._cellNum() == 4){
        Serial.println("4 cells");
        // MAX_VOLTAGE = 16550;
      // MIN_VOLTAGE = 14260;
      }
    case 4:
      MIN_VOLTAGE = 11000;
      MAX_VOLTAGE = 16650;
      break;
  }
}
