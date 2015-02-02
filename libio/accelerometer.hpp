#pragma once
#include <cstdint>
#include "mraa.hpp"

using namespace mraa;

/**
 * Confirming products :
 *  Aitendo - Multi function Sensor [ATD-M4S]  (Accelometor/Gyroscope/Compass/Barometer)
 *   http://www.aitendo.com/product/9551
 *
 *  Akizukidenshi - 3-Axis Sensor Module ADXL345
 *   http://akizukidenshi.com/catalog/g/gM-06724/
 *
 * DataSheets :
 * 3-Axis Accelerometer/Analog Devices ADXL345
 *    http://aitendo3.sakura.ne.jp/aitendo_data/product_img/sensor/multi/GY80/ADXL345.pdf
 *
 */

class Accelerometer {
public:

    // BW_RATE Reg : Output Data Rate (0.10 - 3200Hz)
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

    // POWER_CTL Reg: Data read freq while in the sleep mode
    typedef enum {
        WAKEUP_FREQ_8 = 0,
        WAKEUP_FREQ_4,
        WAKEUP_FREQ_2,
        WAKEUP_FREQ_1,
    } WakeUpMode;

    // DATA_FORMAT Reg: XYZ Data 'g' range (2g - 16g)
    typedef enum {
        G_RANGE_2G = 0,
        G_RANGE_4G,
        G_RANGE_8G,
        G_RANGE_16G,
    } GRange;

    // FIFO_CTL Reg: FIFO Mode 
    typedef enum {
        FIFO_MODE_BYPASS  = 0,
        FIFO_MODE_FIFO, 
        FIFO_MODE_STREAM,
        FIFO_MODE_TRIGGER,
    } FifoMode;
    
private:
    // Using IO Ports 
    I2c* i2c = NULL;
    Gpio *gpioInt[2] = {NULL, NULL};  // INT1 and INT2 

    // IO Port defaults
    static const int     DEFAULT_I2C_BUS_NO    =    6;
    static const uint8_t DEFAULT_I2C_ADDRESS   = 0x53;
    static const int     DEFAULT_INT1_PIN      =   -1;
    static const int     DEFAULT_INT2_PIN      =   -1;

    // Units convresion constants (mili-gravity)
    static const int     TAP_RESOLUTION_MG     =  625; // 62.5mg
    static const int     OFS_RESOLUTION_MG     =  155; // 15.5mg
    static const int     ACT_RESOLUTION_MG     =  625; // 62.5mg
    static const int     INACT_RESOLUTION_MG   =  625; // 62.5mg
    static const int     THRESH_FF_MG          =  625; // 62.6mg
    

    // Units convresion constants (micro-seconds)
    static const int     DUR_RESOLUTION_US     =  625; // 625us
    static const int     LATENT_RESOLUTION_US  = 1250; // 1250us(=1.25ms)
    static const int     WINDOW_RESOLUTION_US  = 1250; // 1250us(=1.25ms)

    // Units conversion constants (milli seconds) 
    static const int     TIME_FF_MS            =    5; // 5ms

    // Unsigned data write to Register (1-byte)
    mraa_result_t writeReg(uint8_t reg, uint8_t data) {
        return i2c->writeReg(reg, data);
    }
    
    // Signed data write to Regisger (1-byte)
    mraa_result_t writeReg(uint8_t reg, int8_t data) {
        return i2c->writeReg(reg, (uint8_t)data);
    }

    // Unsigend read data from Register (1-byte)
    uint8_t readReg(uint8_t reg) {
        return i2c->readReg(reg);
    }

    // Unsigned read data from Register (word = 2-byte)
    uint16_t readWordReg(uint8_t reg) {
        return i2c->readWordReg(reg);
    }

    // Convert unit-value to reg. specification-value
    uint8_t convUtoR(int uval, int perUnit, int perUnitPower = 1) {
        int result = (uval * perUnitPower) / perUnit ;
        return (result > 255) ? 255 : (uint8_t)result;
    }

    // Convert register specification-value to unit-value
    int convRtoU(uint8_t rval, int perUnit, int perUnitPower = 1) {
        return ((int)(rval) * perUnit / perUnitPower);
    }

public:
    // Constructor 
    // Note : Arguments 'interrupt pin number' aren't mandatory.
    Accelerometer(int i2cBusNo     = DEFAULT_I2C_BUS_NO,
                  int i2cAddress   = DEFAULT_I2C_ADDRESS,
                  int gpioInt1pinNo = DEFAULT_INT1_PIN, 
                  int gpioInt2pinNo = DEFAULT_INT2_PIN) {
        if (i2cBusNo == -1) {
            i2cBusNo = DEFAULT_I2C_BUS_NO;
        }
        if (i2cAddress == -1) {
            i2cAddress = DEFAULT_I2C_ADDRESS;
        }
        i2c = new I2c(i2cBusNo);
        i2c->address(i2cAddress);
        i2c->frequency(MRAA_I2C_FAST);

        if (gpioInt1pinNo >= 0) {
            gpioInt[0] = new Gpio(gpioInt1pinNo);
            gpioInt[0]->dir(DIR_IN);
            gpioInt[0]->edge(EDGE_NONE);
        } else {
            gpioInt[0] = NULL; // No use Interrupt No.1
        }
        if (gpioInt2pinNo >= 0) {
            gpioInt[1] = new Gpio(gpioInt2pinNo);
            gpioInt[1]->dir(DIR_IN);
            gpioInt[1]->edge(EDGE_NONE);
        } else {
            gpioInt[1] = NULL; // No use Interrupt No.2
        }
        clearRemainingFifoData();
    }

    ~Accelerometer() {
        delete i2c;
        if (gpioInt[0] != NULL) {
            delete gpioInt[0];
        }
        if (gpioInt[1] != NULL) {
            delete gpioInt[1];
        }
    }

	// -- Set GPIO Interrupt callback handler 
    mraa_result_t setGPIOIntrEdgeCallback(int intno, Edge mode, void (*fptr)(void*), void* arg);

    // -- Exits callback function
    mraa_result_t interruptExit(int intno);

	// -- get DeviceId
    uint8_t getDevId(void);

    // -- Set/Get of Tap detect param (62.5mg/LSB) - Tap detect threshold
    void setTap(uint8_t tap);
    uint8_t getTap(void);

    // -- Set/Get of Tap detect param (mg) - Tap detect threshold
    void setTap_mg(int tapmg);
    int getTap_mg(void);

    // -- Set/Get of X/Y/Z offset (15.6mg/LSB) - X/Y/Z parameter offsets
    void setOffset(int8_t xofs, int8_t yofs, int8_t zofs);
    void getOffset(int8_t &xofs, int8_t &yofs, int8_t &zofs);

    // -- Set/Get of X/Y/Z offset (mg) - X/Y/Z parameter offsets
    void setOffset_mg(int xofsmg, int yofsmg, int zofsmg);
    void getOffset_mg(int &xofsmg, int &yofsmg, int &zofsmg);

    // -- Set/Get DUR (0.625ms/LSB) - if set to 0, tap/doubletap function will be disabled.
    void setDur(uint8_t dur);
    uint8_t getDur(void);

    // -- Set/Get DUR (us) - if set to 0, tap/doubletap functionwill be deisabled.
    void setDur_us(useconds_t durus);
    useconds_t getDur_us(void);
    
    // -- Set/Get Latent (1.25ms/LSB) - if set to 0, doubletap function will be disabled.
    void setLatent(uint8_t latent);
    uint8_t getLatent(void);

    // -- Set/Get Latent (us) - if set to 0, doubletap function will be disabled.
    void setLatent_us(useconds_t latent_us);
    useconds_t getLatent_us(void);

    // -- Set/Get Window (1.25ms/LSB) - if set to 0, doubletap function will be disabled.
    void setWindow(uint8_t window);
    uint8_t getWindow(void);

    // -- Set/Get Window (us) - if set to 0, doubletap function will be disabled.
    void setWindow_us(useconds_t window_us);
    useconds_t getWindow_us(void);

    // -- Set/Get Thresh_Act(62.5mg/LSB) -  "ACTIVE" detection threshhold value.
    void setThreshAct(uint8_t threshAct);
    uint8_t getThreshAct(void);

    // -- Set/Get Thresh_Act(mg) -  "ACTIVE" detection threshhold value.
    void setThreshAct_mg(int threshAct_mg);
    int getThreshAct_mg(void);

    // -- Set/Get ThreshInact(62.5mg/LSB) - "INACTIVE" detection threshhold value.
    void setThreshInAct(uint8_t threshInAct);
    uint8_t getThreshInAct(void);

    // -- St/Get ThreshInact(mg) - "INACTIVE" detection threshhold value.
    void setThreshInAct_mg(int threshInAct_mg);
    int getThreshInAct_mg(void);
    
    // -- Set/Get TimeInAct(1sec/LSB). 
    void setTimeInAct(uint8_t timeInAct);
    uint8_t getTimeInAct(void);

    // -- Set/Get ActInActCtl
    static const uint8_t SET_ACT_INACT_CTL_ACCT_AC_DC     = 0x80;
    static const uint8_t SET_ACT_INACT_CTL_ACT_X_ENABLE   = 0x40;
    static const uint8_t SET_ACT_INACT_CTL_ACT_Y_ENABLE   = 0x20;
    static const uint8_t SET_ACT_INACT_CTL_ACT_Z_ENABLE   = 0x10;
    static const uint8_t SET_ACT_INACT_CTL_INACT_ACDC     = 0x08;
    static const uint8_t SET_ACT_INACT_CTL_INACT_X_ENABLE = 0x04;
    static const uint8_t SET_ACT_INACT_CTL_INACT_Y_ENABLE = 0x02;
    static const uint8_t SET_ACT_INACT_CTL_INACT_Z_ENABLE = 0x01;
    void setActInActCtl(uint8_t c);
    uint8_t getActInActCtl(void);

    typedef struct {
        bool act_ac_dc;
        bool act_x_enable;
        bool act_y_enable;
        bool act_z_enable;
        bool inact_ac_dc;
        bool inact_x_enable;
        bool inact_y_enable;
        bool inact_z_enable;
    } ActInActCtl;
    void setActInActCtl(ActInActCtl c);
    void getActInActCtl(ActInActCtl &c);
    
    // -- Set/Get ThreshFF(62.5/LSB) - Free Fall Detection Threshold.
    void setThreshFF(uint8_t threshFF);
    uint8_t getThreshFF(void);
    void setThreshFF_mg(int threshFF_mg);
    int getThreshFF_mg(void);

    // -- Set/Get TimeFF(5ms/lSB) - Free Fall detection duration
    void setTimeFF(uint8_t timeFF);
    uint8_t getTimeFF(void);
    void setTimeFF_ms(int timeFF_ms);
    int getTimeFF_ms(void);

    // -- Set/Get TapAxes 
    static const uint8_t TAPAXES_SUPRESSS     = 0x08;
    static const uint8_t TAPAXES_TAP_X_ENABLE = 0x04;
    static const uint8_t TAPAXES_TAP_Y_ENABLE = 0x02;
    static const uint8_t TAPAXES_TAP_Z_ENABLE = 0x01;
    void setTapAxes(uint8_t c);
    uint8_t getTapAxes(void);

    typedef struct {
        bool supress;
        bool tap_x_enable;
        bool tap_y_enable;
        bool tap_z_enable;
    } TapAxes;
    void setTapAxes(TapAxes c);
    void getTapAxes(TapAxes &c);

    // -- Get ActTapStatus
    static const uint8_t ACT_TAP_STATUS_ACT_X_SOURCE = 0x40;
    static const uint8_t ACT_TAP_STATUS_ACT_Y_SOURCE = 0x20;
    static const uint8_t ACT_TAP_STATUS_ACT_Z_SOURCE = 0x10;
    static const uint8_t ACT_TAP_STATUS_ASLEEP       = 0x08;
    static const uint8_t ACT_TAP_STATUS_TAP_X_SOURCE = 0x04;
    static const uint8_t ACT_TAP_STATUS_TAP_Y_SOURCE = 0x02;
    static const uint8_t ACT_TAP_STATUS_TAP_Z_SOURCE = 0x01;
    uint8_t getActTapStatus(void);

    typedef struct {
        bool act_x_source;
        bool act_y_source;
        bool act_z_source;
        bool asleep;
        bool tap_x_source;
        bool tap_y_source;
        bool tap_z_source;
    } ActTapStatus;
    void getActTapStatus(ActTapStatus &c);

    // -- Set/Get BWRate
    static const uint8_t BWRATE_RATE_MASK = 0x0F;
    static const uint8_t BWRATE_LOWPOWER  = 0x10;
    void setBWRate(uint8_t bwrate);
    uint8_t getBWRate(void);

    typedef struct {
        int bwrate;
        bool low_power; 
    } BWRate;
    void setBWRate(BWRate c);
    void getBWRate(BWRate &c);

    // -- Set/Get PowerCtl
    static const uint8_t POWERCTL_LINK        = 0x20;
    static const uint8_t POWERCTL_AUTOSLEEP   = 0x10;
    static const uint8_t POWERCTL_MEASURE     = 0x08;
    static const uint8_t POWERCTL_SLEEP       = 0x04;
    static const uint8_t POWERCTL_WAKEUP_MASK = 0x03;

    void setPowerCtl(uint8_t powerctl);
    uint8_t getPowerCtl(void);
    typedef struct {
        bool link;
        bool auto_sleep;
        bool measure;
        bool sleep;
        WakeUpMode wakeup_mode;
    } PowerCtl;
    void setPowerCtl(PowerCtl c);
    void getPowerCtl(PowerCtl &c);
    
    // -- Set/Get IntEnable
    // Note: InterruptMap is commonly useed by IntEnable/IntMap/IntSource
    static const uint8_t INERRUPTMAP_DATAREADY  = 0x80;
    static const uint8_t INERRUPTMAP_SINGLETAP  = 0x40;
    static const uint8_t INERRUPTMAP_DOUBLETAP  = 0x20;
    static const uint8_t INERRUPTMAP_ACTIVITY   = 0x10;
    static const uint8_t INERRUPTMAP_INACTIVITY = 0x08;
    static const uint8_t INERRUPTMAP_FREEFALL   = 0x04;
    static const uint8_t INERRUPTMAP_WATERMARK  = 0x02;
    static const uint8_t INERRUPTMAP_OVERRUN    = 0x01;
    void setIntEnable(uint8_t intEnable);
    uint8_t getIntEnable(void);

    typedef struct {
        bool data_ready;
        bool single_tap;
        bool double_tap;
        bool activity;
        bool inactivity;
        bool free_fall;
        bool watermark;
        bool overrun;
    } InterruptMap;

    void setIntEnable(InterruptMap c);
    void getIntEnable(InterruptMap &c);

    // -- Set/Get IntMap
    void setIntMap(uint8_t c);
    uint8_t getIntMap(void);
    void setIntMap(InterruptMap c);
    void getIntMap(InterruptMap &c);

    // -- Get IntSource
    uint8_t getIntSource(void);
    void getIntSource(InterruptMap &c);

    // -- Set/Get DataFormat
    static const uint8_t DATAFORMAT_SELF_TEST  = 0x80;
    static const uint8_t DATAFORMAT_SPI        = 0x40;
    static const uint8_t DATAFORMAT_INT_INVERT = 0x20;
    static const uint8_t DATAFORMAT_FULLRES    = 0x08;
    static const uint8_t DATAFORMAT_JUSTIFY    = 0x04;
    static const uint8_t DATAFORMAT_RANGE_MASK = 0x03;
    void setDataFormat(uint8_t c);
    uint8_t getDataFormat(void);

    typedef struct {
        bool self_test;
        bool spi;
        bool int_invert;
        bool fullres;
        bool justify;
        GRange range; 
    } DataFormat;
    void setDataFormat(DataFormat c);
    void getDataFormat(DataFormat &c);
    
    // -- Get X/Y/Z Data
    int16_t getXData(void);
    int16_t getYData(void);
    int16_t getZData(void);
    void getXYZData(int16_t &x, int16_t &y, int16_t &z);

    // -- Set/Get FIFO Ctl
    static const uint8_t FIFO_MODE_MASK       = 0xc0;
    static const uint8_t FIFOCTL_TRIGGER      = 0x20;
    static const uint8_t FIFOCTL_SAMPLES_MASK = 0x1F;
    void setFifoCtl(uint8_t c);
    uint8_t getFifoCtl(void);

    typedef struct {
        FifoMode fifo_mode;
        bool trigger;
        int samples;
    } FifoCtl;
    void setFifoCtl(FifoCtl c);
    void getFifoCtl(FifoCtl &c);

    // -- Get FIFO Status
    static const uint8_t FIFO_STATUS_FIFO_TRIG         = 0x80;
    static const uint8_t FIFO_STATUS_FIFO_ENTRIES_MASK = 0x3F;
    uint8_t getFifoStatus(void);

    typedef struct {
        bool fifo_trig;
        int fifo_entries;
    } FifoStatus;
    void getFifoStatus(FifoStatus &c);

    // -- Utilities / Debug
    void startMeasure(void);
    void stopMeasure(void);
    void clearRemainingFifoData(void);
    void dumpAllReg(void);
    
    // -- Application Profile
    typedef struct {
        // time/delay settins
        int dur_us;
        int latent_us;
        int window_us;
        int time_ff_us;
        int time_inact_s;

         // g threshold settings
        int thresh_tap_mg;
        int thresh_act_mg;
        int thresh_inact_mg;
        int thresh_ff_mg;

        // XYZ Offset (for calibrating)
        int xofs_mg;
        int yofs_mg;
        int zofs_mg;

        // Act/Inact Settings
        ActInActCtl actinactctl;
        // TapAxes Settings
        TapAxes tapaxes;
        // BWRate Settings
        BWRate bwrate;
        // PowerCtl Settings
        PowerCtl powerctl;
        // IntEnable
        InterruptMap intenable;
        // IntMap
        InterruptMap intmap;
        // DataFormat
        DataFormat dataformat;
        // FIFO Ctl
        FifoCtl fifoctl;
    } AppProf;
    
    void applyApp(const AppProf &p);
    void createAppProfContinuousRead(AppProf &p);
    void createAppProfTapDetect(AppProf &p);
    void createAppProfTapDubuletapDetect(AppProf &p);
    void createAppProfFreefallDetect(AppProf &p);
    void createAppProfXAxisAccDetect(AppProf &p);
    void createAppProfYAxisAccDetect(AppProf &p);
    void createAppProfZAxisAccDetect(AppProf &p);
    void createAppProfRotationDetect(AppProf &p);
};
