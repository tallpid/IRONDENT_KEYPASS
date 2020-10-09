namespace pass_db {

    #define KEYS_LIST "knames"
    #define DB_NAME "keydb"
    #define PASS_COUNT "kcount"

    bool setup();

    bool add_password(const char*, const char*);
    bool get_password(const char* name, char* password);
    bool remove_password(const char*);
    bool wipe_passwords();

    char** get_list();
    unsigned int get_amount();
}


#ifndef GMODE
enum MODE {
    UNINITIALIZED = 0,
    KEYBOARD,
    CONSOLE
};
#define GMODE
#endif