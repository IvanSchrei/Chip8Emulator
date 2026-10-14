#ifndef CPU_H
#define CPU_H

#include <cstdint> // For uint8_t, uint16_t
#include <random>
#include "../memory/memory.h" // Include Memory class definition (important!)
#include "../display/display.h" // Include Display class definition (important!)
#include "../keypad/keypad.h"

class CPU {
public:
    // Constructor:  Takes references to Memory and Display objects.
    // The CPU needs access to memory to fetch instructions and data,
    // and access to the display to draw graphics.
    CPU(Memory& memory, Display& display);

    // Destructor: If there is dynamic allocation, you'll need a deconstructor
    ~CPU();

    // initialize(): Resets the CPU to a known initial state.  Sets the
    // program counter (PC) to the start of program memory (0x200),
    // initializes registers, the stack pointer, and other internal state.
    void initialize();

    // cycle(): Executes a single CPU cycle.  This is the heart of the emulator.
    // It should fetch an opcode, decode it, and execute the corresponding
    // instruction.  It may also need to update timers.
    void cycle();

    void setKeypad(Keypad* kp);

    void decrementTimers();

private:
    // Memory:  A reference to the Memory object.
    Memory& memory;

    // Display: A reference to the Display object.
    Display& display;

    // Registers:
    uint16_t PC;     // Program Counter (points to the current instruction)
    uint16_t I;      // Index Register (used for addressing memory)
    uint8_t V[16];    // General-purpose registers (V0 - VF)  VF is often used as a carry flag.
    uint16_t stack[16]; // Stack (used for subroutine calls)
    uint8_t SP;      // Stack Pointer (points to the top of the stack)

    uint8_t delayTimer; // Delay timer (decremented at 60 Hz)
    uint8_t soundTimer; // Sound timer (decremented at 60 Hz)

    std::random_device rd;  // Source of entropy
    std::mt19937 gen; // Mersenne Twister engine
    std::uniform_int_distribution<> distrib;
    uint8_t getRandomNumber();

    Keypad* keypad; //keypad for managing input
};

#endif