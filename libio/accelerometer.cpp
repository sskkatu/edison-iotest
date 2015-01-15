#include <cstdint>
#include "mraa.hpp"
#include "accelerometer.hpp"
#include "accelerometer_regmap.hpp"

using namespace accelerometer_ADXL345_regmap;
using namespace mraa;

int Accelerometer::setGPIOIntrEdgeMode(int intno, Edge mode) {
    if (intno == 1 || intno == 2) {
        if (gpioInt != NULL) {
            return gpioInt->edge(mode);
        }
    }
    return MRAA_ERROR_NO_RESOURCES;
}

mraa_result_t Accelerometer::setGPIOIntrEdgeCallback(int intno, Edge mode, void (*fptr)(void*), void* arg) {
    if (intno == 1 || intno == 2) {
        if (gpioInt != NULL) {
            return gpioInt->isr(mode, fptr, arg);
        }
    }
    return MRAA_ERROR_NO_RESOURCES;
}

uint8_t Accelerometer::getDeviceId(void) {
    return readReg(ADXL345_DEVICE_ID);
}

void Accelerometer::setTap(uint8_t tap) {
    writeReg(ADXL345_THRESH_TAP, tap);
}
void Accelerometer::setTapmg(int tap) {
    int reg_tap = (tap / TAP_RESOLUTION_MG) / 10;
    if (reg_tap >= 0xFF) {
        reg_tap = 0xFF;
    }
    setTap((uint8_t)reg_tap);
}

uint8_t Accelerometer::getTap(void) {
    return readReg(ADXL345_THRESH_TAP);
}

int Accelerometer::getTapmg(void) {
    return (int)(getTap())*TAP_RESOLUTION_MG / 10;
}


void Accelerometer::setOffset(int8_t xofs, int8_t yofs, int8_t zofs) {
    writeReg(ADXL345_OFSX, xofs);
    writeReg(ADXL345_OFSY, yofs);
    writeReg(ADXL345_OFSZ, zofs);
}

void Accelerometer::getOffset(int8_t &xofs, int8_t &yofs, int8_t &zofs) {
    xofs = readReg(ADXL345_OFSX);
    yofs = readReg(ADXL345_OFSY);
    zofs = readReg(ADXL345_OFSZ);
}

void Accelerometer::setDur(uint8_t dur) {
    writeReg(ADXL345_DUR, dur);
}

uint8_t Accelerometer::getDur(void) {
    return readReg(ADXL345_DUR);
}

void Accelerometer::setLatent(uint8_t latent) {
    writeReg(ADXL345_LATENT, latent);
}

uint8_t Accelerometer::getLatent(void)  {
    return readReg(ADXL345_LATENT);
}

void Accelerometer::setWindow(uint8_t window) {
    writeReg(ADXL345_WINDOW, window);
}

uint8_t Accelerometer::getWindow(void) {
    return readReg(ADXL345_WINDOW);
}

void Accelerometer::setThreahAct(uint8_t threshAct) {
    writeReg(ADXL345_THREASH_ACT, threshAct);
}

uint8_t Accelerometer::getThreadhAct(void) {
    return readReg(ADXL345_THREASH_ACT);
}

void Accelerometer::setThreshInAct(uint8_t threshInAct) {
    writeReg(ADXL345_THRESH_INACT, threshInAct);
}

uint8_t Accelerometer::getThreshInAct(void) {
    return readReg(ADXL345_THRESH_INACT);
}

void Accelerometer::setTimeInAct(uint8_t timeInAct) {
    writeReg(ADXL345_TIME_INACT, timeInAct);
}

uint8_t Accelerometer::getTimeInAct(void) {
    return readReg(ADXL345_TIME_INACT);
}

void Accelerometer::setActInActCtl(Accelerometer::ActInActCtrl actinactctrl) {
    writeReg(ADXL345_ACT_INACT_CTL, actinactctrl.c);
}    

Accelerometer::ActInActCtrl Accelerometer::getActInActCtl(void) {
    ActInActCtrl result;
    result.c = readReg(ADXL345_ACT_INACT_CTL);
    return result;
}
void Accelerometer::setThreshFF(uint8_t threshFF) {
    writeReg(ADXL345_THRESH_FF, threshFF);
}

uint8_t Accelerometer::getThreshFF(void) {
    return readReg(ADXL345_THRESH_FF);
}

void Accelerometer::setTimeFF(uint8_t timeFF) {
    writeReg(ADXL345_TIME_FF, timeFF);
}

uint8_t Accelerometer::geTimeFF(void) {
    return readReg(ADXL345_TIME_FF);
}

void Accelerometer::setTapAxes(Accelerometer::TapAxes tapAxes) {
    writeReg(ADXL345_TAP_AXES, tapAxes.c);
}

Accelerometer::TapAxes Accelerometer::getTapAxes(void) {
    TapAxes result;
    result.c = readReg(ADXL345_TAP_AXES);
    return result;
}

Accelerometer::ActTapStatus Accelerometer::getActTapStatus(void) {
    ActTapStatus result;
    result.c = readReg(ADXL345_ACT_TAP_STATUS);
    return result;
}

void Accelerometer::setBWRate(Accelerometer::BWRate bwrate) {
    writeReg(ADXL345_BW_RATE, bwrate.c);
}

Accelerometer::BWRate Accelerometer::getBWRate(void) {
    BWRate result;
    result.c = readReg(ADXL345_BW_RATE);
    return result;
}

void Accelerometer::setPowerCtl(Accelerometer::PowerCtl powerctl) {
    writeReg(ADXL345_POWER_CTL, powerctl.c);
}

Accelerometer::PowerCtl Accelerometer::getPowerCtl(void) {
    PowerCtl result;
    result.c = readReg(ADXL345_POWER_CTL);
    return result;
}

void Accelerometer::setIntEnable(Accelerometer::InterruptMap intEnable) {
    writeReg(ADXL345_INT_ENABLE, intEnable.c);
}

Accelerometer::InterruptMap Accelerometer::getIntEnable(void) {
    InterruptMap result;
    result.c = readReg(ADXL345_INT_ENABLE);
    return result;
}

void Accelerometer::setIntMap(Accelerometer::InterruptMap intMap) {
    writeReg(ADXL345_INT_MAP, intMap.c);
}

Accelerometer::InterruptMap Accelerometer::getIntMap(void) {
    InterruptMap result;
    result.c = readReg(ADXL345_INT_MAP);
    return result;
}

Accelerometer::InterruptMap Accelerometer::getIntSource(void) {
    InterruptMap result;
    result.c = readReg(ADXL345_INT_SOURCE);
    return result;
}

void Accelerometer::setDataFormat(Accelerometer::DataFormat dataFormat) {
    writeReg(ADXL345_DATA_FORMAT, dataFormat.c);
}

Accelerometer::DataFormat Accelerometer::getDataFormat(void) {
    DataFormat result;
    result.c = readReg(ADXL345_DATA_FORMAT);
    return result;
}

int16_t Accelerometer::getXData(void) {
    uint8_t h, l;
    l = readReg(ADXL345_DATAX0);
    h = readReg(ADXL345_DATAX1);
    return (int16_t)((uint16_t)l | ((uint16_t)h << 8));
}

int16_t Accelerometer::getYData(void) {
    uint8_t h, l;
    l = readReg(ADXL345_DATAY0);
    h = readReg(ADXL345_DATAY1);
    return (int16_t)((uint16_t)l | ((uint16_t)h << 8));
}

int16_t Accelerometer::getZData(void) {
    uint8_t h, l;
    l = readReg(ADXL345_DATAZ0);
    h = readReg(ADXL345_DATAZ1);
    return (int16_t)((uint16_t)l | ((uint16_t)h << 8));
}

void Accelerometer::getXYZData(int16_t &x, int16_t &y, int16_t &z) {
#if (0)  // It's Buggy (cccasionally Kernel-crash) on Edison + Aitendo Multi Sensor. I2C bus problem?
    uint8_t cmd = ADXL345_DATAX0;
    uint8_t data[6];
    i2c->write(&cmd, 1);
    i2c->read(data, sizeof(data));
    x = (int16_t)((uint16_t)(data[0]) | ((uint16_t)(data[1]) << 8));
    y = (int16_t)((uint16_t)(data[2]) | ((uint16_t)(data[3]) << 8));
    z = (int16_t)((uint16_t)(data[4]) | ((uint16_t)(data[5]) << 8));
#else 
    x = getXData();
    y = getYData();
    z = getZData();
#endif
}

void Accelerometer::setFifoCtl(Accelerometer::FifoCtl fifoCtl) {
    writeReg(ADXL345_FIFO_CTL, fifoCtl.c);
}

Accelerometer::FifoCtl Accelerometer::getFifoCtl(void) {
    FifoCtl result;
    result.c = readReg(ADXL345_FIFO_CTL);
    return result;
}

Accelerometer::FifoStatus Accelerometer::getFifoStatus(void) {
    FifoStatus result;
    result.c = readReg(ADXL345_FIFO_STATUS);
    return result;
}
