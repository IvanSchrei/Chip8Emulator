#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include "memory.h"

// memory_tests.cpp -  Test cases for the Memory class
void testConstructorFontsetPreloaded() {
    Memory memory;
    uint8_t expected_fontset[] = {
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

    for (int i = 0; i < 80; ++i) {
        assert(memory.readByte(0x050 + i) == expected_fontset[i]);
    }
    std::cout << "testConstructorFontsetPreloaded passed" << std::endl;
}

void testLoadROMSuccess() {
    Memory memory;
    std::string rom_file = "test_rom.bin"; // Create a small test ROM file

    // Create a dummy ROM file for testing
    std::ofstream test_rom(rom_file, std::ios::binary);
    uint8_t rom_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    test_rom.write(reinterpret_cast<const char*>(rom_data), sizeof(rom_data));
    test_rom.close();

    memory.loadROM(rom_file);

    for (size_t i = 0; i < sizeof(rom_data); ++i) {
        assert(memory.readByte(0x200 + i) == rom_data[i]);
    }

    remove(rom_file.c_str()); // Clean up the test file
    std::cout << "testLoadROMSuccess passed" << std::endl;
}

void testLoadROMFileOpenError() {
    Memory memory;
    std::string rom_file = "nonexistent_rom.bin";

    bool exceptionThrown = false;
    try {
        memory.loadROM(rom_file);
    }
    catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == ("[loadROM] Failed to open ROM file: " + rom_file));
        exceptionThrown = true;
    }
    assert(exceptionThrown);
    std::cout << "testLoadROMFileOpenError passed" << std::endl;
}

void testLoadROMROMTooLarge() {
    Memory memory;
    std::string rom_file = "large_rom.bin";

    // Create a large ROM file
    std::ofstream test_rom(rom_file, std::ios::binary);
    uint8_t rom_data[4097]; // larger than the available space after 0x200
    test_rom.write(reinterpret_cast<const char*>(rom_data), sizeof(rom_data));
    test_rom.close();

    bool exceptionThrown = false;
    try {
        memory.loadROM(rom_file);
    }
    catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "[loadROM] ROM to long to fit in memory!");
        exceptionThrown = true;
    }
    assert(exceptionThrown);
    remove(rom_file.c_str()); // Clean up the test file
    std::cout << "testLoadROMROMTooLarge passed" << std::endl;
}

void testReadByteValidAddress() {
    Memory memory;
    memory.writeByte(0x100, 0xAB);
    assert(memory.readByte(0x100) == 0xAB);
    std::cout << "testReadByteValidAddress passed" << std::endl;
}

void testReadByteOutOfBounds() {
    Memory memory;
    bool exceptionThrown = false;
    try {
        memory.readByte(4096); // Out of bounds address
    }
    catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "[readByte] Memory address out of bounds!");
        exceptionThrown = true;
    }
    assert(exceptionThrown);
    std::cout << "testReadByteOutOfBounds passed" << std::endl;
}

void testWriteByteValidAddress() {
    Memory memory;
    memory.writeByte(0x100, 0xCD);
    assert(memory.readByte(0x100) == 0xCD);
    std::cout << "testWriteByteValidAddress passed" << std::endl;
}

void testWriteByteOutOfBounds() {
    Memory memory;
    bool exceptionThrown = false;
    try {
        memory.writeByte(4096, 0x12); // Out of bounds address
    }
    catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "[writeByte] Memory address out of bounds!");
        exceptionThrown = true;
    }
    assert(exceptionThrown);
    std::cout << "testWriteByteOutOfBounds passed" << std::endl;
}

void testReadWordValidAddress() {
    Memory memory;
    memory.writeByte(0x100, 0x12);
    memory.writeByte(0x101, 0x34);
    assert(memory.readWord(0x100) == 0x1234);
    std::cout << "testReadWordValidAddress passed" << std::endl;
}

void testReadWordOutOfBounds() {
    Memory memory;
    bool exceptionThrown1 = false;
    try {
        memory.readWord(4095); // Address + 1 is out of bounds
    }
    catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "[readWord] Memory address out of bounds!");
        exceptionThrown1 = true;
    }
    assert(exceptionThrown1);

    bool exceptionThrown2 = false;
    try {
        memory.readWord(4096); // Out of bounds address
    }
    catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "[readWord] Memory address out of bounds!");
        exceptionThrown2 = true;
    }
    assert(exceptionThrown2);
    std::cout << "testReadWordOutOfBounds passed" << std::endl;
}

void testWriteWordValidAddress() {
    Memory memory;
    memory.writeWord(0x100, 0xABCD);
    assert(memory.readByte(0x100) == 0xAB);
    assert(memory.readByte(0x101) == 0xCD);
    std::cout << "testWriteWordValidAddress passed" << std::endl;
}

void testWriteWordOutOfBounds() {
    Memory memory;
    bool exceptionThrown1 = false;
    try {
        memory.writeWord(4095, 0x1234); // Address + 1 is out of bounds
    }
    catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "[writeWord] Memory address out of bounds!");
        exceptionThrown1 = true;
    }
    assert(exceptionThrown1);

    bool exceptionThrown2 = false;
    try {
        memory.writeWord(4096, 0x1234); // Out of bounds address
    }
    catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "[writeWord] Memory address out of bounds!");
        exceptionThrown2 = true;
    }
    assert(exceptionThrown2);
    std::cout << "testWriteWordOutOfBounds passed" << std::endl;
}

void testClearMemory() {

    Memory memory;
    // Preload some values in memory after the fontset
    memory.writeByte(0x200, 0x11);
    memory.writeByte(0x300, 0x22);
    memory.writeByte(0x400, 0x33);

    memory.clearMemory();

    // Check that the fontset is still intact
    uint8_t expected_fontset[] = {
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

    for (int i = 0; i < 80; ++i) {
        assert(memory.readByte(0x050 + i) == expected_fontset[i]);
    }

    // Check that the other memory locations have been cleared
    assert(memory.readByte(0x200) == 0x00);
    assert(memory.readByte(0x300) == 0x00);
    assert(memory.readByte(0x400) == 0x00);
    std::cout << "testClearMemory passed" << std::endl;
}

int main() {
    testConstructorFontsetPreloaded();
    testLoadROMSuccess();
    testLoadROMFileOpenError();
    testLoadROMROMTooLarge();
    testReadByteValidAddress();
    testReadByteOutOfBounds();
    testWriteByteValidAddress();
    testWriteByteOutOfBounds();
    testReadWordValidAddress();
    testReadWordOutOfBounds();
    testWriteWordValidAddress();
    testWriteWordOutOfBounds();
    testClearMemory();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}