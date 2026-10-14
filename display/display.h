#ifndef DISPLAY_H
#define DISPLAY_H

#include <array>
#include <SDL3/SDL.h>

// Define the display dimensions (CHIP-8 standard)
const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 32;

// Scaling factor for the display window (adjust as needed)
const int SCALE_FACTOR = 10;  // Example: Each pixel is 10x10

class Display {
public:
    Display();
    ~Display();

    void clear();
    bool setPixel(int x, int y);
    const std::array<std::array<bool, DISPLAY_WIDTH>, DISPLAY_HEIGHT>& getDisplayBuffer() const;
    void render();

private:
    std::array<std::array<bool, DISPLAY_WIDTH>, DISPLAY_HEIGHT> displayBuffer_;
    SDL_Window* window_;
    SDL_Renderer* renderer_;
};

#endif