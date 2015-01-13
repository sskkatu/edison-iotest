#include "oled_ssd1306.hpp"

using namespace putmode;
int main(int argc, char *argv[]) {
    Oled oled;
    for (int i=0; ; i++) {
        oled.clearScreen();
        oled.putRect(random()%128, random()%64, random()%128, random()%64, SET);
        oled.putRect(random()%128, random()%64, random()%128, random()%64, RESET);
        oled.putRect(random()%128, random()%64, random()%128, random()%64, XOR);
        oled.putTextFormat(random()%64, random()%56, TEXT_OVERWRITE, "This is test");
        oled.putTextFormat(random()%64, random()%56, TEXT_OR, "This is test");
        oled.putTextFormat(random()%64, random()%56, TEXT_XOR, "This is test");
        oled.putTextFormat(random()%64, random()%56, TEXT_REVERSE, "This is test");
        oled.putLine(random()%128, random()%64, random()%128, random()%64, SET);
        oled.putLine(random()%128, random()%64, random()%128, random()%64, RESET);
        oled.putLine(random()%128, random()%64, random()%128, random()%64, XOR);
        oled.putCircle(random()%128, random()%64, random()%32, SET);
        oled.putCircle(random()%128, random()%64, random()%32, RESET);
        oled.putCircle(random()%128, random()%64, random()%32, XOR);
        oled.putTextFormat(0,0, "Count=%08d", i);
        oled.update();
    }
    exit(0);
}
