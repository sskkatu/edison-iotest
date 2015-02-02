#include <cstdint>
#include <stdio.h>
#include "mraa.hpp"
#include "accelerometer.hpp"
#include "accelerometer_regmap.hpp"

using namespace accelerometer_ADXL345_regmap;
using namespace mraa;

mraa_result_t Accelerometer::setGPIOIntrEdgeCallback(int intno, Edge mode, void (*fptr)(void*), void* arg) {
    if (intno == 1 || intno == 2) {
        if (gpioInt[intno-1] != NULL) {
            return gpioInt[intno-1]->isr(mode, fptr, arg);
        }
    }
    return MRAA_ERROR_NO_RESOURCES;
}

mraa_result_t Accelerometer::interruptExit(int intno) {
    if (intno == 1 || intno == 2) {
        if (gpioInt[intno-1] != NULL) {
            return gpioInt[intno-1]->isrExit();
        }
    }
    return MRAA_ERROR_NO_RESOURCES;
}

uint8_t Accelerometer::getDevId(void) {
    return readReg(ADXL345_DEVICE_ID);
}

void Accelerometer::setTap(uint8_t tap) {
    writeReg(ADXL345_THRESH_TAP, tap);
}

void Accelerometer::setTap_mg(int tap) {
    setTap(convUtoR(tap, TAP_RESOLUTION_MG, 10));
}

uint8_t Accelerometer::getTap(void) {
    return readReg(ADXL345_THRESH_TAP);
}

int Accelerometer::getTap_mg(void) {
    return convRtoU(getTap(), TAP_RESOLUTION_MG, 10);
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

void Accelerometer::setOffset_mg(int xofsmg, int yofsmg, int zofsmg) {
    uint8_t xofs = convUtoR(xofsmg, OFS_RESOLUTION_MG, 10);
    uint8_t yofs = convUtoR(yofsmg, OFS_RESOLUTION_MG, 10);
    uint8_t zofs = convUtoR(zofsmg, OFS_RESOLUTION_MG, 10);
    setOffset(xofs, yofs, zofs);
}

void Accelerometer::getOffset_mg(int &xofsmg, int &yofsmg, int &zofsmg) {
    int8_t xofs, yofs, zofs;

    getOffset(xofs, yofs, zofs);
    xofsmg = convRtoU(xofs, OFS_RESOLUTION_MG, 10);
    yofsmg = convRtoU(yofs, OFS_RESOLUTION_MG, 10);
    xofsmg = convRtoU(zofs, OFS_RESOLUTION_MG, 10);
}

void Accelerometer::setDur(uint8_t dur) {
    writeReg(ADXL345_DUR, dur);
}

uint8_t Accelerometer::getDur(void) {
    return readReg(ADXL345_DUR);
}

void Accelerometer::setDur_us(useconds_t durus) {
    setDur(convUtoR((int)durus, DUR_RESOLUTION_US));
}

useconds_t Accelerometer::getDur_us(void) {
    return (useconds_t)(convRtoU(getDur(), DUR_RESOLUTION_US));
}

void Accelerometer::setLatent(uint8_t latent) {
    writeReg(ADXL345_LATENT, latent);
}

uint8_t Accelerometer::getLatent(void)  {
    return readReg(ADXL345_LATENT);
}

void Accelerometer::setLatent_us(useconds_t latent_us) {
    setLatent(convUtoR((int)latent_us, LATENT_RESOLUTION_US));
}
useconds_t Accelerometer::getLatent_us(void) {
    return (useconds_t)(convRtoU(getLatent(), LATENT_RESOLUTION_US));
}

void Accelerometer::setWindow(uint8_t window) {
    writeReg(ADXL345_WINDOW, window);
}

uint8_t Accelerometer::getWindow(void) {
    return readReg(ADXL345_WINDOW);
}

void Accelerometer::setWindow_us(useconds_t window_us) {
    setWindow(convUtoR((int)window_us, WINDOW_RESOLUTION_US));
}
useconds_t Accelerometer::getWindow_us(void) {
    return (useconds_t)(convRtoU(getWindow(), WINDOW_RESOLUTION_US));
}

void Accelerometer::setThreshAct(uint8_t threshAct) {
    writeReg(ADXL345_THREASH_ACT, threshAct);
}

uint8_t Accelerometer::getThreshAct(void) {
    return readReg(ADXL345_THREASH_ACT);
}

void Accelerometer::setThreshAct_mg(int threshAct_mg) {
    setThreshAct(convUtoR(threshAct_mg, ACT_RESOLUTION_MG, 10));
}

int Accelerometer::getThreshAct_mg(void) {
    return (int)(convRtoU(getThreshAct(), ACT_RESOLUTION_MG, 10));
}

void Accelerometer::setThreshInAct(uint8_t threshInAct) {
    writeReg(ADXL345_THRESH_INACT, threshInAct);
}

uint8_t Accelerometer::getThreshInAct(void) {
    return readReg(ADXL345_THRESH_INACT);
}

void Accelerometer::setThreshInAct_mg(int threashInAct_mg) {
    setThreshInAct(convUtoR(threashInAct_mg, INACT_RESOLUTION_MG, 10));
}

int Accelerometer::getThreshInAct_mg(void) {
    return (int)(convRtoU(getThreshInAct(), INACT_RESOLUTION_MG, 10));
}

void Accelerometer::setTimeInAct(uint8_t timeInAct) {
    writeReg(ADXL345_TIME_INACT, timeInAct);
}

uint8_t Accelerometer::getTimeInAct(void) {
    return readReg(ADXL345_TIME_INACT);
}

void Accelerometer::setActInActCtl(uint8_t actinactctl) {
    writeReg(ADXL345_ACT_INACT_CTL, actinactctl);
}    

void Accelerometer::setActInActCtl(Accelerometer::ActInActCtl c) {
    uint8_t cb = 0;

    cb |= (c.act_ac_dc)      ? SET_ACT_INACT_CTL_ACCT_AC_DC     : 0;
    cb |= (c.act_x_enable)   ? SET_ACT_INACT_CTL_ACT_X_ENABLE   : 0;
    cb |= (c.act_y_enable)   ? SET_ACT_INACT_CTL_ACT_Y_ENABLE   : 0;
    cb |= (c.act_z_enable)   ? SET_ACT_INACT_CTL_ACT_Z_ENABLE   : 0;
    cb |= (c.inact_ac_dc)    ? SET_ACT_INACT_CTL_INACT_ACDC     : 0;
    cb |= (c.inact_x_enable) ? SET_ACT_INACT_CTL_INACT_X_ENABLE : 0;
    cb |= (c.inact_y_enable) ? SET_ACT_INACT_CTL_INACT_Y_ENABLE : 0;
    cb |= (c.inact_z_enable) ? SET_ACT_INACT_CTL_INACT_Z_ENABLE : 0;
    setActInActCtl(cb);
}    

uint8_t Accelerometer::getActInActCtl(void) {
    return readReg(ADXL345_ACT_INACT_CTL);
}

void Accelerometer::getActInActCtl(Accelerometer::ActInActCtl &c) {
    uint8_t cb = getActInActCtl();

    c.act_ac_dc      = (cb & SET_ACT_INACT_CTL_ACCT_AC_DC)     ? true : false;
    c.act_x_enable   = (cb & SET_ACT_INACT_CTL_ACT_X_ENABLE)   ? true : false;
    c.act_y_enable   = (cb & SET_ACT_INACT_CTL_ACT_Y_ENABLE)   ? true : false;
    c.act_z_enable   = (cb & SET_ACT_INACT_CTL_ACT_Z_ENABLE)   ? true : false;
    c.inact_ac_dc    = (cb & SET_ACT_INACT_CTL_INACT_ACDC)     ? true : false;
    c.inact_x_enable = (cb & SET_ACT_INACT_CTL_INACT_X_ENABLE) ? true : false;
    c.inact_y_enable = (cb & SET_ACT_INACT_CTL_INACT_Y_ENABLE) ? true : false;
    c.inact_z_enable = (cb & SET_ACT_INACT_CTL_INACT_Z_ENABLE) ? true : false;
}

void Accelerometer::setThreshFF(uint8_t threshFF) {
    writeReg(ADXL345_THRESH_FF, threshFF);
}

uint8_t Accelerometer::getThreshFF(void) {
    return readReg(ADXL345_THRESH_FF);
}

void Accelerometer::setThreshFF_mg(int threshFF_mg) {
    setThreshFF(convUtoR(threshFF_mg, THRESH_FF_MG, 10));
}

int Accelerometer::getThreshFF_mg(void) {
    return (convRtoU(getThreshFF(), THRESH_FF_MG, 10));
}

void Accelerometer::setTimeFF(uint8_t timeFF) {
    writeReg(ADXL345_TIME_FF, timeFF);
}

uint8_t Accelerometer::getTimeFF(void) {
    return readReg(ADXL345_TIME_FF);
}

void Accelerometer::setTimeFF_ms(int timeFF_ms) {
    setTimeFF(convUtoR(timeFF_ms, TIME_FF_MS));
}

int Accelerometer::getTimeFF_ms(void) {
    return (convRtoU(getTimeFF(), TIME_FF_MS));
}

void Accelerometer::setTapAxes(uint8_t c) {
    writeReg(ADXL345_TAP_AXES, c);
}

void Accelerometer::setTapAxes(Accelerometer::TapAxes c) {
    uint8_t cb = 0;

    cb |= c.supress      ? TAPAXES_SUPRESSS : 0;
    cb |= c.tap_x_enable ? TAPAXES_TAP_X_ENABLE : 0;
    cb |= c.tap_y_enable ? TAPAXES_TAP_Y_ENABLE : 0;
    cb |= c.tap_z_enable ? TAPAXES_TAP_Z_ENABLE : 0;
    setTapAxes(cb);
}

uint8_t Accelerometer::getTapAxes(void) {
    return readReg(ADXL345_TAP_AXES);
}

void Accelerometer::getTapAxes(TapAxes &c) {
    uint8_t cb = getTapAxes();

    c.supress = (cb & TAPAXES_SUPRESSS) ? true : false;
    c.tap_x_enable = (cb & TAPAXES_TAP_X_ENABLE) ? true : false;
    c.tap_y_enable = (cb & TAPAXES_TAP_Y_ENABLE) ? true : false;
    c.tap_z_enable = (cb & TAPAXES_TAP_Z_ENABLE) ? true : false;
}

uint8_t Accelerometer::getActTapStatus(void) {
    return readReg(ADXL345_ACT_TAP_STATUS);
}

void Accelerometer::getActTapStatus(Accelerometer::ActTapStatus &c) {
    uint8_t cb = getActTapStatus();

    c.act_x_source = (cb & ACT_TAP_STATUS_ACT_X_SOURCE) ? true : false;
    c.act_y_source = (cb & ACT_TAP_STATUS_ACT_Y_SOURCE) ? true : false;
    c.act_z_source = (cb & ACT_TAP_STATUS_ACT_Z_SOURCE) ? true : false;
    c.asleep       = (cb & ACT_TAP_STATUS_ASLEEP)       ? true : false;
    c.tap_x_source = (cb & ACT_TAP_STATUS_TAP_X_SOURCE) ? true : false;
    c.tap_y_source = (cb & ACT_TAP_STATUS_TAP_Y_SOURCE) ? true : false;
    c.tap_z_source = (cb & ACT_TAP_STATUS_TAP_Z_SOURCE) ? true : false;
}

void Accelerometer::setBWRate(uint8_t bwrate) {
    writeReg(ADXL345_BW_RATE, bwrate);
}

uint8_t Accelerometer::getBWRate(void) {
    return readReg(ADXL345_BW_RATE);
}

void Accelerometer::setBWRate(Accelerometer::BWRate c) {
    uint8_t cb = 0;

    cb |= c.bwrate & BWRATE_RATE_MASK;
    cb |= c.low_power ? BWRATE_LOWPOWER : 0;
    setBWRate(cb);
}

void Accelerometer::getBWRate(Accelerometer::BWRate &c) {
    uint8_t cb = getBWRate();

    c.bwrate = cb & BWRATE_RATE_MASK;
    c.low_power = (cb & BWRATE_LOWPOWER) ? true : false;
}


void Accelerometer::setPowerCtl(uint8_t powerctl) {
    writeReg(ADXL345_POWER_CTL, powerctl);
}

uint8_t Accelerometer::getPowerCtl(void) {
    return readReg(ADXL345_POWER_CTL);
}

void Accelerometer::setPowerCtl(Accelerometer::PowerCtl c) {
    uint8_t cb = 0;

    cb |= (c.link)       ? POWERCTL_LINK      : 0;
    cb |= (c.auto_sleep) ? POWERCTL_AUTOSLEEP : 0;
    cb |= (c.measure)    ? POWERCTL_MEASURE   : 0;
    cb |= (c.sleep)      ? POWERCTL_SLEEP     : 0;
    cb |= (int)(c.wakeup_mode) & POWERCTL_WAKEUP_MASK;
    setPowerCtl(cb);
}

void Accelerometer::getPowerCtl(Accelerometer::PowerCtl &c) {
    uint8_t cb = getPowerCtl();

    c.link       = (cb & POWERCTL_LINK)      ? true : false;
    c.auto_sleep = (cb & POWERCTL_AUTOSLEEP) ? true : false;
    c.measure    = (cb & POWERCTL_MEASURE)   ? true : false;
    c.sleep      = (cb & POWERCTL_SLEEP)     ? true : false;
    c.wakeup_mode = (WakeUpMode)(cb & POWERCTL_WAKEUP_MASK);
}

void Accelerometer::startMeasure(void) {
    PowerCtl c;
    getPowerCtl(c);
    c.measure = true;
    setPowerCtl(c);
}

void Accelerometer::stopMeasure(void) {
    PowerCtl c;
    getPowerCtl(c);
    c.measure = false;
    setPowerCtl(c);
}

void Accelerometer::setIntEnable(uint8_t c) {
    writeReg(ADXL345_INT_ENABLE, c);
}

uint8_t Accelerometer::getIntEnable(void) {
    return readReg(ADXL345_INT_ENABLE);
}

void Accelerometer::setIntEnable(Accelerometer::InterruptMap c) {
    uint8_t cb = 0;

    cb |= c.data_ready ? INERRUPTMAP_DATAREADY  : 0;
    cb |= c.single_tap ? INERRUPTMAP_SINGLETAP  : 0;
    cb |= c.double_tap ? INERRUPTMAP_DOUBLETAP  : 0;
    cb |= c.activity   ? INERRUPTMAP_ACTIVITY   : 0;
    cb |= c.inactivity ? INERRUPTMAP_INACTIVITY : 0;
    cb |= c.free_fall  ? INERRUPTMAP_FREEFALL   : 0;
    cb |= c.watermark  ? INERRUPTMAP_WATERMARK  : 0;
    cb |= c.overrun    ? INERRUPTMAP_OVERRUN    : 0;
    setIntEnable(cb);
}

void Accelerometer::getIntEnable(Accelerometer::InterruptMap &c) {
    uint8_t cb = getIntEnable();
    
    c.data_ready = (cb & INERRUPTMAP_DATAREADY)  ? true : false;
    c.single_tap = (cb & INERRUPTMAP_SINGLETAP)  ? true : false;
    c.double_tap = (cb & INERRUPTMAP_DOUBLETAP)  ? true : false;
    c.activity   = (cb & INERRUPTMAP_ACTIVITY)   ? true : false;
    c.inactivity = (cb & INERRUPTMAP_INACTIVITY) ? true : false;
    c.free_fall  = (cb & INERRUPTMAP_FREEFALL)   ? true : false;
    c.watermark  = (cb & INERRUPTMAP_WATERMARK)  ? true : false;
    c.overrun    = (cb & INERRUPTMAP_OVERRUN)    ? true : false;
}

void Accelerometer::setIntMap(uint8_t c) {
    writeReg(ADXL345_INT_MAP, c);
}

uint8_t Accelerometer::getIntMap(void) {
    return readReg(ADXL345_INT_MAP);
}

void Accelerometer::setIntMap(Accelerometer::InterruptMap c) {
    uint8_t cb = 0;

    cb |= c.data_ready ? INERRUPTMAP_DATAREADY  : 0;
    cb |= c.single_tap ? INERRUPTMAP_SINGLETAP  : 0;
    cb |= c.double_tap ? INERRUPTMAP_DOUBLETAP  : 0;
    cb |= c.activity   ? INERRUPTMAP_ACTIVITY   : 0;
    cb |= c.inactivity ? INERRUPTMAP_INACTIVITY : 0;
    cb |= c.free_fall  ? INERRUPTMAP_FREEFALL   : 0;
    cb |= c.watermark  ? INERRUPTMAP_WATERMARK  : 0;
    cb |= c.overrun    ? INERRUPTMAP_OVERRUN    : 0;
    setIntMap(cb);
}

void Accelerometer::getIntMap(Accelerometer::InterruptMap &c) {
    uint8_t cb = getIntEnable();

    c.data_ready = (cb & INERRUPTMAP_DATAREADY)  ? true : false;
    c.single_tap = (cb & INERRUPTMAP_SINGLETAP)  ? true : false;
    c.double_tap = (cb & INERRUPTMAP_DOUBLETAP)  ? true : false;
    c.activity   = (cb & INERRUPTMAP_ACTIVITY)   ? true : false;
    c.inactivity = (cb & INERRUPTMAP_INACTIVITY) ? true : false;
    c.free_fall  = (cb & INERRUPTMAP_FREEFALL)   ? true : false;
    c.watermark  = (cb & INERRUPTMAP_WATERMARK)  ? true : false;
    c.overrun    = (cb & INERRUPTMAP_OVERRUN)    ? true : false;
}

uint8_t Accelerometer::getIntSource(void) {
    return readReg(ADXL345_INT_SOURCE);
}

void Accelerometer::getIntSource(Accelerometer::InterruptMap &c) {
    uint8_t cb = getIntSource();

    c.data_ready = (cb & INERRUPTMAP_DATAREADY)  ? true : false;
    c.single_tap = (cb & INERRUPTMAP_SINGLETAP)  ? true : false;
    c.double_tap = (cb & INERRUPTMAP_DOUBLETAP)  ? true : false;
    c.activity   = (cb & INERRUPTMAP_ACTIVITY)   ? true : false;
    c.inactivity = (cb & INERRUPTMAP_INACTIVITY) ? true : false;
    c.free_fall  = (cb & INERRUPTMAP_FREEFALL)   ? true : false;
    c.watermark  = (cb & INERRUPTMAP_WATERMARK)  ? true : false;
    c.overrun    = (cb & INERRUPTMAP_OVERRUN)    ? true : false;
}

void Accelerometer::setDataFormat(uint8_t c) {
    writeReg(ADXL345_DATA_FORMAT, c);
}

uint8_t Accelerometer::getDataFormat(void) {
    return readReg(ADXL345_DATA_FORMAT);
}

void Accelerometer::setDataFormat(Accelerometer::DataFormat c) {
    uint8_t cb = 0;

    cb |= c.self_test  ? DATAFORMAT_SELF_TEST  : 0;
    cb |= c.spi        ? DATAFORMAT_SPI        : 0;
    cb |= c.int_invert ? DATAFORMAT_INT_INVERT : 0;
    cb |= c.fullres    ? DATAFORMAT_FULLRES    : 0;
    cb |= c.justify    ? DATAFORMAT_JUSTIFY    : 0;
    cb |= (int)(c.range) & DATAFORMAT_RANGE_MASK;

    setDataFormat(cb);
}

void Accelerometer::getDataFormat(Accelerometer::DataFormat &c) {
    uint8_t cb = getDataFormat();

    c.self_test  = (cb & DATAFORMAT_SELF_TEST)  ? true : false;
    c.spi        = (cb & DATAFORMAT_SPI)        ? true : false;
    c.int_invert = (cb & DATAFORMAT_INT_INVERT) ? true : false;
    c.fullres    = (cb & DATAFORMAT_FULLRES)    ? true : false;
    c.justify    = (cb & DATAFORMAT_JUSTIFY)    ? true : false;
    c.range      = (GRange)(cb & DATAFORMAT_RANGE_MASK);
}


int16_t Accelerometer::getXData(void) {
    return (int16_t)readWordReg(ADXL345_DATAX0);
}

int16_t Accelerometer::getYData(void) {
    return (int16_t)readWordReg(ADXL345_DATAY0);
}

int16_t Accelerometer::getZData(void) {
    return (int16_t)readWordReg(ADXL345_DATAZ0);
}

void Accelerometer::getXYZData(int16_t &x, int16_t &y, int16_t &z) {
    uint8_t cmd = ADXL345_DATAX0;
    uint8_t data[6];

    i2c->write(&cmd, 1);
    i2c->read(data, sizeof(data));
    x = (int16_t)((uint16_t)(data[0]) | ((uint16_t)(data[1]) << 8));
    y = (int16_t)((uint16_t)(data[2]) | ((uint16_t)(data[3]) << 8));
    z = (int16_t)((uint16_t)(data[4]) | ((uint16_t)(data[5]) << 8));
}

void Accelerometer::setFifoCtl(uint8_t c) {
    writeReg(ADXL345_FIFO_CTL, c);
}

uint8_t Accelerometer::getFifoCtl(void) {
    return readReg(ADXL345_FIFO_CTL);
}

void Accelerometer::setFifoCtl(Accelerometer::FifoCtl c) {
    uint8_t cb = 0;

    cb |= ((int)(c.fifo_mode) << 6) & FIFO_MODE_MASK;
    cb |= c.trigger ? FIFOCTL_TRIGGER : 0;
    cb |= c.samples & FIFOCTL_SAMPLES_MASK;
    setFifoCtl(cb);
}

void Accelerometer::getFifoCtl(Accelerometer::FifoCtl &c) {
    uint8_t cb = getFifoCtl();
    
    c.fifo_mode = (FifoMode) ((cb & FIFO_MODE_MASK) >> 6);
    c.trigger = (cb & FIFOCTL_TRIGGER) ? true : false;
    c.samples = (int)(cb & FIFOCTL_SAMPLES_MASK);
}

uint8_t Accelerometer::getFifoStatus(void) {
    return readReg(ADXL345_FIFO_STATUS);
}

void Accelerometer::getFifoStatus(Accelerometer::FifoStatus &c) {
    uint8_t cb = getFifoStatus();

    c.fifo_trig = (cb & FIFO_STATUS_FIFO_TRIG) ? true : false;
    c.fifo_entries = (int)(cb & FIFO_STATUS_FIFO_ENTRIES_MASK);
}

void Accelerometer::clearRemainingFifoData(void) {
    int16_t x, y, z;
    FifoStatus fifostatus;
    stopMeasure();
    getFifoStatus(fifostatus);
    for (int i=0; i < fifostatus.fifo_entries; i++) {
        getXYZData(x, y, z);
    }
}

void Accelerometer::dumpAllReg(void) {
    int8_t xofs, yofs, zofs;
    int16_t datax, datay, dataz;
    getOffset(xofs, yofs, zofs);
    getXYZData(datax, datay, dataz);

    printf("-------------------------\n");
    printf("(00) Device ID      : %02x\n", getDevId());
    printf("(1D) THRESH TAP     : %02x\n", getTap());
    printf("(1E) OFSX           : %02x\n", xofs);
    printf("(1F) OFSY           : %02x\n", yofs);
    printf("(20) OFSZ           : %02x\n", zofs);
    printf("(21) DUR            : %02x\n", getDur());
    printf("(22) Latent         : %02x\n", getLatent());
    printf("(23) Window         : %02x\n", getWindow());
    printf("(24) THRESH_ACT     : %02x\n", getThreshAct());
    printf("(25) THRESH_INACT   : %02x\n", getThreshInAct());
    printf("(26) TIME_INACT     : %02x\n", getTimeInAct());
    printf("(27) ACT_INACT_CTL  : %02x\n", getActInActCtl());
    printf("(28) THRESH_FF      : %02x\n", getThreshFF());
    printf("(29) TIME_FF        : %02x\n", getTimeFF());
    printf("(2A) TAP_AXES       : %02x\n", getTapAxes());
    printf("(2B) ACT_TAP_STATUS : %02x\n", getActTapStatus());
    printf("(2C) BW_RATE        : %02x\n", getBWRate());
    printf("(2D) POWER_CTL      : %02x\n", getPowerCtl());
    printf("(2E) INT_ENABLE     : %02x\n", getIntEnable());
    printf("(2F) INT_MAP        : %02x\n", getIntMap());
    printf("(30) INT_SOURCE     : %02x\n", getIntSource());
    printf("(31) DATA_FORMAT    : %02x\n", getDataFormat());
    printf("(32-33) DATAX       : %04x\n", datax);
    printf("(34-35) DATAY       : %04x\n", datay);
    printf("(36-37) DATAZ       : %04x\n", dataz);
    printf("(38) FIFO_CTL       : %02x\n", getFifoCtl());
    printf("(39) FIFO_STATUS    : %02x\n", getFifoStatus());
    printf("-------------------------\n");
}
