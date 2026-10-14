#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include <cstdint>
#include <array>

class Memory{
    public:
        Memory();   //constructor
        ~Memory();  //deconstructor

        void loadROM(const std::string& filename);  //load ROM from file into memory
        uint8_t readByte(uint16_t address); //read a Byte from the specified address
        void writeByte(uint16_t address, uint8_t value);    //write a Byte into the specified address
        uint16_t readWord(uint16_t address);   //read 2 Bytes from the specified address. so address and address+1
        void writeWord(uint16_t address, uint16_t value); //write 2 Bytes into specified address
        void clearMemory(); //reset all memory to 0 (not the fontset)
    private:
        std::array<uint8_t, 4096> memoryArray_;
};

#endif