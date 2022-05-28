// Version 1.1

#include <TinyGPS++.h>

//*** ShiftPWM Setup ***//
const int ShiftPWM_latchPin = 9; //Latch Pin for Shift-Register
const bool ShiftPWM_invertOutputs = true; //Invert Outputs
const bool ShiftPWM_balanceLoad = true; //If true, only one Output will be on at the time (Power Saving)
#include <ShiftPWM.h>   // including ShiftPWM.h after setting the pins

unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 90;
int numRegisters = 3;
int numRGBleds = numRegisters * 8 / 3;
int Brightness = 128;
int green[3] = { 0, 128, 0};
int yellow[3] = { 128, 64, 0};
int red[3] = { 128, 0, 0};
int blue[3] = { 0, 0, 128};


//*** Definition of operational ranges ***//

int oilPressRange[] = { 0 , 8 , 20 , 50 , 70};
int oilTempRange[] = { 0 , 50 , 110 , 130 , 150};
int egtRange[] = { 850, 880, 1000};
int CHTTempRange[] = {0, 120, 150};
int fuelPressureRange[] = {0, 15, 50, 55}; // ( 1/100 Bar)
int fuelLHRange[] = {0, 40, 100};
int fuelRHRange[] = {0, 40, 100};
int BatChargeRange[] = {10};  //If smaller than 1V -> green. Otherwise red.


//TIMEOUT 3 SEC -< blau


// The TinyGPS++ object
TinyGPSPlus motorStream;
TinyGPSCustom RPM(motorStream, "ICMB912", 1);
TinyGPSCustom manPress(motorStream, "ICMB912", 2);
TinyGPSCustom oilPress(motorStream, "ICMB912", 3);
TinyGPSCustom fuelPress(motorStream, "ICMB912", 4);
TinyGPSCustom oilTemp(motorStream, "ICMB912", 5);
TinyGPSCustom CHT1(motorStream, "ICMB912", 6);
TinyGPSCustom CHT2(motorStream, "ICMB912", 7);
TinyGPSCustom OAT(motorStream, "ICMB912", 8);
TinyGPSCustom fuelLH(motorStream, "ICMB912", 9);
TinyGPSCustom fuelRH(motorStream, "ICMB912", 10);
TinyGPSCustom fuelFlow(motorStream, "ICMB912", 11);
TinyGPSCustom fuelSum(motorStream, "ICMB912", 12);
TinyGPSCustom EGT1(motorStream, "ICMB912", 13);
TinyGPSCustom EGT2(motorStream, "ICMB912", 14);
TinyGPSCustom flaps(motorStream, "ICMB912", 15);
TinyGPSCustom volts(motorStream, "ICMB912", 16);
TinyGPSCustom amps(motorStream, "ICMB912", 17);


void setup() {
  Serial.begin(19200);

  //Init Shiftregisters
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.SetPinGrouping(1);
  ShiftPWM.Start(pwmFrequency, maxBrightness);

  //Set all LEDs to blue
  for (int led = 0; led < numRGBleds; led++) {
    setLED(led, "blue");
  }
  //End init Shiftregisters

}


void loop() { //SetLEDs Everytime a value is Updated.
  if (oilPress.isUpdated()) {
    setOilPress();
  }
  if (fuelPress.isUpdated()) {
    setFuelPress();
  }
  if (oilTemp.isUpdated()) {
    setOilTemp();
  }
  if (CHT1.isUpdated()) {
    setCHTTemp();
  }
  if (fuelLH.isUpdated()) {
    setFuelLH();
  }
  if (fuelRH.isUpdated()) {
    setFuelRH();
  }
  if (volts.isUpdated()) {
    setBatCharge();
  }
  if (EGT1.isUpdated()) {
    setEGT();
  }

  while (Serial.available() > 0) {
    motorStream.encode(Serial.read());
  }
}

void setOilPress() {
  //Set Oil Pressure LED
  int currOilPress = atoi(oilPress.value());
  if (currOilPress <= oilPressRange[1]) {
    setLED(7, "red");
  } else if (currOilPress >= oilPressRange[1] &&  currOilPress <= oilPressRange[2]) {
    setLED(7, "yellow");
  } else if (currOilPress >= oilPressRange[2] &&  currOilPress <= oilPressRange[3]) {
    setLED(7, "green");
  } else if (currOilPress >= oilPressRange[4] && currOilPress <= oilPressRange[5]) {
    setLED(7, "yellow");
  }
  //End Oil Pressure

}
void setFuelLH() {
  //Set Fuel LH LED
  int currfuelLH = atoi(fuelLH.value());
  if (currfuelLH <= fuelLHRange[1] ) {
    setLED(6, "red");
  } else if (currfuelLH <= fuelLHRange[2] ) {
    setLED(6, "yellow");
  } else if (currfuelLH >= fuelLHRange[2] ) {
    setLED(6, "green");
  }
  //End Fuel LH
}

void setFuelRH() {

  //Set Fuel RH LED
  int currfuelRH = atoi(fuelRH.value());
  if (currfuelRH <= fuelRHRange[1] ) {
    setLED(1, "red");
  } else if (currfuelRH <= fuelRHRange[2] ) {
    setLED(1, "yellow");
  } else if (currfuelRH >= fuelRHRange[2] ) {
    setLED(1, "green");
  }
  //End Fuel RH
}
void setOilTemp() {

  //Set Oil Temp LED
  int currOilTemp = atoi(oilTemp.value());
  if (currOilTemp <= oilTempRange[1]) {
    setLED(5, "yellow");
  } else if (currOilTemp >= oilTempRange[1] &&  currOilTemp <= oilTempRange[2] ) {
    setLED(5, "green");
  } else if (currOilTemp >= oilTempRange[2] &&  currOilTemp <= oilTempRange[3]) {
    setLED(5, "yellow");
  } else if (currOilTemp >= oilTempRange[3] &&  currOilTemp <= oilTempRange[4]) {
    setLED(5, "red");
  }
  //End Oil Temp
}
void setFuelPress() {

  //Set Fuel Pressure LED
  int currFuelPress = atoi(fuelPress.value());
  if (currFuelPress < fuelPressureRange[1] ) {
    setLED(4, "red");
  } else if (currFuelPress >= fuelPressureRange[1] &&  currFuelPress <= fuelPressureRange[2]) {
    setLED(4, "green");
  } else if ((currFuelPress >= fuelPressureRange[4] && currFuelPress <= fuelPressureRange[3] )) {
    setLED(4, "red");
  }
  //End Fuel Pressure
}

void setBatCharge() {
  //Set Volts LED
  int currCharge = atoi(amps.value());
  Serial.println(currCharge);
  if ((currCharge < BatChargeRange[0] )) {
    setLED(3, "red");
  } else if (currCharge >= BatChargeRange[0]  || currCharge <= BatChargeRange[1]) {
    setLED(3, "green");
  }
  //End Volts
}

void setCHTTemp() {

  //Set CHT Temp LED
  int currCHTTemp = atoi(CHT1.value());
  if ((currCHTTemp <= CHTTempRange[2] ) &&  (currCHTTemp >= CHTTempRange[1] )) {
    setLED(2, "red");
  } else if (currCHTTemp <= CHTTempRange[1]) {
    setLED(2, "green");
  }
  //End CHT Temp
}

void setEGT() {
  //Set EGT LED
  char currEGT = atoi(EGT1.value());
  if ((currEGT <= egtRange[0]) ) {
    setLED(0, "green");
  } else  if ((currEGT >= egtRange[0]) && (currEGT <= egtRange[1]) ) {
    setLED(0, "yellow");
  } else if ((currEGT >= egtRange[1]) && (currEGT <= egtRange[2]) ) {
    setLED(0, "red");
  }
  //End EGT
}


void setLED(int LEDNumber, String color) {
  if (color == "red") {
    ShiftPWM.SetRGB(LEDNumber, red[0], red[1], red[2]);           //red
  } else if (color == "yellow") {
    ShiftPWM.SetRGB(LEDNumber, yellow[0], yellow[1], yellow[2]);  //yellow
  } else if (color == "green") {
    ShiftPWM.SetRGB(LEDNumber, green[0], green[1], green[2]);     //green
  } else if (color == "blue") {
    ShiftPWM.SetRGB(LEDNumber, blue[0], blue[1], blue[2]);     //blue
  } else if (color == "off") {
    ShiftPWM.SetRGB(LEDNumber, 0, 0, 0);     //off
  }
}
