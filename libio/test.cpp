#include <stdio.h>
#include <signal.h>
#include "mraa.hpp"
#include "oled_ssd1306.hpp"
#include "accelerometer.hpp"
using namespace putmode;
using namespace std;

static void interrupt(void *arg);

Oled *oled;
Accelerometer *acc;

int main(int argc, char *argv[]) {
    sigset_t ss;
    int ret, signo;
    Accelerometer::DataFormat dataformat;
    Accelerometer::TapAxes tapaxes;
    Accelerometer::ActInActCtl actinactctl;
    Accelerometer::InterruptMap interruptmap;
    Accelerometer::BWRate bwrate;
    Accelerometer::FifoCtl fifoctl;
    Accelerometer::PowerCtl powerctl;

    //acc = new Accelerometer(-1, -1, 26); // Int1 <= GP130(UART1_RX)
    acc = new Accelerometer(-1, 0x1d,  26); // Int1 <= GP130(UART1_RX)
    oled = new Oled();

    // Block SIGHUP 
    sigemptyset(&ss);
    ret = sigaddset(&ss, SIGINT);
    if (ret != 0) {
        exit(1);
    }
    ret = sigprocmask(SIG_BLOCK, &ss, NULL);
    if (ret != 0) {
        exit(1);
    }

    // XYZ calibration setting 
    acc->setOffset(-4, -1, +13);

    // Configure the timing parameter
    acc->setDur(1);
    acc->setLatent(1);
    acc->setWindow(1);

    // Configure threshold parameter 
    acc->setTap_mg(100);  
    acc->setThreshAct_mg(100);
    acc->setThreshInAct_mg(100);
    acc->setTimeInAct(1);
    acc->setThreshFF_mg(1000);
    acc->setTimeFF_ms(500);

    // Configure Tap 
    tapaxes.supress      = true;
    tapaxes.tap_x_enable = false;
    tapaxes.tap_y_enable = false;
    tapaxes.tap_z_enable = false;
    acc->setTapAxes(tapaxes);

    // Configure act/inact
    actinactctl.act_ac_dc      = false;
    actinactctl.act_x_enable   = false;
    actinactctl.act_y_enable   = false;
    actinactctl.act_z_enable   = false;
    actinactctl.inact_ac_dc    = false;
    actinactctl.inact_x_enable = false;
    actinactctl.inact_y_enable = false;
    actinactctl.inact_z_enable = false;
    acc->setActInActCtl(actinactctl);

    // Configure Fifo Control
    acc->getFifoCtl(fifoctl);
    fifoctl.fifo_mode = Accelerometer::FIFO_MODE_FIFO;
    fifoctl.trigger = false;
    fifoctl.samples = 8;
    acc->setFifoCtl(fifoctl);

    // Configure Interrupt Map (false = INT1, true = INT2)
    interruptmap.data_ready = true;
    interruptmap.single_tap = false;
    interruptmap.double_tap = false;
    interruptmap.activity   = false;
    interruptmap.inactivity = false;
    interruptmap.free_fall  = false;
    interruptmap.watermark  = false;
    interruptmap.overrun    = false;
    acc->setIntMap(interruptmap);

    // Configure Inerrupt enable (true = Enable, false = disable)
    interruptmap.data_ready = false;
    interruptmap.single_tap = false;
    interruptmap.double_tap = false;
    interruptmap.activity   = false;
    interruptmap.inactivity = false;
    interruptmap.free_fall  = false;
    interruptmap.watermark  = true;
    interruptmap.overrun    = false;
    acc->setIntEnable(interruptmap);

    // Configure Sampling rate
    acc->getBWRate(bwrate);
    bwrate.bwrate = Accelerometer::RATE_0100_00;
    acc->setBWRate(bwrate);

    // Configure Dataformat
    dataformat.self_test  = false;
    dataformat.spi        = false;
    dataformat.int_invert = true;
    dataformat.fullres    = true;
    dataformat.justify    = false;
    dataformat.range      = Accelerometer::G_RANGE_16G;
    acc->setDataFormat(dataformat);
    
    // Set interrupt handler
    acc->setGPIOIntrEdgeCallback(1, mraa::EDGE_FALLING, interrupt, &acc);

    // Configure Power control. 'measure' is not start in this moment,
    powerctl.link = false;
    powerctl.auto_sleep = false;
    powerctl.measure = false;
    powerctl.sleep = false;
    powerctl.wakeup_mode = Accelerometer::WAKEUP_FREQ_8;
    acc->setPowerCtl(powerctl);

    // Dump All Register for Debugging
    acc->dumpAllReg();

    // Start measure 
    acc->startMeasure();

    // Waiting for SIGINT (User Ctrl-c)
    if (sigwait(&ss, &signo) == 0) {
        acc->interruptExit(1);
        acc->stopMeasure();
        delete acc;
        delete oled;
        printf("Stop Measure\n");
        exit(0);
    }
    exit(1);
}

static void interrupt(void *arg) {
    int16_t x, y, z;
    Accelerometer::FifoStatus fifostatus;
    Accelerometer::InterruptMap interruptMap;
    acc->getIntSource(interruptMap);
    acc->getFifoStatus(fifostatus);

    if (fifostatus.fifo_entries >= 0) {
        oled->clearScreen();
        for (int i=0; i < fifostatus.fifo_entries; i++) {
            acc->getXYZData(x, y, z);
            oled->putTextFormat(0, i*8, "x=% 3d, y=% 3d, z=% 3d", x, y, z);
        }
        oled->update();
    }
    printf("intr map = to %02x, fifoentries=%d\n", acc->getIntSource(), fifostatus.fifo_entries);
}

