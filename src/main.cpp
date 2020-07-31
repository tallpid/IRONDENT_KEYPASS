#define US_KEYBOARD 1

#include <Arduino.h>

#include "pass_db.h"
#include "ble_console.h"
#include "ble_keyboard.h"
#include "tft_io.h"

#include <Button2.h>

#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_L        0
#define BUTTON_R        35

Button2 btn_l(BUTTON_L);
Button2 btn_r(BUTTON_R);

char buff[512];
int vref = 1100;
int btnCick = false;

enum MODE {
    UNINITIALIZED = 0,
    KEYBOARD,
    CONSOLE
};

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

void button_init()
{
    btn_l.setLongClickHandler([](Button2 & b) {
        btnCick = false;
        
        /*
        int r = digitalRead(TFT_BL);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Press again to wake up",  tft.width() / 2, tft.height() / 2 );
        
        espDelay(6000);
        digitalWrite(TFT_BL, !r);

        tft.writecommand(TFT_DISPOFF);
        tft.writecommand(TFT_SLPIN);
        
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_deep_sleep_start();
        */
    });

    btn_r.setPressedHandler([](Button2 & b) {
       // Serial.println("[D] Pressed R");

        switch (STATE)
        {
        case KEYBOARD:
            if (password_index != -1) {
                password_index = (password_index + 1) % pass_db::get_amount();
                tft_io::clean_screen();
                tft_io::print_tft_text(pass_db::get_list()[password_index]);
                tft_io::show_switch(ble_io::get_state());
            }

            break;
        case UNINITIALIZED:
            Serial.println("[+] Switched to keyboard mode");
            
            STATE = KEYBOARD;
            
            if (!ble_io::setup()) {
                tft_io::print_tft_text("IO FAILED");
                Serial.println("IO FAILED");
                STATE = UNINITIALIZED;
                break;
            }
                            
            if (pass_db::get_amount() <= 0) {
                tft_io::print_tft_text("NO SALT");
                espDelay(10000);
                break;
            }    

            password_index = 0;
            tft_io::clean_screen();           
            
            tft_io::print_tft_text(pass_db::get_list()[password_index]);
            tft_io::show_switch(ble_io::get_state());
            
            break;
                
        default:
            Serial.println("[-] STATE IS UNDEFINED");
        }

    });

    btn_l.setPressedHandler([](Button2 & b) {
       // Serial.println("[D] Pressed L");

        switch (STATE) {
            case KEYBOARD:
                if (password_index != -1) {
                    char pass[MAX_PASS_LENGTH] = {0};
                    if (!pass_db::get_password(pass_db::get_list()[password_index], &pass[0])) {
                        Serial.println("[-] Not able to get the password");
                        return;
                    }
                    ble_io::print_pass(pass);
                }
                break;

            case UNINITIALIZED:
                Serial.println("[+] Switched to console mode");
                STATE = CONSOLE;
                tft_io::clean_screen();

                if (!ble_cli::setup()) {
                    tft_io::print_tft_text("CLI FAILED");
                    Serial.println("[-] CLI FAILED");
                    STATE = UNINITIALIZED;                
                    return;
                }
                break;
            default:
                Serial.println("[-] STATE IS UNDEFINED");
            }
        });
}

void setup() {
    Serial.begin(115200);

    tft_io::start_tft();
    espDelay(5000);
    button_init();

    if(!pass_db::setup()) {
        Serial.println("[-] Was not able to init pass db");
        return;
    }

    tft_io::clean_screen();
    Serial.println("[+] System initialized");
    KS_INITIALIZED = true;
    tft_io::show_help();
}

void button_loop() {
    btn_l.loop();
    btn_r.loop();
}

void loop() {
  
    if (!KS_INITIALIZED) {
        tft_io::print_tft_text("NOT INITIALIZED");
        espDelay(2000);
        return;
    }
    
    button_loop();

    switch (STATE) {
        case CONSOLE:
            ble_cli::loop();
            break;
        case KEYBOARD:   
            
            break;
        default: // UNINITIALIZED        
            espDelay(100);
            break;
    }
}