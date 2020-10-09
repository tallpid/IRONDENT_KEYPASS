#include <M5StickC.h>
#include "pass_db.h"

namespace tft_io {
    void start_tft(void);
    void clean_screen(void);
    void print_tft_text(const char*);
    void show_switch(bool is_connected);
    void show_help();

    void update_menu_ble_status(bool status);
    void update_menu_status(MODE);
    void update_menu_message(const char*);
    void update_menu_battery();
}