#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>
#include "mraa.hpp"
#include "oled_ssd1306.hpp"
#include "accelerometer.hpp"
#include "getipaddr.hpp"

using namespace putmode;
using namespace std;

static void intrHandler_contread(void *arg);
//static void intrHandler_tapdetect(void *arg);
//static void intrHandler_xaxis(void *arg);
static void enablePullup(int gpioport);
static int isAccelMode(void);
static int information_display(void);
static int analogwatch_display(void);
static int gps_display(void);
static int graph_demo1(void);
static int graph_demo2(void);
static int graph_demo3(void);
static int graph_demo4(void);
static double hour2rad(int hour, int minuite);
static double min2rad(int minuite, int sec);
static double sec2rad(int sec);

#define GPIO_I2C1SDA    7
#define GPIO_I2C1SCL    19
#define GPIO_I2C6SDA    6
#define GPIO_I2C6SCL    8 
#define GPIO_SWITCH     11

#define PI 3.141592653589793

typedef enum {
    DISPLAYMODE_HELLO = 0,
    DISPLAYMODE_ANALOGWATCH,
    DISPLAYMODE_GPS,
    DISPLAYMODE_ACCEL_DC,
    DISPLAYMODE_ACCEL_AC,
    DISPLAYMODE_PEDOMETER,
    DISPLAYMODE_GRAPHICSDEMO1,
    DISPLAYMODE_GRAPHICSDEMO2,
    DISPLAYMODE_GRAPHICSDEMO3,
    DISPLAYMODE_GRAPHICSDEMO4,
    DISPLAYMODE_MAX,
} DisplayMode;

Oled *oled;
Accelerometer *acc;
int displayMode = DISPLAYMODE_HELLO;
int walked = 0;
Gpio *gpio_switch;

static void *thread_func(void *vptr_args) {
    static int current_switch_on = 0;
    static int prev_switch_state = 0;
    static int current_switch_state = 0;
    while (1) {
        current_switch_state = gpio_switch->read();
        if (!current_switch_on && current_switch_state && prev_switch_state) {
            displayMode ++;
            current_switch_on = 1;
            if (displayMode >= DISPLAYMODE_MAX) {
                displayMode = DISPLAYMODE_HELLO;
            }
        }
        if (!current_switch_state && !prev_switch_state) {
            current_switch_on = 0;
        }
        prev_switch_state = current_switch_state;
        usleep(50000);
    }
    return 0;
}


int main(int argc, char *argv[]) {
    pthread_t thread;

    enablePullup(GPIO_I2C1SDA);        // i2c1 SDA
    enablePullup(GPIO_I2C1SCL);        // i2c1 SCL
    enablePullup(GPIO_I2C6SDA);        // i2c6 SDA
    enablePullup(GPIO_I2C6SCL);        // i2c6 SCL

    gpio_switch = new mraa::Gpio(GPIO_SWITCH);
    if (gpio_switch == NULL) {
        fprintf(stderr, "Cannot open GPIO port:%d\n", GPIO_SWITCH);
    }
    gpio_switch->dir(DIR_IN);
    gpio_switch->mode(MODE_PULLDOWN);
    pthread_create(&thread, NULL, thread_func, NULL);

    oled = new Oled(6, 0x3c, 24);
    acc = new Accelerometer(1, 0x1d, 10);

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

    while (1) {
        switch (displayMode) {
        case DISPLAYMODE_HELLO:         information_display();   break;
        case DISPLAYMODE_ANALOGWATCH:   analogwatch_display();   break;
        case DISPLAYMODE_GPS:           gps_display();           break;
        case DISPLAYMODE_GRAPHICSDEMO1: graph_demo1();           break;
        case DISPLAYMODE_GRAPHICSDEMO2: graph_demo2();           break;
        case DISPLAYMODE_GRAPHICSDEMO3: graph_demo3();           break;
        case DISPLAYMODE_GRAPHICSDEMO4: graph_demo4();           break;
        case DISPLAYMODE_ACCEL_AC:
        case DISPLAYMODE_ACCEL_DC:
        default:
            break;
        }
        usleep(10);
    }

    exit(1);
}
static int graph_demo1(void)
{
    int i;
    oled->clearScreen();
    
    for (i=0; i < 64; i++) {
        oled->putLine(rand()%128, rand()%64, rand()%128, rand()%64, PutMode::SET);
    }
    oled->putTextFormat(1,  3,  TextPutMode::TEXT_OVERWRITE, "Graphics DEMO1:Line");

    oled->update();
    return 0;
}

static int graph_demo2(void)
{
    int i;
    oled->clearScreen();
    
    for (i=0; i < 64; i++) {
        oled->putCircle(rand()%128, rand()%64, rand()%32, PutMode::SET);
    }
    oled->putTextFormat(1,  3, TextPutMode::TEXT_OVERWRITE, "Graphics DEMO2:Circle");

    oled->update();
    return 0;
}

static int graph_demo3(void)
{
    int i;
    oled->clearScreen();
    
    for (i=0; i < 16; i++) {
        oled->putRect(rand()%128, rand()%64, rand()%128, rand()%64, PutMode::SET);
        oled->putRect(rand()%128, rand()%64, rand()%128, rand()%64, PutMode::RESET);
        oled->putRect(rand()%128, rand()%64, rand()%128, rand()%64, PutMode::XOR);
    }
    oled->putTextFormat(1,  3,  TextPutMode::TEXT_OVERWRITE, "Graphics DEMO1:Rect");

    oled->update();
    return 0;
}

static int g_rad = 0;

static int graph_demo4(void)
{
    oled->clearScreen();
    oled->putCircle(63, 31, g_rad, PutMode::SET);
    oled->putTextFormat(1,  3, TextPutMode::TEXT_OVERWRITE, "Graphics DEMO4:Circle");
    oled->update();
    g_rad++;
    if (g_rad >= 64) {
        g_rad = 0;
    }
    return 0;
}

static int information_display(void)
{
    static int flg = 0;
    time_t timer;
    struct tm *local;
    const char *ROT = "|/-\\*";

    oled->clearScreen();
    oled->putTextFormat(1,  1, "Hello, Edison.");
    oled->putTextFormat(1, 16, "IP : %s", getIPv4AdrString("wlan0"));
    timer = time(NULL);
    local = localtime(&timer);
    oled->putTextFormat(3, 32, "Date : %04d/%02d/%02d",
                  local->tm_year + 1900,
                  local->tm_mon + 1,
                  local->tm_mday);
    oled->putTextFormat(3, 44, "Time :   %02d:%02d:%02d",
                  local->tm_hour,
                  local->tm_min,
                  local->tm_sec);
    
    flg++;
    //    oled->printTextFormat(100, 1, mraa_gpio_read(gpio_switch) ? "1" : "0") ;
    oled->putTextFormat(112, 1, "%c", ROT[flg%5]);
    oled->update();
    return 0;
}

static double hour2rad(int hour, int minuite) {
    double hourdegree = (((12 - hour) + 3) % 12) * 30.0;
    double mindegree = minuite * 30.0 / 60.0;
    return (hourdegree - mindegree)* PI / 180;
}

static double min2rad(int minuite, int second) {
    double mindegree = (((60-minuite) + 15) % 60) * 6.0;
    double secdegree = second * 6.0 / 60.0;
    return (mindegree - secdegree) * PI / 180;
}

static double sec2rad(int second) {
    double secdegree = (((60-second) + 15) % 60) * 6.0;
    return (secdegree) * PI / 180;
}

static int analogwatch_display(void){
#define CX 96
#define CY 31
#define R  31
    time_t timer;
    struct tm *local;
    int hour, min, sec;
    double radhour, radmin, radsec;
    timer = time(NULL);
    local = localtime(&timer);
    hour = local->tm_hour;
    min = local->tm_min;
    sec = local->tm_sec;
        
    oled->clearScreen();
    oled->putTextFormat(0, 0, "DateTime");
    oled->putCircle(CX, CY, R, PutMode::SET);
    oled->putCircle(CX, CY, 1, PutMode::SET);
    oled->putTextString(CX-3, 3, "0");
    oled->putTextString(CX+24, 28, "3");
    oled->putTextString(CX-3, 54, "6");
    oled->putTextString(CX-27, 28, "9");

    radhour = hour2rad(hour % 12, min);
    radmin = min2rad(min, sec);
    radsec = sec2rad(sec);

    oled->putLine(CX, CY, CX + 13 * cos(radhour), CY - 13 * sin(radhour));
    oled->putLine(CX, CY, CX + 20 * cos(radmin), CY - 20 * sin(radmin));
    oled->putLine(CX, CY, CX + 30 * cos(radsec), CY - 30 * sin(radsec));

    oled->putTextFormat(0, 12, "%04d/%02d/%02d",
                  local->tm_year + 1900,
                  local->tm_mon + 1,
                  local->tm_mday);

    oled->putTextFormat(12, 22, "%02d:%02d:%02d",
                  local->tm_hour,
                  local->tm_min,
                  local->tm_sec);
    

    oled->update();
    return 0;
}


static int gps_display(void) {
    oled->clearScreen();
    oled->putTextFormat(1, 0, "GPS:");
    oled->putTextFormat(1, 12, " Under construction");
    oled->update();
    return 0;
}

static int isAccelMode(void) {
    return displayMode == DISPLAYMODE_ACCEL_AC || 
        displayMode == DISPLAYMODE_ACCEL_DC || 
        displayMode == DISPLAYMODE_PEDOMETER;
}

static void intrHandler_contread(void *arg) {
#define WALKED_THRESH 256
    static int16_t gx = 0, gy = 0, gz = 0;
    static const float alpha = 0.8;
    int16_t rx, ry, rz, x, y, z;
    Accelerometer::FifoStatus fifostatus;
    Accelerometer::InterruptMap interruptMap;
    acc->getIntSource(interruptMap);
    acc->getFifoStatus(fifostatus);

    if (isAccelMode()) {
        oled->clearScreen();
        if (displayMode == DISPLAYMODE_ACCEL_AC) {
            oled->putTextFormat(0, 0, "Accelerometer:AC");
        } else if (displayMode == DISPLAYMODE_ACCEL_DC) {
            oled->putTextFormat(0, 0, "Accelerometer:DC");
        } else if (displayMode == DISPLAYMODE_PEDOMETER) {
            oled->putTextFormat(0, 0, "Pedometer");
        }
        
    }
    if (fifostatus.fifo_entries >= 0) {
        for (int i=0; i < fifostatus.fifo_entries+1; i++) {
            acc->getXYZData(rx, ry, rz);
            gx = alpha * gx + (1 - alpha) * rx;
            gy = alpha * gy + (1 - alpha) * ry;
            gz = alpha * gz + (1 - alpha) * rz;
            x = rx - gx;
            y = ry - gy;
            z = rz - gz;
            if (isAccelMode()) {
                if (displayMode == DISPLAYMODE_ACCEL_AC) {
                    oled->putTextFormat(0, (i+1)*8, "x=% 3d, y=% 3d, z=% 3d", x, y, z);
                    //            printf("(%02d): x=% 3d, y=% 3d, z=% 3d\n", i, x, y, z);
                } else if (displayMode == DISPLAYMODE_ACCEL_DC) {
                    oled->putTextFormat(0, (i+1)*8, "x=% 3d, y=% 3d, z=% 3d", rx, ry, rz);
                    //            printf("(%02d): x=% 3d, y=% 3d, z=% 3d\n", i, rx, ry, rz);
                }
            }
            if (sqrt(pow(abs(x), 2) + pow(abs(y), 2) + pow(abs(z), 2)) > WALKED_THRESH) {
                walked++;
            }
        }
    }
    if (displayMode == DISPLAYMODE_PEDOMETER) {
        oled->putTextFormat(28, 24, "Detected:%d", walked/2);
    }
    if (isAccelMode()) {
        oled->update();
    }
    //    printf("intr map = to %02x, fifoentries=%d\n", acc->getIntSource(), fifostatus.fifo_entries);
}

#if (0)
static void intrHandler_tapdetect(void *arg) {
    printf("intr map = to %02x\n", acc->getIntSource());
    intrHandler_contread(arg);

    printf("act_tap_stastus = %02x\n", acc->getActTapStatus());
    printf("intr map = to %02x\n", acc->getIntSource());
}

static void intrHandler_xaxis(void *arg) {
    printf("int source = to %02x\n", acc->getIntSource());
    intrHandler_contread(arg);
    printf("act_tap_stastus = %02x\n", acc->getActTapStatus());
    printf("int source = to %02x\n", acc->getIntSource());
}
#endif

static void enablePullup(int gpioport)
{
     mraa_gpio_context gpio = mraa_gpio_init(gpioport);
     if (gpio == NULL) {
          fprintf(stderr, "Cannot open GPIO port:%d\n", gpioport);
     }
     mraa_gpio_mode(gpio, MRAA_GPIO_PULLUP);
     mraa_gpio_close(gpio);
}
