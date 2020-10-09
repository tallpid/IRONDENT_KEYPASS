#define US_KEYBOARD 1

#include <Arduino.h>
#include <M5StickC.h>

#include "pass_db.h"
#include "ble_console.h"
#include "ble_keyboard.h"
#include "tft_io.h"

MODE STATE = UNINITIALIZED;

bool KS_INITIALIZED = false;

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{   
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

int password_index = -1;

void button_A_callback () {
    switch (STATE)
        {
        case KEYBOARD:
            if (password_index != -1) {
                password_index = (password_index + 1) % pass_db::get_amount();
                //tft_io::clean_screen();
                                
                tft_io::print_tft_text(pass_db::get_list()[password_index]);
                // tft_io::show_switch(ble_io::get_state());
            }

            break;
        case UNINITIALIZED:           
            STATE = KEYBOARD;
            tft_io::update_menu_status(STATE);
            
            if (!ble_io::setup()) {
                tft_io::print_tft_text("IO FAILED");
                STATE = UNINITIALIZED;
                break;
            }
                            
            if (pass_db::get_amount() <= 0) {
                tft_io::print_tft_text("DB is empty");
                espDelay(10000);
                break;
            }    

            password_index = 0;                   
            
            //tft_io::clean_screen();  
            tft_io::show_switch(ble_io::get_state());
            tft_io::print_tft_text(pass_db::get_list()[password_index]);               
            break;
        }

}

void button_B_callback () {

    switch (STATE) {
        case KEYBOARD:
            if (password_index != -1) {
                char pass[MAX_PASS_LENGTH] = {0};

                if (!pass_db::get_password(pass_db::get_list()[password_index], &pass[0])) {
                    tft_io::print_tft_text("Not able to get the password");
                    return;
                }
                ble_io::print_pass(pass);
            }
            break;

        case UNINITIALIZED:
            tft_io::print_tft_text("Switched to console mode");
            STATE = CONSOLE;
            tft_io::update_menu_status(STATE);
            //tft_io::clean_screen();

            if (!ble_cli::setup()) {
                tft_io::print_tft_text("CLI initialization failed");
                //Serial.println(" CLI FAILED");
                STATE = UNINITIALIZED;                
                return;
            }
            break;
        default:
            tft_io::print_tft_text("STATE IS UNDEFINED");
    }
}


void BUTTON_Aoop() {
    M5.update();
    tft_io::update_menu_battery(); 
    
    if (M5.BtnA.isPressed()) {
        button_A_callback();
        //espDelay(500);        
    }

    if (M5.BtnB.isPressed()) {
        button_B_callback();
        //espDelay(500);
    }

    //espDelay(100);
}

void setup() {
    M5.begin();

    tft_io::start_tft();
    espDelay(1000);

    if(!pass_db::setup()) {
        tft_io::print_tft_text("Failed to init pass db");
        return;
    }
    
    KS_INITIALIZED = true;    
    tft_io::show_help();
}

void loop() {
  
    if (!KS_INITIALIZED) {
        tft_io::print_tft_text("Can't initialized keystore");
        espDelay(2000);
        return;
    }
    
    BUTTON_Aoop();

    switch (STATE) {
        case CONSOLE: ble_cli::loop(); break;
        case KEYBOARD: break;
        default: espDelay(100); 
    }
}