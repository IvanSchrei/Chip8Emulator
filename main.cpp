#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "cpu/cpu.h"
#include "display/display.h"
#include "memory/memory.h"
#include "keypad/keypad.h"
#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: chip8 <ROM_file>" << std::endl;
        return 1;
    }

    std::string romFilename = argv[1];

    Memory memory;
    Display display;
    Keypad keypad;
    CPU cpu(memory, display);

    //initialize Keypad
    cpu.setKeypad(&keypad);
    cpu.initialize();

    //load ROM
    try {
        memory.loadROM(romFilename);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error loading ROM: " << e.what() << std::endl;
        return 1;
    }

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // Handle SDL events (key presses, window close)
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            // Key press events
            else if (e.type == SDL_EVENT_KEY_DOWN) {
                SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)e;
                SDL_Scancode scancode = keyEvent.scancode;
                switch (scancode) {
                    case SDL_SCANCODE_1: keypad.setKey(0x1, true); break;
                    case SDL_SCANCODE_2: keypad.setKey(0x2, true); break;
                    case SDL_SCANCODE_3: keypad.setKey(0x3, true); break;
                    case SDL_SCANCODE_4: keypad.setKey(0xC, true); break;
                    case SDL_SCANCODE_Q: keypad.setKey(0x4, true); break;
                    case SDL_SCANCODE_W: keypad.setKey(0x5, true); break;
                    case SDL_SCANCODE_E: keypad.setKey(0x6, true); break;
                    case SDL_SCANCODE_R: keypad.setKey(0xD, true); break;
                    case SDL_SCANCODE_A: keypad.setKey(0x7, true); break;
                    case SDL_SCANCODE_S: keypad.setKey(0x8, true); break;
                    case SDL_SCANCODE_D: keypad.setKey(0x9, true); break;
                    case SDL_SCANCODE_F: keypad.setKey(0xE, true); break;
                    case SDL_SCANCODE_Y: keypad.setKey(0xA, true); break;
                    case SDL_SCANCODE_X: keypad.setKey(0x0, true); break;
                    case SDL_SCANCODE_C: keypad.setKey(0xB, true); break;
                    case SDL_SCANCODE_V: keypad.setKey(0xF, true); break;

                }
            }
            // Key release events
            else if (e.type == SDL_EVENT_KEY_UP) {
                SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)e;
                SDL_Scancode scancode = keyEvent.scancode;
                switch (scancode) {
                    case SDL_SCANCODE_1: keypad.setKey(0x1, false); break;
                    case SDL_SCANCODE_2: keypad.setKey(0x2, false); break;
                    case SDL_SCANCODE_3: keypad.setKey(0x3, false); break;
                    case SDL_SCANCODE_4: keypad.setKey(0xC, false); break;
                    case SDL_SCANCODE_Q: keypad.setKey(0x4, false); break;
                    case SDL_SCANCODE_W: keypad.setKey(0x5, false); break;
                    case SDL_SCANCODE_E: keypad.setKey(0x6, false); break;
                    case SDL_SCANCODE_R: keypad.setKey(0xD, false); break;
                    case SDL_SCANCODE_A: keypad.setKey(0x7, false); break;
                    case SDL_SCANCODE_S: keypad.setKey(0x8, false); break;
                    case SDL_SCANCODE_D: keypad.setKey(0x9, false); break;
                    case SDL_SCANCODE_F: keypad.setKey(0xE, false); break;
                    case SDL_SCANCODE_Y: keypad.setKey(0xA, false); break;
                    case SDL_SCANCODE_X: keypad.setKey(0x0, false); break;
                    case SDL_SCANCODE_C: keypad.setKey(0xB, false); break;
                    case SDL_SCANCODE_V: keypad.setKey(0xF, false); break;
                }
            }
        }

        try {
            cpu.cycle();
        } catch (const std::exception& ex) {
            std::cerr << "CPU Exception: " << ex.what() << std::endl;
            break;
        }

        cpu.decrementTimers();

        display.render();

        //Control Speed of Emulation
        std::this_thread::sleep_for(std::chrono::microseconds(400));

    }

    return 0;
}