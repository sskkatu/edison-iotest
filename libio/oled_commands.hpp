#include <cstdint>

#pragma once

namespace SSD1306_CMDS{
    const uint8_t SSD_COMMAND_MODE            = 0x00;
    const uint8_t SSD_DATA_MODE               = 0x40;
    const uint8_t SSD_INVERSE_DISPLAY         = 0xA7;
    const uint8_t SSD_DISPLAY_OFF             = 0xAE;
    const uint8_t SSD_DISPLAY_ON              = 0xAF;
    const uint8_t SSD_SET_CONTRASTLEVEL       = 0x81;
    const uint8_t SSD_EXTERNAL_VCC            = 0x01;
    const uint8_t SSD_INTERNAL_VCC            = 0x02;
    const uint8_t SSD_ACTIVATE_SCROLL         = 0x2F;
    const uint8_t SSD_DEACTIVATE_SCROLL       = 0x2E;
    const uint8_t SCROLL_LEFT                 = 0x00;
    const uint8_t SCROLL_RIGHT                = 0x01;
    const uint8_t SCROLL_2FRAMES              = 0x07;
    const uint8_t SCROLL_3FRAMES              = 0x04;
    const uint8_t SCROLL_4FRAMES              = 0x05;
    const uint8_t SCROLL_5FRAMES              = 0x00;
    const uint8_t SCROLL_25FRAMES             = 0x06;
    const uint8_t SCROLL_64FRAMES             = 0x01;
    const uint8_t SCROLL_128FRAMES            = 0x02;
    const uint8_t SCROLL_256FRAMES            = 0x03;
    const uint8_t VERTICAL_MODE               = 0x01;
    const uint8_t PAGE_MODE                   = 0x01;
    const uint8_t HORIZONTAL_MODE             = 0x02;

    const uint8_t SSD1306_DISPLAYALLON_RESUME = 0xA4;
    const uint8_t SSD1306_DISPLAYALLON        = 0xA5;
    const uint8_t SSD1306_NORMAL_DISPLAY      = 0xA6;
    const uint8_t SSD1306_SETDISPLAYOFFSET    = 0xD3;
    const uint8_t SSD1306_SETCOMPINS          = 0xDA;
    const uint8_t SSD1306_SETVCOMDETECT       = 0xDB;
    const uint8_t SSD1306_SETDISPLAYCLOCKDIV  = 0xD5;
    const uint8_t SSD1306_SETPRECHARGE        = 0xD9;
    const uint8_t SSD1306_SETMULTIPLEX        = 0xA8;
    const uint8_t SSD1306_SETLOWCOLUMN        = 0x00;
    const uint8_t SSD1306_SETHIGHCOLUMN       = 0x10;
    const uint8_t SSD1306_SETSTARTLINE        = 0x40;
    const uint8_t SSD1306_MEMORYMODE          = 0x20;
    const uint8_t SSD1306_COMSCANINC          = 0xC0;
    const uint8_t SSD1306_COMSCANDEC          = 0xC8;
    const uint8_t SSD1306_SEGREMAP            = 0xA0;
    const uint8_t SSD1306_CHARGEPUMP          = 0x8D;
    const uint8_t SSD1306_SET_VERTICAL_SCROLL_AREA             = 0xA3;
    const uint8_t SSD1306_RIGHT_HORIZONTAL_SCROLL              = 0x26;
    const uint8_t SSD1306_LEFT_HORIZONTAL_SCROLL               = 0x27;
    const uint8_t SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29;
    const uint8_t SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  = 0x2A;
    const uint8_t SSD1308_NORMAL_DISPLAY                       = 0xA6;
};

