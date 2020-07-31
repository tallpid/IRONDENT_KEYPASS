#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include "esp_adc_cal.h"
#include "bmp.h"

namespace tft_io {

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4  // Display backlight control pin

void start_tft(void);
void clean_screen(void);
void print_tft_text(const char*);
void show_switch(bool is_connected);
void show_help();

}