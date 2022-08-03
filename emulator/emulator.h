#ifndef VOLTAGE_EMULATOR_H_
#define VOLTAGE_EMULATOR_H_

#include <SDL2/SDL.h>

#include <functional>

#define VOLTAGE_EMULATOR
#include "../Voltage/src/Voltage.h"
#include "SDL2Writer.h"

class Emulator {
 public:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  const unsigned int resolution;
  const int size;
  unsigned int *pixels;

 public:
  Emulator(const unsigned int resolution) : resolution(resolution), size(1 << resolution) {
    pixels = new unsigned int[size * size];

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Voltage Oscilloscope Emulator", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, size, size, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                size, size);
  }

  ~Emulator() { delete[] pixels; }

  voltage::DualDACWriter *createWriter() { return new SDL2Writer(resolution, pixels); }

  void run(std::function<void()> callback) {
    bool quit = false;
    SDL_Event event;
    while (!quit) {
      SDL_UpdateTexture(texture, NULL, pixels, size * sizeof(unsigned int));

      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT ||
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
          quit = 1;
        }
      }

      for (int i = 0; i < size * size; i++) {
        pixels[i] = 0;
      }
      callback();

      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
};

#endif
