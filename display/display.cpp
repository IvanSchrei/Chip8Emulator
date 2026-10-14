#include <iostream>
#include "display.h"
#include <exception>
#include <SDL3/SDL.h>

Display::Display() : window_(nullptr), renderer_(nullptr) { // Initialize pointers
    clear();

    //init SDl
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL initialization failed");
    }

    //create window
    window_ = SDL_CreateWindow(
        "CHIP-8 Emulator",
        DISPLAY_WIDTH * SCALE_FACTOR,  // Scaled width
        DISPLAY_HEIGHT * SCALE_FACTOR, // Scaled height
        0 // No longer use SDL_WINDOW_SHOWN, windows are visible by default
    );

    if (window_ == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        throw std::runtime_error("SDL window creation failed");
    }

    // Create a renderer
    renderer_ = SDL_CreateRenderer(window_, nullptr); // Use nullptr for driver selection

    if (renderer_ == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window_);
        SDL_Quit();
        throw std::runtime_error("SDL renderer creation failed");
    }

    //Set render color to black (background will be black)
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);
}

Display::~Display() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Display::clear() {
    //set screen to black -> set whole buffer to false
    for (int i = 0; i < DISPLAY_HEIGHT; ++i) {
        for (int j = 0; j < DISPLAY_WIDTH; ++j) {
            displayBuffer_[i][j] = false;
        }
    }
}

bool Display::setPixel(int x, int y) {
    x %= DISPLAY_WIDTH;
    y %= DISPLAY_HEIGHT;

    bool collision = false;
    if (displayBuffer_[y][x]) {
        collision = true; // Pixel was already on
    }

    displayBuffer_[y][x] = !displayBuffer_[y][x]; // XOR the pixel state

    return collision;
}


const std::array<std::array<bool, DISPLAY_WIDTH>, DISPLAY_HEIGHT>& Display::getDisplayBuffer() const {
    return displayBuffer_;
}

void Display::render() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE); // Clear the screen
    SDL_RenderClear(renderer_);

    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE); // Set draw color to white

    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            if (displayBuffer_[y][x]) {
                // Draw a rectangle (scaled up) for each pixel
                SDL_FRect pixelRect = { static_cast<float>(x * SCALE_FACTOR), static_cast<float>(y * SCALE_FACTOR), static_cast<float>(SCALE_FACTOR), static_cast<float>(SCALE_FACTOR) };
                SDL_RenderFillRect(renderer_, &pixelRect);
            }
        }
    }

    SDL_RenderPresent(renderer_); //Update screen
}