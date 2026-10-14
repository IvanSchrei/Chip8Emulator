#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <stdexcept>
#include <cstring>
#include "memory.h"

const uint8_t fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Memory::Memory(){
    //preload fontset starting at 0x050
    std::memcpy(memoryArray_.data() + 0x050, fontset, sizeof(fontset));
}

Memory::~Memory(){}

void Memory::loadROM(const std::string& filename){
    std::ifstream RomFile(filename, std::ios::binary);

    if(!RomFile.is_open()){
        throw std::runtime_error("[loadROM] Failed to open ROM file: " + filename);
    }

    RomFile.seekg(0, std::ios::end);
    size_t fileSize = RomFile.tellg();
    if(fileSize > (memoryArray_.size() - 0x200)){
        throw std::runtime_error("[loadROM] ROM to long to fit in memory!");
    }

    RomFile.seekg(0, std::ios::beg);
    RomFile.read(reinterpret_cast<char*>(memoryArray_.data() + 0x200), fileSize);

    if(RomFile.fail()){
        throw std::runtime_error("[loadROM] Failed to read ROM file: " + filename);
    }

    std::cout << "[loadROM] ROM file read correctly and saved into memory starting from 0x200";

    RomFile.close();
}

uint8_t Memory::readByte(uint16_t address){
    if(address >= memoryArray_.size()){
        throw std::out_of_range("[readByte] Memory address out of bounds!");
    }
    return memoryArray_[address];
}

void Memory::writeByte(uint16_t address, uint8_t value){
    if(address >= memoryArray_.size()){
        throw std::out_of_range("[writeByte] Memory address out of bounds!");
    }
    memoryArray_[address] = value;
}

uint16_t Memory::readWord(uint16_t address){
    if(address >= (memoryArray_.size() -1)){
        throw std::out_of_range("[readWord] Memory address out of bounds!");
    }
    //combine two bytes into 16bit word
    return (static_cast<uint16_t>(memoryArray_[address]) << 8 | memoryArray_[address + 1]);
}

void Memory::writeWord(uint16_t address, uint16_t value){
    if(address >= (memoryArray_.size() -1)){
        throw std::out_of_range("[writeWord] Memory address out of bounds!");
    }
    //split word into high and low byte and write them in
    memoryArray_[address] = static_cast<uint8_t>((value >> 8) & 0xFF);
    memoryArray_[address + 1] = static_cast<uint8_t>(value & 0xFF);
}

//delete memory but leave first 512 bytes untouched
void Memory::clearMemory(){
    std::memset(memoryArray_.data() + 0x200, 0, memoryArray_.size() - 0x200);
}