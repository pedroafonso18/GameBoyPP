#include "screen.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <chrono>

void Screen::Init() {
  viewport_pixels.fill(0xFF);
  InitWindow(window_width, window_height);
  viewport_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, viewport_width, viewport_height);
}

void Screen::InitWindow(
  float window_width,
  float window_height
) 
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer("GameBoyPP", window_width * 2, window_height * 2, 0, &window, &renderer);
  SDL_SetWindowResizable(window, true);
}

void Screen::Render() {
  SDL_SetTextureColorMod(viewport_texture, 255, 255, 255);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, viewport_texture);
  Draw();
  SDL_RenderTexture(renderer, viewport_texture, nullptr, &viewport_rect);
  SDL_RenderPresent(renderer);
}

void Screen::Draw() {

}