#include "tft_io.h"

namespace tft_io {

#define screen M5.Lcd
#define BACKGROUND_COLOR BLACK
#define FONT_COLOR ORANGE
#define LINE_COLOR ORANGE

#define MARGX 6
#define MARGY 6
#define BORDER_SIZE 2

#define MAXX screen.width() - MARGY
#define MAXY screen.height() - MARGX

#define TEXT_SIZE 1
#define TEXT_HEIGHT 14

void show_logo();
void print_tft_text(const char* output);

typedef struct t_menu {
    bool ble_connected = false;
    MODE status = UNINITIALIZED;    
    unsigned int keys;
    int battery = 0;

    const int message_lines = 4;
    const int message_length = 60;
    int message_current_line = 0;
    //char message[message_lines * message_length];

    bool lock = false;
    bool force_update = false;
} menu;

menu M;

bool get_lock() {
    while(M.lock) {
        delay(50);
    }

    M.lock = true;
}

bool release_lock() {
    if (M.lock) M.lock = false;
}

void update_menu_status(MODE status) {    

    if (status == M.status && !M.force_update) return;    
    
    get_lock();

    char c_status[20] = {0};

    switch (status)
    {
    case UNINITIALIZED:
        screen.setTextColor(YELLOW);
        memcpy(c_status, "UNDEFINE", strlen("UNDEFINE"));
        break;
    case KEYBOARD:
        screen.setTextColor(GREEN);
        memcpy(c_status, "KEYBOARD", strlen("KEYBOARD"));
        break;
    case CONSOLE:
        screen.setTextColor(BLUE);
        memcpy(c_status, "BCONSOLE", strlen("BCONSOLE"));
        break;
    }
        
    screen.setCursor(MARGX, MARGY);
    screen.fillRect(MARGX, MARGY, 52, TEXT_HEIGHT, BACKGROUND_COLOR);    
    screen.print(c_status);
    screen.setTextColor(FONT_COLOR);

    release_lock();
}

void update_menu_ble_status(bool status) {
    if (status == M.ble_connected && !M.force_update) {
        return;
    }

    get_lock();

    M.ble_connected = status;

    screen.setCursor(MARGX + 60, MARGY);   
    screen.fillRect(MARGX + 60, MARGY, 30, TEXT_HEIGHT, BACKGROUND_COLOR); 
    screen.print("BLE:");
    status ? screen.setTextColor(GREEN) : screen.setTextColor(RED);    
    status ? screen.print("O") : screen.print("X");
    screen.setTextColor(FONT_COLOR);

    release_lock();
}

void update_menu_message(const char* message) {    

    if (message == NULL || *message == '\0' || strlen(message) == 0) return;        

    get_lock();

    for (; M.message_current_line < M.message_lines; M.message_current_line++) {
        screen.setCursor(
                     MARGX + BORDER_SIZE, 
                     MARGX + (TEXT_HEIGHT * (M.message_current_line + 1)) + BORDER_SIZE + 3);

        for (int pos = 0; (pos < M.message_length); pos++) {
            if (*message == '\0') {
                M.message_current_line++;                
                release_lock();
                return;
            }
            
            char data = *message++; delay(20);
            screen.print(data);
        }
    }

    release_lock();

    if (*message) {
        screen.setCursor(MARGX + BORDER_SIZE, MARGX + TEXT_HEIGHT + BORDER_SIZE);
        screen.fillRect (MARGX + BORDER_SIZE, MARGX + TEXT_HEIGHT + BORDER_SIZE,
                        screen.width() - MARGX - BORDER_SIZE - 4, 
                        TEXT_HEIGHT * M.message_lines + 1, BACKGROUND_COLOR);
        
        M.message_current_line = 0;        
        update_menu_message(message);
    }
}

void update_menu_borders() {
    get_lock();

    for(int x = 0; x < BORDER_SIZE; x++) {
        screen.drawLine(x, 0, x, screen.height(), LINE_COLOR);
        screen.drawLine(screen.width() - x, 0, screen.width() - x, screen.height(), LINE_COLOR);
    }

    for(int y = 0; y < BORDER_SIZE; y++) {
        screen.drawLine(0, y, screen.width(), y, LINE_COLOR);
        screen.drawLine(0, screen.height() - y, screen.width(), screen.height()- y, LINE_COLOR);
    }    

    screen.drawLine(0, MARGX + TEXT_HEIGHT, screen.width(), MARGX + TEXT_HEIGHT, LINE_COLOR);
    screen.drawLine(60, 0, 60, MARGX + TEXT_HEIGHT, LINE_COLOR);
    screen.drawLine(100, 0, 100, MARGX + TEXT_HEIGHT, LINE_COLOR);

    release_lock();
}

int get_batt_lvl()
{
  double vbat = M5.Axp.GetVbatData() * 1.1;
  const double minimum = 3000.0;
  const double maximum = 3900.0; //4100
  const double measure = 4.0;
  const double padding = (maximum - minimum) / measure;

  if (vbat > 4000.0) return 100;

  for(int i = 0; i < measure; i += 1) {
      double lb = minimum + (padding * i);
      double rb = lb + padding;
      if ( lb <= vbat && vbat <= rb  ) 
         return (i + 1) * (100 / measure);
  }

  return 100;
}

void update_menu_battery() {    

    int btr_lvl = get_batt_lvl();
    if (M.battery == btr_lvl && !M.force_update) {
        return;
    }

    get_lock();

    M.battery = btr_lvl;


    screen.setCursor(MARGX + 100, MARGY);
    screen.fillRect(MARGX + 100, MARGY, 60, TEXT_HEIGHT, BACKGROUND_COLOR);    
    screen.print("BTR:");
    
    if (btr_lvl <= 25) screen.setTextColor(RED);
    else if (btr_lvl <= 50) screen.setTextColor(YELLOW);
    else screen.setTextColor(GREEN);

    screen.print(btr_lvl);
    screen.print("%");

    screen.setTextColor(FONT_COLOR);

    release_lock();
}

void menu_loop(bool force=false) {
    M.force_update = force;

    update_menu_ble_status(M.ble_connected);
    update_menu_status(M.status);
    update_menu_message(NULL);
    update_menu_battery();
    update_menu_borders();

    M.force_update = false;
}

void start_tft(void) {

    screen.setRotation(1);
    //screen.setTextDatum(TL_DATUM);
    screen.setTextSize(1);
    screen.setTextColor(FONT_COLOR, BACKGROUND_COLOR);
    
    menu_loop(true);

    print_tft_text("IRONDENT KEYPASS");
}

void clean_screen(){
    return;
}

void print_tft_text(const char* output) {
    // char* buffer = (char*)malloc(M.message_length * M.message_lines);
    // stpcpy(buffer, output);

    Serial.println(output);
    update_menu_message(output);    

    //free(buffer);
}

void show_switch(bool is_connected) {
    tft_io::print_tft_text("A Button --> SWITCH");
    tft_io::print_tft_text("B Button --> CONFIRM");
}

void show_help() {
    tft_io::print_tft_text("A Button --> KEYBOARD");
    tft_io::print_tft_text("B Button --> CONSOLE");
}

}