/*!
 *  @file Adafruit_LTR390.cpp
 *
 *  @mainpage Adafruit LTR390 I2C UV and Light Sensor
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the LTR390 I2C UV and Light sensor
 *
 * 	This is a library for the Adafruit LTR390 breakout:
 * 	http://www.adafruit.com/
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried (Adafruit Industries)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_LTR390.h"

/*!
 *    @brief  Instantiates a new LTR390 class
 */
Adafruit_LTR390::Adafruit_LTR390(void) {}

/*!
 *    @brief  Setups the hardware for talking to the LTR390
 *    @param  theWire An optional pointer to an I2C interface
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_LTR390::begin(TwoWire *theWire) {
  i2c_dev = new Adafruit_I2CDevice(LTR390_I2CADDR_DEFAULT, theWire);

  if (!i2c_dev->begin()) {
    return false;
  }

  // check part ID!
  Adafruit_I2CRegister idreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_PART_ID);
  if ((idreg.read() >> 4) != 0xB) {
    return false;
  }

  // OK now we can do a soft reset
  if (! reset()) {
    return false;
  }

  // main screen turn on
  enable(true);
  if (!enabled()) {
    return false;
  }

  StatusReg = new Adafruit_I2CRegister(i2c_dev, LTR390_MAIN_STATUS);
  DataReadyBit = new Adafruit_I2CRegisterBits(StatusReg, 1, 3); 
    
  return true;
}


bool Adafruit_LTR390::reset(void) {
  Adafruit_I2CRegister mainreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_MAIN_CTRL);
  Adafruit_I2CRegisterBits softreset =
    Adafruit_I2CRegisterBits(&mainreg, 1, 4); // # bits, bit_shift
  
  // this write will fail because it resets before acking?
  softreset.write(1);
  delay(10);
  // however it does reset, check that the value is zero
  if (softreset.read()) {
    return false;
  }

  return true;
}

bool Adafruit_LTR390::newDataAvailable(void) {
  return DataReadyBit->read();
}

uint32_t Adafruit_LTR390::readALS(void) {
  Adafruit_I2CRegister datareg =
    Adafruit_I2CRegister(i2c_dev, LTR390_ALSDATA, 3, LSBFIRST);
  return datareg.read();
}

uint32_t Adafruit_LTR390::readUVS(void) {
  Adafruit_I2CRegister datareg =
    Adafruit_I2CRegister(i2c_dev, LTR390_UVSDATA, 3, LSBFIRST);
  return datareg.read();
}

  

void Adafruit_LTR390::enable(bool en) {
  Adafruit_I2CRegister mainreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_MAIN_CTRL);
  Adafruit_I2CRegisterBits enbit =
    Adafruit_I2CRegisterBits(&mainreg, 1, 1); // # bits, bit_shift

  enbit.write(en);
}

bool Adafruit_LTR390::enabled(void) {
  Adafruit_I2CRegister mainreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_MAIN_CTRL);
  Adafruit_I2CRegisterBits enbit =
    Adafruit_I2CRegisterBits(&mainreg, 1, 1); // # bits, bit_shift

  return enbit.read();
}

void Adafruit_LTR390::setMode(ltr390_mode_t mode) {
  Adafruit_I2CRegister mainreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_MAIN_CTRL);
  Adafruit_I2CRegisterBits modebit =
    Adafruit_I2CRegisterBits(&mainreg, 1, 3); // # bits, bit_shift

  modebit.write(mode);
}

ltr390_mode_t Adafruit_LTR390::getMode(void) {
  Adafruit_I2CRegister mainreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_MAIN_CTRL);
  Adafruit_I2CRegisterBits modebit =
    Adafruit_I2CRegisterBits(&mainreg, 1, 3); // # bits, bit_shift

  return (ltr390_mode_t)modebit.read();
}


void Adafruit_LTR390::setGain(ltr390_gain_t gain) {
  Adafruit_I2CRegister gainreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_GAIN);
  Adafruit_I2CRegisterBits gainbits =
    Adafruit_I2CRegisterBits(&gainreg, 3, 0); // # bits, bit_shift

  gainbits.write(gain);
}


ltr390_gain_t Adafruit_LTR390::getGain(void) {
  Adafruit_I2CRegister gainreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_GAIN);
  Adafruit_I2CRegisterBits gainbits =
    Adafruit_I2CRegisterBits(&gainreg, 3, 0); // # bits, bit_shift

  (ltr390_gain_t)gainbits.read();
}

void Adafruit_LTR390::setResolution(ltr390_resolution_t res) {
  Adafruit_I2CRegister ratereg =
    Adafruit_I2CRegister(i2c_dev, LTR390_MEAS_RATE);
  Adafruit_I2CRegisterBits resbits =
    Adafruit_I2CRegisterBits(&ratereg, 3, 4); // # bits, bit_shift

  resbits.write(res);
}


ltr390_resolution_t Adafruit_LTR390::getResolution(void) {
  Adafruit_I2CRegister ratereg =
    Adafruit_I2CRegister(i2c_dev, LTR390_MEAS_RATE);
  Adafruit_I2CRegisterBits resbits =
    Adafruit_I2CRegisterBits(&ratereg, 3, 4); // # bits, bit_shift

  return (ltr390_resolution_t)resbits.read();
}


void Adafruit_LTR390::setThresholds(uint32_t lower, uint32_t higher) {
  Adafruit_I2CRegister lowreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_THRESH_LOW, 3, LSBFIRST);
  lowreg.write(lower);

  Adafruit_I2CRegister upreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_THRESH_UP, 3, LSBFIRST);
  upreg.write(higher);
}

void Adafruit_LTR390::configInterrupt(bool enable, ltr390_mode_t source, uint8_t persistance) {
  Adafruit_I2CRegister intcfgreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_INT_CFG);
  Adafruit_I2CRegisterBits enbit =
    Adafruit_I2CRegisterBits(&intcfgreg, 1, 2); // # bits, bit_shift

  enbit.write(enable);
  
  Adafruit_I2CRegisterBits srcbits =
    Adafruit_I2CRegisterBits(&intcfgreg, 2, 4); // # bits, bit_shift
  if (source == LTR390_MODE_ALS) {
    srcbits.write(1);
  }
  if (source == LTR390_MODE_UVS) {
    srcbits.write(3);
  }

  Adafruit_I2CRegister persreg =
    Adafruit_I2CRegister(i2c_dev, LTR390_INT_PST);
  Adafruit_I2CRegisterBits pstbits =
    Adafruit_I2CRegisterBits(&persreg, 4, 4); // # bits, bit_shift
  pstbits.write(persistance);
}
