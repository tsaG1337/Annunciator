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
int egtRange[] = { 500, 800, 880};
int CHTTempRange[] = {120, 150};
int fuelPressureRange[] = {40, 150}; // ( 1/100 Bar)
int fuelLHRange[] = {0, 200, 400};
int fuelRHRange[] = {0, 200, 400};
int voltsRange[] = {100, 140};




//***Linking TinyGPS with the incoming data***//
TinyGPSCustom RPM(gps, "ICMB912", 1);
TinyGPSCustom manPress(gps, "ICMB912", 2);
TinyGPSCustom oilPress(gps, "ICMB912", 3);
TinyGPSCustom fuelPress(gps, "ICMB912", 4);
TinyGPSCustom oilTemp(gps, "ICMB912", 5);
TinyGPSCustom CHT1(gps, "ICMB912", 6);
TinyGPSCustom CHT2(gps, "ICMB912", 7);
TinyGPSCustom OAT(gps, "ICMB912", 8);
TinyGPSCustom fuelLH(gps, "ICMB912", 9);
TinyGPSCustom fuelRH(gps, "ICMB912", 10);
TinyGPSCustom fuelFlow(gps, "ICMB912", 11);
TinyGPSCustom fuelSum(gps, "ICMB912", 12);
TinyGPSCustom EGT1(gps, "ICMB912", 13);
TinyGPSCustom EGT2(gps, "ICMB912", 14);
TinyGPSCustom flaps(gps, "ICMB912", 15);
TinyGPSCustom volts(gps, "ICMB912", 16);
TinyGPSCustom amps(gps, "ICMB912", 17);

void setup() {
  Serial.begin(230400);

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
  for (int led = 0; led < numRGBleds; led++) {
      ShiftPWM.SetRGB(led, 0, 0, currBrightness); // blue
    }
  //End init Shiftregisters
}


void loop() {

  while (Serial.available() > 0) {
    char incomingByte = Serial.read();
    gps.encode(incomingByte);
  }
  bool updated = fuelLH.isUpdated();
  if (updated) {
    setLED();
  }

  if (fuelLH.age() >= 1500) {
    for (int led = 0; led < numRGBleds; led++) {
      ShiftPWM.SetRGB(led, 0, 0, currBrightness); // blue
    }
  }
}

void setLED() {
  //Set Oil Pressure LED
  //int oilPressRange[] = { 0 , 15 , 15 , 50 , 50 , 70};
  int currOilPress = atoi(oilPress.value());

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
  int currfuelLH = atoi(fuelLH.value());
  if (currfuelLH <= fuelLHRange[1] ) {
    ShiftPWM.SetRGB(6, currBrightness, 0, 0); //red
  } else  {
    ShiftPWM.SetRGB(6, 0, currBrightness, 0); //green
  }
  //End Fuel LH


  //Set Oil Temp LED
  //int oilTempRange[] = { 0 , 50 , 90 , 110 , 110 , 140};
  int currOilTemp = atoi(oilTemp.value());
  if ((currOilTemp < oilTempRange[1] ) ||  (currOilTemp > oilTempRange[5] )) {
    ShiftPWM.SetRGB(5, currBrightness, 0, 0); //red
  } else if (currOilTemp >= oilTempRange[2] &&  currOilTemp <= oilTempRange[3]) {
    ShiftPWM.SetRGB(5, 0, currBrightness, 0); //green
  }
  //End Oil Temp


  //Set Fuel Pressure LED
  //int fuelPressureRange[] = {40, 150};
  int currFuelPress = atoi(fuelPress.value());
  if ((currFuelPress < fuelPressureRange[0] ) ||  (currFuelPress > fuelPressureRange[1] )) {
    ShiftPWM.SetRGB(4, currBrightness, 0, 0); //red
  } else if (currFuelPress >= fuelPressureRange[0] &&  currFuelPress <= fuelPressureRange[1]) {
    ShiftPWM.SetRGB(4, 0, currBrightness, 0); //green
  }
  //End Fuel Pressure



  //Set Volts LED
  //int voltsRange[] = {100,140};
  int currVolts = atoi(volts.value());
  if ((currVolts < voltsRange[0] ) ||  (currVolts > voltsRange[1] )) {
    ShiftPWM.SetRGB(3, currBrightness, 0, 0); //red
  } else if (currVolts >= voltsRange[0] &&  currVolts <= voltsRange[1]) {
    ShiftPWM.SetRGB(3, 0, currBrightness, 0); //green
  }
  //End Volts


  //Set CHT Temp LED
  //int CHTTempRange[] = {60, 150};
  int currCHTTemp = atoi(CHT1.value());
  if ((currCHTTemp < CHTTempRange[0] ) ||  (currCHTTemp > CHTTempRange[1] )) {
    ShiftPWM.SetRGB(2, currBrightness, 0, 0); //red
  } else if (currCHTTemp >= CHTTempRange[0] &&  currCHTTemp <= CHTTempRange[1]) {
    ShiftPWM.SetRGB(2, 0, currBrightness, 0); //green
  }
  //End CHT Temp


  //Set Fuel RH LED
  //int fuelRHRange[] = {0, 200, 400};

  int currfuelRH = atoi(fuelRH.value());
  if (currfuelRH <= fuelRHRange[1] ) {
    ShiftPWM.SetRGB(1, currBrightness, 0, 0); //red
  } else  {
    ShiftPWM.SetRGB(1, 0, currBrightness, 0); //green
  }
  //End Fuel RH


  //Set EGT LED
  //int egtRange[] = { 500, 800, 880};
  int currEGT = atoi(EGT1.value());
  if ((currEGT <= egtRange[0]) ) {
    ShiftPWM.SetRGB(0, currBrightness, currBrightness, 0); //yellow
  } else  if ((currEGT >= egtRange[2]) ) {
    ShiftPWM.SetRGB(0, currBrightness, 0, 0); //red
  } else {
    ShiftPWM.SetRGB(0, 0, currBrightness, 0); //green
  }
  //End EGT
}
