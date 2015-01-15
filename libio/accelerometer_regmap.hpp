#include <cstdint>

#pragma once

namespace accelerometer_ADXL345_regmap {
    const uint8_t ADXL345_DEVICE_ID      = 0x00; // R    Device ID
    const uint8_t ADXL345_THRESH_TAP     = 0x1d; // R/W  Tap threshhold
    const uint8_t ADXL345_OFSX           = 0x1e; // R/W  X-axis offset
    const uint8_t ADXL345_OFSY           = 0x1f; // R/W  Y-axis offset
    const uint8_t ADXL345_OFSZ           = 0x20; // R/W  Z-axis offset
    const uint8_t ADXL345_DUR            = 0x21; // R/W  Tap duration
    const uint8_t ADXL345_LATENT         = 0x22; // R/W  Tap latency
    const uint8_t ADXL345_WINDOW         = 0x23; // R/W  Tap window
    const uint8_t ADXL345_THREASH_ACT    = 0x24; // R/W  Activity threshold
    const uint8_t ADXL345_THRESH_INACT   = 0x25; // R/W  Inactivity threshold
    const uint8_t ADXL345_TIME_INACT     = 0x26; // R/W  Inactivity time
    const uint8_t ADXL345_ACT_INACT_CTL  = 0x27; // R/W  Axis enable control for activity and inactivity detection
    const uint8_t ADXL345_THRESH_FF      = 0x28; // R/W  Free-fall threshold
    const uint8_t ADXL345_TIME_FF        = 0x29; // R/W  Free-fall time
    const uint8_t ADXL345_TAP_AXES       = 0x2a; // R/W  Axis xontrol for single tap/double tap
    const uint8_t ADXL345_ACT_TAP_STATUS = 0x2b; // R    Source of single tap/double tap 
    const uint8_t ADXL345_BW_RATE        = 0x2c; // R/W  Data rate and power mode control
    const uint8_t ADXL345_POWER_CTL      = 0x2d; // R/W  Power-saving feature control
    const uint8_t ADXL345_INT_ENABLE     = 0x2e; // R/W  Interrupt enable control
    const uint8_t ADXL345_INT_MAP        = 0x2f; // R/W  Interrupt mapping control
    const uint8_t ADXL345_INT_SOURCE     = 0x30; // R    Source of interrupts
    const uint8_t ADXL345_DATA_FORMAT    = 0x31; // R/W  Data format control
    const uint8_t ADXL345_DATAX0         = 0x32; // R    X-Axis Data 0
    const uint8_t ADXL345_DATAX1         = 0x33; // R    X-Axis Data 1
    const uint8_t ADXL345_DATAY0         = 0x34; // R    Y-Axis Data 0
    const uint8_t ADXL345_DATAY1         = 0x35; // R    Y-Axis Data 1
    const uint8_t ADXL345_DATAZ0         = 0x36; // R    Z-Axis Data 0 
    const uint8_t ADXL345_DATAZ1         = 0x37; // R    Z-Axis Data 1
    const uint8_t ADXL345_FIFO_CTL       = 0x38; // R/W  FIFO control
    const uint8_t ADXL345_FIFO_STATUS    = 0x39; // R    FIFO status
}
