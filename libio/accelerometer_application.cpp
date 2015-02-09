#include <cstdint>
#include <stdio.h>
#include <string.h>
#include "accelerometer.hpp"

void Accelerometer::applyApp(const Accelerometer::AppProf &p) {
    // time/delay settins
    setDur_us(p.dur_us);
    setLatent_us(p.latent_us);
    setWindow_us(p.window_us);
    setTimeFF_ms(p.time_ff_us);
    setTimeInAct(p.time_inact_s);

    // g threshold settings
    setTap_mg(p.thresh_tap_mg);
    setThreshAct_mg(p.thresh_act_mg);
    setThreshInAct_mg(p.thresh_inact_mg);
    setThreshFF_mg(p.thresh_ff_mg);

    // XYZ Offset (for calibrating)
    setOffset_mg(p.xofs_mg, p.yofs_mg, p.zofs_mg);
        
    // Act/Inact Settings
    setActInActCtl(p.actinactctl);
    // TapAxes Settings
    setTapAxes(p.tapaxes);
    // BWRate Settings
    setBWRate(p.bwrate);
    // IntMap
    setIntMap(p.intmap);
    // DataFormat
    setDataFormat(p.dataformat);
    // FIFO Ctl
    setFifoCtl(p.fifoctl);
    // PowerCtl Settings
    setPowerCtl(p.powerctl);
    // IntEnable
    setIntEnable(p.intenable);
}

void Accelerometer::createAppProfContinuousRead(Accelerometer::AppProf &p) {
    p = {};

    p.bwrate.bwrate = Accelerometer::RATE_0200_00;
    p.bwrate.low_power = false;

    p.powerctl.wakeup_mode = Accelerometer::WAKEUP_FREQ_8;

    p.intenable.watermark = true;

    p.dataformat.int_invert = true;
    p.dataformat.fullres = true;
    p.dataformat.range = Accelerometer::G_RANGE_2G;

    p.fifoctl.fifo_mode = Accelerometer::FIFO_MODE_FIFO;
    p.fifoctl.samples = 8;
}

void Accelerometer::createAppProfTapDetect(Accelerometer::AppProf &p) {
    p = {};
    p.dur_us = 10000;
    p.thresh_tap_mg = 500;
    p.bwrate.bwrate = Accelerometer::RATE_0200_00;
    p.tapaxes.tap_x_enable = true;
    p.tapaxes.tap_y_enable = true;
    //    p.tapaxes.tap_z_enable = true;
    p.powerctl.wakeup_mode = Accelerometer::WAKEUP_FREQ_8;

    p.intenable.single_tap = true;
    p.intenable.watermark  = true;

    p.dataformat.int_invert = true;
    p.dataformat.fullres = true;
    p.dataformat.range = Accelerometer::G_RANGE_16G;

    p.fifoctl.fifo_mode = Accelerometer::FIFO_MODE_FIFO;
    p.fifoctl.samples = 31;
}

void Accelerometer::createAppProfTapDubuletapDetect(Accelerometer::AppProf &p) {
    p = {};
    // TODO: Not implemented yet
}

void Accelerometer::createAppProfFreefallDetect(Accelerometer::AppProf &p) {
    p = {};
    // TODO: Not implemented yet
}

void Accelerometer::createAppProfXAxisAccDetect(Accelerometer::AppProf &p) {
    p = {};

    p.thresh_act_mg = 250;
    p.thresh_inact_mg = 250;
    p.time_inact_s = 1;
    
    p.actinactctl.act_ac_dc = true;
    p.actinactctl.act_x_enable = true;
    p.actinactctl.act_y_enable = false;
    p.actinactctl.act_z_enable = false;
    p.actinactctl.inact_ac_dc = true;
    p.actinactctl.inact_x_enable = true;
    p.actinactctl.inact_y_enable = false;
    p.actinactctl.inact_z_enable = false;
    
    p.bwrate.bwrate = Accelerometer::RATE_0100_00;
    p.bwrate.low_power = false;

    p.powerctl.link = true;
    p.powerctl.auto_sleep = true;
    p.powerctl.wakeup_mode = Accelerometer::WAKEUP_FREQ_8;

    p.intenable.activity  = true;
    p.intenable.inactivity = true;
    p.intenable.watermark = true;

    p.dataformat.int_invert = true;
    p.dataformat.fullres    = false;
    p.dataformat.justify    = true;
    p.dataformat.range      = Accelerometer::G_RANGE_2G;

    p.fifoctl.fifo_mode = Accelerometer::FIFO_MODE_FIFO;
    p.fifoctl.samples = 1;
}

void Accelerometer::createAppProfYAxisAccDetect(Accelerometer::AppProf &p) {
    p = {};
    // TODO: Not implemented yet
}

void Accelerometer::createAppProfZAxisAccDetect(Accelerometer::AppProf &p) {
    p = {};
    // TODO: Not implemented yet
}

void Accelerometer::createAppProfRotationDetect(Accelerometer::AppProf &p) {
    p = {};
    // TODO: Not implemented yet
}

