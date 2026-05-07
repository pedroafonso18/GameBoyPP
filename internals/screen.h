#ifndef SCREEN_H
#define SCREEN_H

#include <SDL3/SDL.h>
#include <array>
#include <chrono>

class Screen {
  public:
    Screen();
    void Render();
    void Init();
    void Draw();

  private:  
    void InitWindow(
      float window_width,
      float window_height
    );

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* viewport_texture;
    int viewport_width = 160;
    int viewport_height = 144;
    std::array<uint8_t, 160 * 144 * 4> viewport_pixels;
    SDL_FRect viewport_rect = {0, 0, (float)viewport_width, (float)viewport_height};
    float window_height = viewport_height;
    float window_width = viewport_width;
    std::chrono::steady_clock::time_point startFrame;
    std::chrono::steady_clock::time_point endFrame;
    int framerate_time_us = 16742;

};

#endif