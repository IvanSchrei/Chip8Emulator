#include <cstdint>
#include "keypad.h"

Keypad::Keypad() {}
Keypad::~Keypad() {}

bool Keypad::isPressed(uint8_t key) const{
    return keys[key];
}

void Keypad::setKey(uint8_t key, bool pressed){
    keys[key] = pressed;
}