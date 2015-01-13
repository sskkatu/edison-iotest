#include "oled_ssd1306.hpp"

int main(int argc, char *argv[]) {
    OLED_ssd1306 oled;
#if (1)
    for (int i=0; i < 128; i++) {
        oled.clearScreen();
        oled.putRect(random()%128, random()%64, random()%128, random()%64, OLED_ssd1306::SET);
        oled.putRect(random()%128, random()%64, random()%128, random()%64, OLED_ssd1306::RESET);
        oled.putRect(random()%128, random()%64, random()%128, random()%64, OLED_ssd1306::XOR);
        oled.putTextFormat(random()%64, random()%56, OLED_ssd1306::TEXT_OVERWRITE, "This is test");
        oled.putTextFormat(random()%64, random()%56, OLED_ssd1306::TEXT_OR, "This is test");
        oled.putTextFormat(random()%64, random()%56, OLED_ssd1306::TEXT_XOR, "This is test");
        oled.putTextFormat(random()%64, random()%56, OLED_ssd1306::TEXT_REVERSE, "This is test");
        oled.putLine(random()%128, random()%64, random()%128, random()%64, OLED_ssd1306::SET);
        oled.putLine(random()%128, random()%64, random()%128, random()%64, OLED_ssd1306::SET);
        oled.putLine(random()%128, random()%64, random()%128, random()%64, OLED_ssd1306::SET);

        oled.flushDisplay();
        sleep(1);
    }
#else
    oled.putRect(0, 0, 127, 63, OLED_ssd1306::SET);
    oled.putRect(32, 16, 96, 48, OLED_ssd1306::XOR);
    for (int i=0; i < 128; i++) {
        oled.putLine(i, 0, i, 63, OLED_ssd1306::SET);
        oled.flushDisplay();
    }
    for (int i=0; i < 64; i++) {
        oled.putLine(0, i, 127, i, OLED_ssd1306::RESET);
        oled.flushDisplay();
    }
    oled.flushDisplay();
#endif    
    exit(0);
}
