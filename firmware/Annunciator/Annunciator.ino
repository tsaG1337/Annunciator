#include "TinyGPS++.h"
TinyGPSPlus gps;

//ShiftPWM Setup
const int ShiftPWM_latchPin = 9; //Latch Pin for Shift-Register
const bool ShiftPWM_invertOutputs = true; //Invert Outputs
const bool ShiftPWM_balanceLoad = true; //If true, only one Output will be on at the time (Power Saving)
#include <ShiftPWM.h>   // including ShiftPWM.h after setting the pins

unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 120;
int numRegisters = 3;
int numRGBleds = numRegisters * 8 / 3;
int currBrightness = 128;

//***Definition of operational ranges***//

int rpmRange[] = {0, 1440, 1800, 5500, 5800, 5850};
// Red_end , yellow_begin, green_begin, green_end, yellow_end, red_begin) (1/10 Bar)
int oilPressRange[] = { 1 , 15 , 15 , 50 , 50 , 70};
int oilTempRange[] = { 1 , 50 , 90 , 110 , 110 , 140};
int egtRange[] = { 880};
int CHTTempRange[] = {120, 150};
int fuelPressureRange[] = {40, 150}; // ( 1/100 Bar)
int fuelLHRange[] = {0, 200, 400};
int fuelRHRange[] = {0, 200, 400};
int voltsRange[] = {10, 14};

int lastUpdate = 0;
int updateTimeout = 100;
char sentence[7] = "$ICMB912";

//***Linking TinyGPS with the incoming data***//
TinyGPSCustom RPM(gps, sentence, 1);
TinyGPSCustom manPress(gps, sentence, 2);
TinyGPSCustom oilPress(gps, sentence, 3);
TinyGPSCustom fuelPress(gps, sentence, 4);
TinyGPSCustom oilTemp(gps, sentence, 5);
TinyGPSCustom CHT1(gps, sentence, 6);
TinyGPSCustom CHT2(gps, sentence, 7);
TinyGPSCustom OAT(gps, sentence, 8);
TinyGPSCustom fuelLH(gps, sentence, 9);
TinyGPSCustom fuelRH(gps, sentence, 10);
TinyGPSCustom fuelFlow(gps, sentence, 11);
TinyGPSCustom fuelSum(gps, sentence, 12);
TinyGPSCustom EGT1(gps, sentence, 13);
TinyGPSCustom EGT2(gps, sentence, 14);
TinyGPSCustom flaps(gps, sentence, 15);
TinyGPSCustom volts(gps, sentence, 16);
TinyGPSCustom amps(gps, sentence, 17);

void setup() {
  Serial.begin(4800);

  //Init Shiftregisters
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.SetPinGrouping(1);
  ShiftPWM.Start(pwmFrequency, maxBrightness);
  // Turn all LED's off.
  ShiftPWM.SetAll(0);

  for (int hue = 0; hue < 360; hue++) {
    ShiftPWM.SetAllHSV(hue, 255, 255);
    delay(5);
  }
  ShiftPWM.SetAll(0);
  //End init Shiftregisters
}


void loop() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
    lastUpdate = millis();
  }

  setLED();
}

void setLED() {
  //Set Oil Pressure LED
  //int oilPressRange[] = { 0 , 15 , 15 , 50 , 50 , 70};
  int currOilPress = oilPress.value();

  if ((currOilPress < oilPressRange[1] && currOilPress > oilPressRange[0] ) ||  currOilPress > oilPressRange[4]) {
    ShiftPWM.SetRGB(7, currBrightness, 0, 0); //red
  } else if (currOilPress >= oilPressRange[2] &&  currOilPress <= oilPressRange[3]) {
    ShiftPWM.SetRGB(7, 0, currBrightness, 0); //green
  } else {
    ShiftPWM.SetRGB(7, currBrightness, 0, currBrightness); //purple
  }
  //End Oil Pressure


  //Set Fuel LH LED
  //int fuelLHRange[] = {0, 200, 400};
  int currfuelLH = fuelLH.value();
  if ((currfuelLH < fuelLHRange[1] && currfuelLH > 0 ) ) {
    ShiftPWM.SetRGB(6, currBrightness, 0, 0); //red
  } else if (currfuelLH >= fuelLHRange[1] &&  currfuelLH <= fuelLHRange[2]) {
    ShiftPWM.SetRGB(6, 0, currBrightness, 0); //green
  } else {
    ShiftPWM.SetRGB(6, currBrightness, 0, currBrightness); // purple
  }
  //End Fuel LH


  //Set Oil Temp LED
  //int oilTempRange[] = { 0 , 50 , 90 , 110 , 110 , 140};
  int currOilTemp = oilTemp.value();
  if ((currOilTemp < oilTempRange[1] && currOilTemp > 0 ) ||  (currOilTemp > oilTempRange[5] && currOilTemp > 0 )) {
    ShiftPWM.SetRGB(5, currBrightness, 0, 0); //red
  } else if (currOilTemp >= oilTempRange[2] &&  currOilTemp <= oilTempRange[3]) {
    ShiftPWM.SetRGB(5, 0, currBrightness, 0); //green
  } else {
    ShiftPWM.SetRGB(5, currBrightness, 0, currBrightness); // purple
  }
  //End Oil Temp

  int thisUpdate = millis();
  if (thisUpdate - lastUpdate <= updateTimeout) {
    ShiftPWM.SetRGB(4, 0, currBrightness, 0); //green
  } else {
    ShiftPWM.SetRGB(4, currBrightness, 0, 0);//red
  }



  //Set Volts LED
  //int voltsRange[] = {10,14};
  int currVolts = volts.value();
  if ((currVolts < voltsRange[0] && currVolts > 0 ) ||  (currVolts > voltsRange[1] && currVolts > 0 )) {
    ShiftPWM.SetRGB(3, currBrightness, 0, 0); //red
  } else if (currVolts >= voltsRange[0] &&  voltsRange <= voltsRange[1]) {
    ShiftPWM.SetRGB(3, 0, currBrightness, 0); //green
  } else {
    ShiftPWM.SetRGB(3, currBrightness, 0, currBrightness); // purple
  }
  //End Volts


  //Set CHT Temp LED
  //int CHTTempRange[] = {120, 150};
  int currCHTTemp = CHT1.value();
  if ((currCHTTemp < CHTTempRange[0] && currOilTemp > 0 ) ||  (currCHTTemp > CHTTempRange[1] && currCHTTemp > 0 )) {
    ShiftPWM.SetRGB(2, currBrightness, 0, 0); //red
  } else if (currCHTTemp >= CHTTempRange[0] &&  currCHTTemp <= CHTTempRange[1]) {
    ShiftPWM.SetRGB(2, 0, currBrightness, 0); //green
  } else {
    ShiftPWM.SetRGB(2, currBrightness, 0, currBrightness); // purple
  }
  //End CHT Temp


  //Set Fuel RH LED
  //int fuelRHRange[] = {0, 200, 400};
  int currfuelRH = fuelRH.value();
  if ((currfuelRH < fuelRHRange[1] && currfuelRH > 1 ) ) {
    ShiftPWM.SetRGB(1, currBrightness, 0, 0); //red
  } else if (currfuelRH >= fuelRHRange[1] &&  currfuelRH <= fuelRHRange[2]) {
    ShiftPWM.SetRGB(1, 0, currBrightness, 0); //green
  } else {
    ShiftPWM.SetRGB(1, currBrightness, 0, currBrightness); // purple
  }
  //End Fuel RH


  //Set EGT LED
  //int egtRange[] = { 880};
  int currEGT = EGT1.value();
  if ((currEGT <= egtRange[1]) ) {
    ShiftPWM.SetRGB(0, 0, currBrightness, 0); //green
  } else {
    ShiftPWM.SetRGB(0, currBrightness, 0, 0); //green
  }
  //End EGT
}
