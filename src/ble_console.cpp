#include <BluetoothSerial.h>
#include "pass_db.h"
#include "tft_io.h"

namespace ble_cli {

BluetoothSerial ESP_BT;

bool BLE_CLI_INITIALIZED = false;
bool BLE_CLI_UNLOCKED = false;

enum BLE_CLI_COMMANDS {
    UNINITIALIZED = -1,
    ADD = 0,
    RM = 1,
    WIPE = 2
};


// TODO: wipe it and move to the build config

#define MAX_PIN_LENGTH 10
char CONSOLE_PIN[MAX_PIN_LENGTH] = {0};

/*
#define MAX_NAME_LENGTH 40
#define MAX_PASS_LENGTH 40

#define MIN_PASS_LENGTH 6
#define MIN_NAME_LENGTH 3
*/

void initialize_cli_pin() {
    long randNumber = random(1000, 1000000);
    itoa(randNumber, CONSOLE_PIN, 10);
}

bool setup(void) {
    if( (BLE_CLI_INITIALIZED = ESP_BT.begin("IRONDENT_KS"))) {
        Serial.println("[+] BLE console was enabled");
    } else {
        Serial.println("[-] BLE console initialization was failed");
    }

    initialize_cli_pin();
    tft_io::print_tft_text(CONSOLE_PIN);
    
    return BLE_CLI_INITIALIZED;
}

bool check_password(String incoming) {
    return BLE_CLI_UNLOCKED = incoming.indexOf(CONSOLE_PIN) == 0;
}

void print_help(void) {
    ESP_BT.println("[~] Add new: add <name_of_key> <password>");
    ESP_BT.println("[~] Remove: rm <name_of_key>");
    ESP_BT.println("[~] Delete all: wipe");
}

bool add_password(String command);
bool rm_password(String command);
bool wipe();

bool command_handler(String incoming) {
    const unsigned int commands_count = 3;

    incoming.replace("\n", "");
   // Serial.println("[D] Incoming command: " + incoming);

    const char * commands_array [] = {
        "add",
        "rm",
        "wipe"
    };

    bool command_was_parsed = false;
    BLE_CLI_COMMANDS command_index = UNINITIALIZED;

    for (int i = 0; i < commands_count && command_index == -1; i++) {
       // Serial.println("[D] Compare with command:");
        Serial.println(commands_array[i]);

        if(incoming.indexOf(commands_array[i]) == 0) {
            command_index = (BLE_CLI_COMMANDS) i;
        }
    } 

    if (command_index == UNINITIALIZED) return command_was_parsed;

    switch (command_index)
    {
    case ADD:
        command_was_parsed = add_password(incoming);
        if (!command_was_parsed) {
            Serial.println("[-] Was not able to add the password");
        }
        break;
    case RM:
        command_was_parsed = rm_password(incoming);
        if (!command_was_parsed) {
            Serial.println("[-] Was not able to delete the password");
        }
        break;
    case WIPE:
        command_was_parsed = wipe();
        if (!command_was_parsed) {
            Serial.println("[-] Was not able to wipe DB");
        }
        break;
    default:
        Serial.println("[-] Command was not found");
    }

    return command_was_parsed;
}

void loop(void) {
    
    if (ESP_BT.available() && BLE_CLI_INITIALIZED) //Check if we receive anything from Bluetooth
    {
        String incoming = ESP_BT.readString(); //Read what we recevive
        if (!BLE_CLI_UNLOCKED) {                        
            if(!check_password(incoming)) {
                Serial.println("[-] Incorrect password: " + incoming);              
                // To stop bruteforce
                delay(10000);
            } else {
                ESP_BT.println("[+] BLE Console was unlocked");
                print_help();
            }

            return;
        }

        if (!command_handler(incoming)) {
            Serial.println("[-] Incorrect command: " + incoming);
        }
    }
}

bool add_password(String command) {
    bool status = false;
    
    String args = command.substring(strlen("add "));
    if (args.length() == 0) return status;

   // Serial.println("[D] ADD command : " + command);
   // Serial.println("[D] ADD args : " + args);

    int separator = args.indexOf(" ");
    if (separator <= 0) return status;

    /*
   // Serial.println("[D] Separator position: ");
    Serial.println(separator);
    */
    
    char name[MAX_NAME_LENGTH] = {0};
    char pass[MAX_PASS_LENGTH] = {0};

    if (args.length() >= MAX_PASS_LENGTH + MAX_NAME_LENGTH) return status;

    args.toCharArray(name, separator + 1, 0);
    args.toCharArray(pass, args.length() - separator - 1, separator + 1);

    /*
    
    Serial.println(name);
    Serial.println(strlen(name));

    Serial.println(pass);
    Serial.println(strlen(pass));
    
    */

    if (strlen(name) < MIN_NAME_LENGTH || strlen(pass) < MIN_PASS_LENGTH) return status;

    return pass_db::add_password(name, pass);
}

bool rm_password(String command) {
    char name[MAX_NAME_LENGTH] = {0};
    if(command.length() == 0) {
        Serial.println("[-] Cant remove the password with empty name");
        return false;
    }

    command.toCharArray(name, command.length() - strlen("rm "), strlen("rm "));
    return pass_db::remove_password(name);
}

bool wipe() {    
    return pass_db::wipe_passwords();
}

}