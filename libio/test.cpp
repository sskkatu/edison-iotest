#include <stdio.h>
#include <signal.h>
#include "mraa.hpp"
#include "oled_ssd1306.hpp"
#include "accelerometer.hpp"
using namespace putmode;
using namespace std;

static void intrHandler_contread(void *arg);
static void intrHandler_tapdetect(void *arg);
static void intrHandler_xaxis(void *arg);

Oled *oled;
Accelerometer *acc;

int main(int argc, char *argv[]) {
    sigset_t ss;
    int ret, signo;

    //acc = new Accelerometer(-1, -1, 26); // Int1 <= GP130(UART1_RX)
    acc = new Accelerometer(-1, 0x1d, 26); // Int1 <= GP130(UART1_RX)
    oled = new Oled();
    Accelerometer::AppProf prof;


    // XYZ calibration setting 
    // acc->setOffset(-4, -1, +13);
#if (1)
    // application : continuous read 
    acc->setGPIOIntrEdgeCallback(1, EDGE_FALLING, intrHandler_contread, NULL);
    acc->createAppProfContinuousRead(prof);
    acc->applyApp(prof);
    acc->dumpAllReg();
    acc->startMeasure();
    intrHandler_contread(NULL);
#elif (0)
    acc->setGPIOIntrEdgeCallback(1, EDGE_FALLING, intrHandler_tapdetect, NULL);
    acc->createAppProfTapDetect(prof);
    acc->applyApp(prof);
    acc->startMeasure();
    acc->dumpAllReg();
    intrHandler_tapdetect(NULL);
#else 
    acc->setGPIOIntrEdgeCallback(1, EDGE_FALLING, intrHandler_xaxis, NULL);
    acc->createAppProfXAxisAccDetect(prof);
    acc->applyApp(prof);
    acc->startMeasure();
    acc->dumpAllReg();
    intrHandler_xaxis(NULL);
#endif

#if (1)
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
    // Waiting for SIGINT (Ctrl-c)
    if (sigwait(&ss, &signo) == 0) {
        acc->stopMeasure();
        delete acc;
        delete oled;
        printf("Stop Measure\n");
        exit(0);
	}
#else
    while (1) {
        sleep(1);
        acc->dumpAllReg();
    }
#endif
    exit(1);
}

static void intrHandler_contread(void *arg) {
    static int16_t gx = 0, gy = 0, gz = 0;
    static const float alpha = 0.8;
    int16_t rx, ry, rz, x, y, z;
    Accelerometer::FifoStatus fifostatus;
    Accelerometer::InterruptMap interruptMap;
    acc->getIntSource(interruptMap);
    acc->getFifoStatus(fifostatus);

    if (fifostatus.fifo_entries >= 0) {
    //        oled->clearScreen();
    for (int i=0; i < fifostatus.fifo_entries+1; i++) {
            acc->getXYZData(rx, ry, rz);
            gx = alpha * gx + (1 - alpha) * rx;
            gy = alpha * gy + (1 - alpha) * ry;
            gz = alpha * gz + (1 - alpha) * rz;
            x = rx - gx;
            y = ry - gy;
            z = rz - gz;
            //oled->putTextFormat(0, i*8, "x=% 3d, y=% 3d, z=% 3d", x, y, z);
            printf("(%02d): x=% 3d, y=% 3d, z=% 3d\n", i, x, y, z);
        }
        //        oled->update();
    }
    //    printf("intr map = to %02x, fifoentries=%d\n", acc->getIntSource(), fifostatus.fifo_entries);
}

static void intrHandler_tapdetect(void *arg) {
    int16_t rx, ry, rz;
    printf("intr map = to %02x\n", acc->getIntSource());
    intrHandler_contread(arg);

    printf("act_tap_stastus = %02x\n", acc->getActTapStatus());
    printf("intr map = to %02x\n", acc->getIntSource());
}

static void intrHandler_xaxis(void *arg) {
    int16_t rx, ry, rz;
    printf("int source = to %02x\n", acc->getIntSource());
    intrHandler_contread(arg);
    printf("act_tap_stastus = %02x\n", acc->getActTapStatus());
    printf("int source = to %02x\n", acc->getIntSource());
}
