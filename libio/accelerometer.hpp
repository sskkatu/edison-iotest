#pragma once
#include <cstdint>
#include "mraa.hpp"

using namespace mraa;

/**
 * Products :
 *  Aitendo - Multi function Sensor [ATD-M4S]  (includes Accelometor/Gyroscope/Compass/Barometer)
 *   http://www.aitendo.com/product/9551
 *
 * DataSheets :
 * 3-Axis Accelerometer/Analog Devices ADXL345
 *    http://aitendo3.sakura.ne.jp/aitendo_data/product_img/sensor/multi/GY80/ADXL345.pdf
 *
 */

class Accelerometer {
public:
    typedef enum {
        RATE_0000_10 = 0,  //    0.10Hz
        RATE_0000_20,      //    0.20Hz
        RATE_0000_39,      //    0.39Hz
        RATE_0000_78,      //    0.78Hz
        RATE_0001_56,      //    1.56Hz
        RATE_0003_13,      //    3.13Hz
        RATE_0006_25,      //    6.25Hz
        RATE_0012_50,      //   12.50Hz
        RATE_0025_00,      //   25.00Hz
        RATE_0050_00,      //   50.00Hz
        RATE_0100_00,      //  100.00Hz
        RATE_0200_00,      //  200.00Hz
        RATE_0400_00,      //  400.00Hz
        RATE_0800_00,      //  800.00Hz
        RATE_1600_00,      // 1600.00Hz
        RATE_3200_00,      // 3200.00Hz
    } OutputDataRate;
private:
    static const int     DEFAULT_I2C_BUS_NO    =    6;
    static const uint8_t DEFAULT_I2C_ADDRESS   = 0x53;
    static const int     DEFAULT_INT1_PIN      =   -1;
    static const int     TAP_RESOLUTION_MG     =  625;
    static const int     OFS_RESOLUTION_MG     =  156;

    I2c* i2c = NULL;
    Gpio *gpioInt = NULL;
    
    void initializeDevice(void) {
        // Do nothing as of now!
    }
    mraa_result_t writeReg(uint8_t reg, uint8_t data) {
        return i2c->writeReg(reg, data);
    }
        
    mraa_result_t writeReg(uint8_t reg, int8_t data) {
        return i2c->writeReg(reg, (uint8_t)data);
    }
        
    uint8_t readReg(uint8_t reg) {
        return i2c->readReg(reg);
    }
    
    uint16_t readWordReg(uint8_t reg) {
        return i2c->readWordReg(reg);
    }
    
public:
    Accelerometer(int i2cBusNo       = DEFAULT_I2C_BUS_NO,
                  int i2cAddress = DEFAULT_I2C_ADDRESS,
                  int gpioInt1pinNo  = DEFAULT_INT1_PIN) {
        if (i2cBusNo == -1) {
            i2cBusNo = DEFAULT_I2C_BUS_NO;
        }
        if (i2cAddress == -1) {
            i2cAddress = DEFAULT_I2C_ADDRESS;
        }
        // setupPullUp(is2BusNo);
        i2c = new I2c(i2cBusNo);
        i2c->address(i2cAddress);
        i2c->frequency(MRAA_I2C_FAST);
        if (gpioInt1pinNo != -1) {
            gpioInt = new Gpio(gpioInt1pinNo);
            gpioInt->dir(DIR_IN);
            gpioInt->edge(EDGE_NONE);
        }
        initializeDevice();
    }

    ~Accelerometer() {
        delete i2c;
        if (gpioInt != NULL) {
            delete gpioInt;
        }
    }
	// -- set GPIO Interrupt Edge Mode
    int setGPIOIntrEdgeMode(int intno, Edge mode);

	// -- set GPIO Interrupt callback handler 
    mraa_result_t setGPIOIntrEdgeCallback(int intno, Edge mode, void (*fptr)(void*), void* arg);

    void setOutputDataRate(OutputDataRate rate) {
        BWRate bwrate = getBWRate();
        bwrate.rate = rate; 
        setBWRate(bwrate);
    }

    void startMeasure(void) {
        PowerCtl pwr = getPowerCtl();
        pwr.measure = 1;
        setPowerCtl(pwr);
    }

    void stopMeasure(void) {
        PowerCtl pwr = getPowerCtl();
        pwr.measure = 0;
        setPowerCtl(pwr);
    }

    // --- Low Level Interface ---
	// -- get DeviceId
    uint8_t getDeviceId(void);

    // -- Set/Get of Tap detect param (62.5mg/LSB) - Tap detect threshold
    void setTap(uint8_t tap);
    uint8_t getTap(void);

    // -- Set/Get of Tap detect param (mg) - Tap detect threshold
    void setTapmg(int tapmg);
    int getTapmg(void);

    // -- Set/Get of X/Y/Z offset (15.6mg/LSB) - X/Y/Z parameter offsets
    void setOffset(int8_t xofs, int8_t yofs, int8_t zofs);
    void getOffset(int8_t &xofs, int8_t &yofs, int8_t &zofs);

    // -- Set/Get of X/Y/Z offset (mg) - X/Y/Z parameter offsets
    void setOffsetmg(int xofsmg, int yofsmg, int zofsmg);
    void getOffsetmg(int &xofsmg, int &yofsmg, int &zofsmg);

    // -- Set/Get DUR (0.625ms/LSB) - if set to 0, tap/doubletap function will be disabled.
    void setDur(uint8_t dur);
    uint8_t getDur(void);
    
    // -- Set/Get Latent (1.25ms/LSB) - if set to 0, doubletap function will be disabled.
    void setLatent(uint8_t latent);
    uint8_t getLatent(void);

    // -- Set/Get Window (1.25ms/LSB) - if set to 0, doubletap function will be disabled.
    void setWindow(uint8_t window);
    uint8_t getWindow(void);

    // -- Set/Get Thresh_Act(62.5mg/LSB) -  "ACTIVE" detection threshhold value.
    void setThreahAct(uint8_t threshAct);
    uint8_t getThreadhAct(void);

    // -- Set/Get ThreshInact(62.5mg/LSB) - "INACTIVE" detection threshhold value.
    void setThreshInAct(uint8_t threshInAct);
    uint8_t getThreshInAct(void);
    
    // -- Set/Get TimeInAct(1sec/LSB). 
    void setTimeInAct(uint8_t timeInAct);
    uint8_t getTimeInAct(void);

    // -- Set/Get ActInActCtl
    typedef union {
        struct {
            uint8_t inactZ     : 1;
            uint8_t inactY     : 1;
            uint8_t inactX     : 1;
            uint8_t inactAcDc  : 1;
            uint8_t actZEnable : 1;
            uint8_t actYEnable : 1;
            uint8_t actXEnable : 1;
            uint8_t actAcDc    : 1;
        };
        uint8_t c;
    } ActInActCtrl;
    void setActInActCtl(ActInActCtrl actinactctrl);
    ActInActCtrl getActInActCtl(void);
    
    // -- Set/Get ThreshFF(62.5/LSB) - Free Fall Detection Threshold.
    void setThreshFF(uint8_t threshFF);
    uint8_t getThreshFF(void);

    // -- Set/Get TimeFF(5ms/lSB) - Free Fall detection duration
    void setTimeFF(uint8_t timeFF);
    uint8_t geTimeFF(void);

    // -- Set/Get TapAxes 
    typedef union { 
        struct {
            uint8_t tapZEnable : 1;
            uint8_t tapYEnable : 1;
            uint8_t tapXEnable : 1;
            uint8_t suppress   : 1;
            uint8_t reserved0  : 4;
        };
        uint8_t c;
    } TapAxes;
    void setTapAxes(TapAxes tapAxes);
    TapAxes getTapAxes(void);

    // -- Get ActTapStatus
    typedef union {
        struct {
            uint8_t tapZsource : 1; 
            uint8_t tapYsource : 1;
            uint8_t tapXsource : 1;
            uint8_t asleep     : 1;
            uint8_t actZsource : 1;
            uint8_t actYsource : 1;
            uint8_t actXsource : 1;
            uint8_t reserved0  : 1;
        };
        uint8_t c;
    } ActTapStatus;
    ActTapStatus getActTapStatus(void);
    
    // -- Set/Get BWRate
    typedef union {
        struct {
            uint8_t rate      : 4;
            uint8_t lowPower  : 1;
            uint8_t reserved0 : 3;
        };
        uint8_t c;
    } BWRate;
    void setBWRate(BWRate bwrate);
    BWRate getBWRate(void);

    // -- Set/Get PowerCtl
    typedef union {
        struct {
            uint8_t wakeup    : 2;
            uint8_t sleep     : 1;
            uint8_t measure   : 1;
            uint8_t autoSleep : 1;
            uint8_t link      : 1;
            uint8_t reserved0 : 2;
        };
        uint8_t c;
    } PowerCtl;
    void setPowerCtl(PowerCtl powerctl);
    PowerCtl getPowerCtl(void);
    
    // -- Set/Get IntEnable
    typedef union {
        struct {
            uint8_t overrun    : 1;
            uint8_t watermark  : 1;
            uint8_t freeFall   : 1;
            uint8_t inactivity : 1;
            uint8_t activity   : 1;
            uint8_t doubleTap  : 1;
            uint8_t singleTap  : 1;
            uint8_t dataReady  : 1;
        };
        uint8_t c;
    } InterruptMap;
    void setIntEnable(InterruptMap intEnable);
    InterruptMap getIntEnable(void);

    // -- Set/Get IntMap
    void setIntMap(InterruptMap intMap);
    InterruptMap getIntMap(void);

    // -- Get IntSource
    InterruptMap getIntSource(void);

    // -- Set/Get DataFormat
    typedef union {
        struct {
            uint8_t range      : 2;
            uint8_t justify    : 1;
            uint8_t fullRes    : 1;
            uint8_t reserverd0 : 1;
            uint8_t intInvert  : 1;
            uint8_t spi        : 1;
            uint8_t selfTest   : 1;
        };
        uint8_t c;
    } DataFormat;
    void setDataFormat(DataFormat dataFormat);
    DataFormat getDataFormat(void);
    
    // -- Get X/Y/Z Data
    int16_t getXData(void);
    int16_t getYData(void);
    int16_t getZData(void);
    void getXYZData(int16_t &x, int16_t &y, int16_t &z);

    // -- Set/Get FIFO Ctl
    typedef union {
        struct {
            uint8_t samples  : 5;
            uint8_t trigger  : 1;
            uint8_t fifoMode : 2;
        };
        uint8_t c;
    } FifoCtl;
    void setFifoCtl(FifoCtl fifoCtl);
    FifoCtl getFifoCtl(void);
    
    // -- Get FIFO Status
    typedef struct {
        struct {
            uint8_t entries   : 6;
            uint8_t reserved0 : 1;
            uint8_t fifoTrig  : 1;
        };
        uint8_t c;
    } FifoStatus;
    FifoStatus getFifoStatus(void);
};
