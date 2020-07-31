#include <Arduino.h>

#include "BLEDevice.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"

namespace ble_io {

   bool setup();
   void loop();
   bool get_state();
   bool print_pass(const char*);
}