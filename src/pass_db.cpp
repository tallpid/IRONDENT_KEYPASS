#include <Preferences.h>
#include "pass_db.h"

namespace pass_db {

Preferences preferences;

unsigned int amount_of_passwords = 0;
char** list_of_password_names = nullptr;

unsigned int get_amount() {
    return amount_of_passwords;
}

char** get_list() {
    return list_of_password_names;
}

bool init_password_list() {
    String concated_pass_names = "";
    try {
        concated_pass_names = preferences.getString(KEYS_LIST);
        if (concated_pass_names.length() == 0) throw "[-] Length of the key list == 0";
    }
    catch (...) {
        Serial.println("Was not able to get the list of passwords");
        amount_of_passwords = 0;
        return false;
    }

   // Serial.println("[D] Full password list: " + concated_pass_names);

    list_of_password_names = (char **)malloc(sizeof(char*) * amount_of_passwords);

    // int current_pos = 0;
    int parsed_count = 0;    

    for (int i = 0, current_pos = 0; i < amount_of_passwords; i++, parsed_count++) {
        int separator = concated_pass_names.indexOf(';', current_pos);
        if (separator == -1) return false;

       // Serial.println("[D] Separator position: ");
        Serial.println(separator);

        //if (separator + 1 == concated_pass_names.length()) break;

        list_of_password_names[i] = (char*)malloc(MAX_NAME_LENGTH);
        concated_pass_names.toCharArray(list_of_password_names[i], separator - current_pos + 1, current_pos);
        
       // Serial.println("[D] Name of the key:");
        Serial.println(list_of_password_names[i]);
        
        current_pos = separator + 1;
    }

    return parsed_count == amount_of_passwords; 
}

bool setup() {
    preferences.begin(DB_NAME, false);
    amount_of_passwords = preferences.getUInt(PASS_COUNT, 0);

    Serial.print("[~] Amount of passwords in DB: ");
    Serial.print(amount_of_passwords);

    // wipe_passwords();

    if (amount_of_passwords == 0) {
        return true;
    }

    return init_password_list();
}

bool update_password_list() {
    String newList = "";

    for (int i = 0; i < amount_of_passwords; i++) {        
        newList += list_of_password_names[i];
        newList += ";";
    }

    try {
        preferences.remove(KEYS_LIST);
    } catch (...) {
       // Serial.println("[D] Key list is empty");
    }

   // Serial.println("[D] New password list: ");
    Serial.println(newList);
    
    preferences.putString(KEYS_LIST, newList);
    
    Serial.println("[~] The password list was updated");
    return true;
}

bool add_password(const char* name, const char* pass) {
    if (name == nullptr || pass == nullptr) return false;

   // Serial.println("[D] New password name: ");
    Serial.println(name);
   // Serial.println("[D] New password: ");
    Serial.println(pass);

    amount_of_passwords += 1;
    if (amount_of_passwords >= MAX_AMOUNT_OF_PASS) { 
        Serial.println("[~] Too many passwords");
        return false;
    }

    char** new_list_of_passwords = (char **)malloc(sizeof(char*) * amount_of_passwords);
    for(int i = 0; i < amount_of_passwords - 1; i++) {
        new_list_of_passwords[i] = list_of_password_names[i];        
    }

    new_list_of_passwords[amount_of_passwords - 1] = (char*)malloc(MAX_NAME_LENGTH);
    strcpy(new_list_of_passwords[amount_of_passwords - 1], name);
    
    free(list_of_password_names);
    list_of_password_names = new_list_of_passwords;       

    return (preferences.putString(name, pass) && preferences.putUInt(PASS_COUNT, amount_of_passwords) && update_password_list());
}

bool remove_password(const char* name) {
   // Serial.println("[D] Received RM command for the key: ");
    Serial.println(name);

    if (amount_of_passwords == 0) {
        Serial.println("[~] Password DB is empty");
        return false;
    }

    int password_index = -1;
    for (int i = 0; i < amount_of_passwords && password_index == -1; i++) {
        if (strcmp(name, list_of_password_names[i]) == 0) {
           // Serial.println("[D] Key index ");
            Serial.println(i);
            
            password_index = i;
        }
    }

    if (password_index == -1) {
        Serial.println("[-] Was not able to find the password in the list");
        return false;
    }

    String check = "";

    try {
        check = preferences.getString(list_of_password_names[password_index]);
    }
    catch (...) {
        Serial.print("[-] Was not able to find the password: ");
        Serial.println(name);
    }

    if (check.length() == 0) {
        Serial.print("[-] Was not able to find the password: ");
        Serial.println(name);
        return false;
    }

    preferences.remove(name);

   // Serial.println("[D] Password ");
    Serial.print(name);
    Serial.println(" was removed");

    free(list_of_password_names[password_index]);
    list_of_password_names[password_index] = nullptr;

   // Serial.println("[D] Password name buffer was removed");

    if (password_index == 0) {
        list_of_password_names = &list_of_password_names[1];
    }

    if (password_index > 0 && password_index < amount_of_passwords - 1) {
        list_of_password_names[password_index] = list_of_password_names[amount_of_passwords - 1];
        list_of_password_names[amount_of_passwords - 1] = nullptr;
    }

    amount_of_passwords -= 1;    
    return (update_password_list() && preferences.putUInt(PASS_COUNT, amount_of_passwords));
}

bool wipe_passwords() {

    if (list_of_password_names != nullptr) {
        for(int i = 0; i < amount_of_passwords; i++) {
            if (list_of_password_names[i] == nullptr) continue;

            preferences.remove(list_of_password_names[i]);
            free(list_of_password_names[i]);            
        }
    }

    amount_of_passwords = 0;
    
    preferences.clear();
    preferences.putUInt(PASS_COUNT, amount_of_passwords);
    preferences.putString(KEYS_LIST, "");

    Serial.println("[~] Password database was wiped");

    return true;
}

bool get_password(const char* name, char* password) {
    String tmp_password = "";

    try {
        tmp_password = preferences.getString(name);
    } catch (...) {
        Serial.print("[-] Was not able to get password ");
        Serial.println(name);
    }

    if (tmp_password.length() == 0 || password == nullptr) return false;

    tmp_password.toCharArray(password, MAX_PASS_LENGTH);
    return true;
}

}