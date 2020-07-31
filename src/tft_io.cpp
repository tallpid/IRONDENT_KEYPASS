#include "tft_io.h"

namespace tft_io {

TFT_eSPI tft = TFT_eSPI(135, 240);

#define BACKGROUND_COLOR TFT_DARKGREY
#define FONT_COLOR TFT_WHITE

void show_logo();

void start_tft(void) {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(BACKGROUND_COLOR);

    /*
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);
    */

    if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
         pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
         digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }

    tft.setSwapBytes(true);
    show_logo();
}

void clean_screen(void) {
    tft.fillScreen(TFT_DARKGREY);
}

void print_tft_text(const char* output) {
    tft.setRotation(1);
    tft.setTextSize(2);
    tft.setTextColor(FONT_COLOR);
    tft.drawString(output, tft.height() / 2 - 40, tft.width() / 4);
}

void show_logo() {
    tft.setRotation(1);
    tft.setTextSize(3);
    tft.setTextColor(FONT_COLOR);
    tft.fillScreen(BACKGROUND_COLOR);

    tft.drawString("IRONDENT", tft.height() / 2 - 12, tft.width() / 4 - 10);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("bag of salt", tft.height() / 2 - 7, tft.width() / 4 + 20);
}

void show_switch(bool is_connected) {
    tft.setRotation(1);
    tft.setTextSize(2);
    tft.setTextColor(FONT_COLOR);

    for (int i = tft.width() / 4 - 50; i < tft.width() / 4 + 60; i += 10) {
        tft.drawString("|", tft.height() / 2 + 120, i);
    }

    tft.drawString("Y", tft.height() / 2 + 135, tft.width() / 4 + 50);// tft.width() / 4 - 10);
    tft.drawString("N", tft.height() / 2 + 135, tft.width() / 4 - 50);
    tft.drawString(is_connected ? "ON" : "OFF", tft.height() / 2 + 130, tft.width() / 4);
    

}

void show_help() {
    tft.setRotation(1);
    tft.setTextSize(2);
    tft.setTextColor(FONT_COLOR);
    tft.fillScreen(BACKGROUND_COLOR);

    tft.drawString("CONSOLE >", tft.height() / 2 + 40, tft.width() / 4 + 50);// tft.width() / 4 - 10);
    tft.drawString("KEYBOARD >", tft.height() / 2 + 35, tft.width() / 4 - 50);
  
}

}