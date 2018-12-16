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
int currBrightness = 128;

//*** Definition of operational ranges ***//

int rpmRange[] = {0, 1440, 1800, 5500, 5800, 5850};
// Red_end , yellow_begin, green_begin, green_end, yellow_end, red_begin) (1/10 Bar)
int oilPressRange[] = { 0 , 8 , 20 , 50 , 70};
int oilTempRange[] = { 1 , 50 , 90 , 110 , 140};
int egtRange[] = { 500, 800, 880};
int CHTTempRange[] = {120, 150};
int fuelPressureRange[] = {15, 40}; // ( 1/100 Bar)
int fuelLHRange[] = {0, 200, 400};
int fuelRHRange[] = {0, 200, 400};
int voltsRange[] = {100, 140};

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
  Serial.begin(230400);

  //Init Shiftregisters
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.SetPinGrouping(1);
  ShiftPWM.Start(pwmFrequency, maxBrightness);

  //Set all LEDs to blue
  for (int led = 0; led < numRGBleds; led++) {
    ShiftPWM.SetRGB(led, 0, 0, currBrightness); // blue
  }
  //End init Shiftregisters

}


void loop() { //SetLEDs Everytime a value is Updated.
  if (RPM.isUpdated() || manPress.isUpdated() || oilPress.isUpdated() || fuelPress.isUpdated() || oilPress.isUpdated() || CHT1.isUpdated() || 
  CHT2.isUpdated() || OAT.isUpdated() || fuelLH.isUpdated() || fuelRH.isUpdated() || fuelFlow.isUpdated() || fuelSum.isUpdated() || 
  EGT1.isUpdated() || EGT2.isUpdated() || flaps.isUpdated() || volts.isUpdated() || amps.isUpdated() ) {
    setLED();
  }
}

void serialEvent() {
  while (Serial.available() > 0) {
    motorStream.encode(Serial.read());
  }
}

void setLED() {
  //Set Oil Pressure LED
  //int oilPressRange[] = { 0 , 8 , 20 , 50 , 70};
  int currOilPress = oilPress.value();
  if (currOilPress <= oilPressRange[1] ||  currOilPress >= oilPressRange[4]) {
    ShiftPWM.SetRGB(7, currBrightness, 0, 0); //red
  } else if (currOilPress >= oilPressRange[1] &&  currOilPress <= oilPressRange[2]) {
    ShiftPWM.SetRGB(7, currBrightness, currBrightness, 0 ); //yellow
  } else if (currOilPress >= oilPressRange[2] &&  currOilPress <= oilPressRange[3]) {
    ShiftPWM.SetRGB(7, 0, currBrightness, 0); //green
  }
  //End Oil Pressure


  //Set Fuel LH LED
  //int fuelLHRange[] = {0, 200, 400};
  int currfuelLH = fuelLH.value();
  if (currfuelLH <= fuelLHRange[1] ) {
    ShiftPWM.SetRGB(6, currBrightness, 0, 0); //red
  } else  {
    ShiftPWM.SetRGB(6, 0, currBrightness, 0); //green
  }
  //End Fuel LH


  //Set Oil Temp LED
  //int oilTempRange[] = { 1 , 50 , 90 , 110 , 140};
  int currOilTemp = oilTemp.value();
  if (currOilTemp <= oilTempRange[1] ||  currOilTemp >= oilTempRange[4]) {
    ShiftPWM.SetRGB(5, currBrightness, 0, 0); //red
  } else if (currOilTemp >= oilTempRange[1] &&  currOilTemp <= oilTempRange[2] || currOilTemp >= oilTempRange[3] && currOilTemp <= oilTempRange[4] ) {
    ShiftPWM.SetRGB(5, currBrightness, currBrightness, 0 ); //yellow
  } else if (currOilTemp >= oilTempRange[2] &&  currOilTemp <= oilTempRange[3]) {
    ShiftPWM.SetRGB(5, 0, currBrightness, 0); //green
  }
  //End Oil Temp


  //Set Fuel Pressure LED
  //int fuelPressureRange[] = {15, 40};
  int currFuelPress = fuelPress.value();
  if ((currFuelPress < fuelPressureRange[0] ) ||  (currFuelPress > fuelPressureRange[1] )) {
    ShiftPWM.SetRGB(4, currBrightness, 0, 0); //red
  } else if (currFuelPress >= fuelPressureRange[0] &&  currFuelPress <= fuelPressureRange[1]) {
    ShiftPWM.SetRGB(4, 0, currBrightness, 0); //green
  }
  //End Fuel Pressure


  //Set Volts LED
  //int voltsRange[] = {100,140};
  //int currVolts = valueArray[volts];
  int currVolts = volts.value();
  Serial.println(currVolts);
  if ((currVolts < voltsRange[0] ) ||  (currVolts > voltsRange[1] )) {
    ShiftPWM.SetRGB(3, currBrightness, 0, 0); //red
  } else if (currVolts >= voltsRange[0]  || currVolts <= voltsRange[1]) {
    ShiftPWM.SetRGB(3, 0, currBrightness, 0); //green
  }
  //End Volts


  //Set CHT Temp LED
  //int CHTTempRange[] = {60, 150};
  int currCHTTemp = CHT1.value();
  if ((currCHTTemp < CHTTempRange[0] ) ||  (currCHTTemp > CHTTempRange[1] )) {
    ShiftPWM.SetRGB(2, currBrightness, 0, 0); //red
  } else if (currCHTTemp >= CHTTempRange[0] &&  currCHTTemp <= CHTTempRange[1]) {
    ShiftPWM.SetRGB(2, 0, currBrightness, 0); //green
  }
  //End CHT Temp


  //Set Fuel RH LED
  //int fuelRHRange[] = {0, 200, 400};
  int currfuelRH = fuelRH.value();
  if (currfuelRH <= fuelRHRange[1] ) {
    ShiftPWM.SetRGB(1, currBrightness, 0, 0); //red
  } else  {
    ShiftPWM.SetRGB(1, 0, currBrightness, 0); //green
  }
  //End Fuel RH


  //Set EGT LED
  //int egtRange[] = { 500, 800, 880};
  int currEGT = EGT1.value();
  if ((currEGT <= egtRange[0]) ) {
    ShiftPWM.SetRGB(0, currBrightness, currBrightness, 0); //yellow
  } else  if ((currEGT >= egtRange[2]) ) {
    ShiftPWM.SetRGB(0, currBrightness, 0, 0); //red
  } else {
    ShiftPWM.SetRGB(0, 0, currBrightness, 0); //green
  }
  //End EGT
}
