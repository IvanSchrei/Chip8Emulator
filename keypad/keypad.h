#ifndef KEYPAD_H
#define KEYPAD_H

#include <cstdint>

class Keypad{
    public:
        Keypad();
        ~Keypad();

        bool isPressed(uint8_t key) const;
        void setKey(uint8_t key, bool pressed);
    private:
        bool keys[16] = {false};

};

#endif