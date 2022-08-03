#include <SDL2/SDL.h>

#include <functional>

class Emulator {
  SDL_Window *window;
  SDL_Renderer *renderer;

 public:
  Emulator(const int width = 800, const int height = 600) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Voltage Oscilloscope Emulator", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  }

  void run(std::function<void()> callback) {
    bool quit = false;
    SDL_Event event;
    while (!quit) {
      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT ||
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
          quit = 1;
        }
      }

      callback();

      SDL_RenderClear(renderer);
      SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
};
