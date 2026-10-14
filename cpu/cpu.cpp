// cpu.cpp

#include <cstdint>
#include <stdexcept>
#include <random>
#include "cpu.h"
#include "../display/display.h"
#include "../memory/memory.h"

CPU::CPU(Memory& memory, Display& display) : memory(memory), display(display), gen(rd()), distrib(0, 255) {}

CPU::~CPU(){}

void CPU::initialize(){
    //set Programm Counter to start of useable memory.
    PC = 0x200;
    //initialize Index Register by setting to 0
    I = 0;
    //initialize registers by setting them to 0
    for(int i = 0; i<16; ++i){
        V[i] = 0;
    }
    //initialize Stack
    for(int i = 0; i<16; ++i){
        stack[i] = 0;
    }
    SP = 0;
    //set Timers to 0
    delayTimer = 0;
    soundTimer = 0;
}

uint8_t CPU::getRandomNumber(){
    return distrib(gen);
}

void CPU::setKeypad(Keypad* kp){
    keypad = kp;
}

void CPU::cycle(){
    //fetch
    uint16_t opcode = memory.readWord(PC);
    PC += 2;

    //decode
    uint8_t instructionType = (opcode >> 12) & 0x0F;

    //execute
    switch(instructionType){
        case 0x0: {
            if(opcode == 0x00E0){
                //00E0
                //clear display
                display.clear();
            } else if(opcode == 0x00EE){
                //00EE
                //return from subroutine
                if(SP >= 1){
                    SP--;
                    PC = stack[SP];
                } else{
                    throw std::underflow_error("Stack underflow Error");
                }
            }
            break;
        }
        case 0x1: {
            //1NNN
            //Jump to address NNN
            uint16_t address = (opcode & 0x0FFF);
            PC = address;
            break;
        }
        case 0x2: {
            //2NNN
            //call subroutine at NNN
            uint16_t address = (opcode & 0x0FFF);
            stack[SP] = PC;
            SP++;
            PC = address;
            break;
        }
        case 0x3: {
            //3XNN
            //skip next instruction, if Vx == NN
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t nn = (opcode & 0x00FF);
            if(V[x] == nn){
                PC += 2;
            }
            break;
        }
        case 0x4: {
            //4XNN
            //skip next instruction, if Vx != NN
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t nn = (opcode & 0x00FF);
            if(V[x] != nn){
                PC += 2;
            }
            break;
        }
        case 0x5: {
            //5XY0
            //skip next instruction, if Vx == Vy
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            if(V[x] == V[y]){
                PC += 2;
            }
            break;
        }
        case 0x6: {
            //6XNN
            //set Vx to NN
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t nn = (opcode & 0x00FF);
            V[x] = nn;
            break;
        }
        case 0x7: {
            //7XNN
            //add NN to Vx. carry flag is unchanged
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t nn = (opcode & 0x00FF);
            V[x] += nn;
            break;
        }
        case 0x8: {
            //8XY
            uint8_t lastnibble = (opcode & 0x000F);
            switch(lastnibble){
                case 0: {
                    //8XY0
                    //set Vx to the value of Vy
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    V[x] = V[y];
                    break;
                }
                case 1: {
                    //8XY1
                    //set Vx to Vx OR(bitwise) Vy
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    V[x] = (V[x] | V[y]);
                    break;
                }
                case 2: {
                    //8XY2
                    //set Vx to Vx AND(bitwise) Vy
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    V[x] = (V[x] & V[y]);
                    break;
                }
                case 3: {
                    //8XY3
                    //set Vx to Vx XOR(bitwise) Vy
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    V[x] = (V[x] ^ V[y]);
                    break;
                }
                case 4: {
                    //8XY4
                    //add Vy to Vx. VF is set to 1 when there's an overflow otherwise its set to 0
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    uint8_t initialVx = V[x];
                    V[x] = V[x] + V[y];
                    //overflow
                    if(V[x] < initialVx){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    break;
                }
                case 5: {
                    //8XY5
                    //subtract Vy from Vx. VF is set to 0 when there's an underflow and to 1 if not
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    uint8_t initialVx = V[x];
                    V[x] = V[x] - V[y];
                    //underflow
                    if(V[x] > initialVx){
                        V[0xF] = 0;
                    }
                    else{
                        V[0xF] = 1;
                    }
                    break;
                }
                case 6: {
                    //8XY6
                    //shift Vx bitwise to the right. stores least significant bit of Vx into Vf
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    V[0xf] = (V[x] & 1); //store least significant bit into Vf
                    V[x] = (V[x] >> 1);
                    break;
                }
                case 7: {
                    //8XY7
                    //sets Vx to Vy - Vx. Vf = 0 when there is an underflow, else Vf = 1
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t y = (opcode & 0x00F0) >> 4;
                    if(V[x] > V[y]){
                        //underflow
                        V[0xf] = 0;
                    }
                    else{
                        V[0xf] = 1;
                    }
                    V[x] = V[y] - V[x];
                    break;
                }
                case 0xE: {
                    //8XYE
                    //shift Vx to the left, set Vf to 1 if most significant bit was set else 0
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    V[0xf] = (V[x] & 0x80) >> 7;
                    V[x] = (V[x] << 1);
                    break;
                }
                default:
                    throw std::logic_error("No such Instruction found!");
            }
            break;
        }
        case 0x9: {
            //9XY0
            //skip next instruction if Vx != Vy
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            if(V[x] != V[y]){
                PC += 2;
            }
            break;
        }
        case 0xA: {
            //ANNN
            //set I to NNN
            //NNNs address or the memory at that address???
            uint16_t nnn = (opcode & 0x0FFF);
            I = nnn;
            break;
        }
        case 0xB: {
            //BNNN
            //Jump to address NNN + V0
            uint16_t nnn = (opcode & 0x0FFF);
            PC = nnn + V[0];
            break;
        }
        case 0xC: {
            //CXNN
            //set Vx to a bitwise and with a random number and nn
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t nn = (opcode & 0x00FF);
            V[x] =  (nn & getRandomNumber());
            break;
        }
        case 0xD: {
            //DXYN
            //Draw sprite at coordinates V[x] V[y]. has width of 8 pixels and height of N
            //each row of 8 pixels is read starting from I.
            //Vf is set to 1 if any pixels are flipped from set to unset and to 0 if none are
            uint8_t xindex = (opcode & 0x0F00) >> 8;
            uint8_t yindex = (opcode & 0x00F0) >> 4;
            uint8_t x = V[xindex] % DISPLAY_WIDTH; // Wrap coordinates
            uint8_t y = V[yindex] % DISPLAY_HEIGHT;
            uint8_t n = opcode & 0x000F;

            bool collision = false; // Reset collision flag for each sprite

            for (int i = 0; i < n; ++i) {
                uint8_t currentByte = memory.readByte(I + i);
                for (int j = 7; j >= 0; --j) {
                    // Check if the bit is set in the current byte
                    if ((currentByte & (1 << j)) != 0) {
                        // If the bit is set, draw the pixel and check for collision
                        if (display.setPixel(x + (7 - j), y + i)) {
                            collision = true;
                        }
                    }
                }
            }
            V[0xF] = collision;
            break;
        }
        case 0xE: {
            //E
            uint8_t lastByte = (opcode & 0x00FF);
            if(lastByte == 0x9E){
                //EX9E
                //skip next instruction if Vx == key pressed
                uint8_t x = (opcode & 0x0F00) >> 8;
                if(keypad->isPressed(V[x])){
                    PC += 2;
                }
            }
            else if(lastByte == 0xA1){
                //EXA1
                //skip next instruction if Vx != key pressed
                uint8_t x = (opcode & 0x0F00) >> 8;
                if(!keypad->isPressed(V[x])){
                    PC += 2;
                }
            }
            break;
        }
        case 0xF: {
            uint8_t lastByte = (opcode & 0x00FF);
            switch (lastByte) {
                case 0x07: {
                    //FX07
                    //set Vx to value of delay timer
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    V[x] = delayTimer;
                    break;
                }
                case 0x0A: {
                    //FX0A
                    //await keypress, then store in Vx
                    uint8_t x = (opcode & 0x0F00) >> 8;

                    // Check if any key is currently pressed
                    bool keyPressed = false;
                    uint8_t pressedKey = 0;
                    for (int i = 0; i < 16; ++i) {
                        if (keypad->isPressed(i)) {
                            keyPressed = true;
                            pressedKey = i;
                            break; // Found a pressed key
                        }
                    }

                    if (keyPressed) {
                        V[x] = pressedKey; // Store the pressed key in Vx
                        break; // Continue to the next instruction
                    } else {
                        // No key pressed, so *don't* advance PC.  Re-execute this instruction.
                        PC -= 2; // Go back to this instruction.
                        return; // Return from cycle, will be called again
                    }
                }
                case 0x15: {
                    //FX15
                    //set delay timer to Vx
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    delayTimer = V[x];
                    break;
                }
                case 0x18: {
                    //FX18
                    //set sound timer to Vx
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    soundTimer = V[x];
                    break;
                }
                case 0x1E: {
                    //FX1E
                    //add Vx to I
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint16_t initialI = I;
                    I += V[x];

                    // Check for overflow
                    if (I < initialI) {  // Overflow occurred
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    break;
                }
                case 0x29: {
                    //FX29
                    //set I to location of character in Vx
                    //to font (starts at 0x050) + character * 5
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t lowestNibble = (V[x] & 0x0F);
                    I = 0x050 + lowestNibble * 5;
                    break;
                }
                case 0x33: {
                    //FX33
                    //store binary representation of Vx. hundreds at I, tens at I+1, ones at I+2
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    uint8_t value = V[x];
                    memory.writeByte(I, value/100);
                    memory.writeByte(I + 1, (value/10)%10);
                    memory.writeByte(I + 2, value % 10);
                    break;
                }
                case 0x55: {
                    //FX55
                    //store from V0 to Vx in memory, starting at address I.
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    for (int i = 0; i <= x; i++) {
                        memory.writeByte(I + i, V[i]);
                    }
                    break;
                }
                case 0x65: {
                    //FX65
                    //fill V0 to Vx with values from memory. start from I.
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    for (int i = 0; i <= x; i++) {
                        V[i] = memory.readByte(I + i);
                    }
                    break;
                }
                default:
                    throw std::logic_error("No such Instruction found!");
            }
            break;
        }
        default:
            throw std::logic_error("No such Instruction found!");
    }

}

void CPU::decrementTimers() {
    if (delayTimer > 0) {
        delayTimer--;
    }
    if (soundTimer > 0) {
        soundTimer--;
        // TODO: Implement sound here (e.g., play a beep if soundTimer just became 1)
    }
}