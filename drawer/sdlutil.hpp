//------------------------------------------------------------------------------
//
// SDL implementation for drawer interface
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <stdexcept>

#include <SDL2/SDL.h>

#include "drawutil.hpp"

namespace DrawUtil {

class SDLSurface : public ISurface {
  SDL_Renderer *Renderer;

public:
  SDLSurface(SDL_Renderer *R) : Renderer(R) {}
  unsigned w() const noexcept override {
    int W;
    SDL_GetRendererOutputSize(Renderer, &W, NULL);
    return W;
  }
  unsigned h() const noexcept override {
    int H;
    SDL_GetRendererOutputSize(Renderer, NULL, &H);
    return H;
  }
  void putpixel(int X, int Y, unsigned C) override {
    SDL_SetRenderDrawColor(Renderer, cpart_r(C), cpart_g(C), cpart_b(C),
                           cpart_a(C));
    SDL_RenderDrawPoint(Renderer, X, Y);
  }
  void fillwith(unsigned C) override {
    SDL_SetRenderDrawColor(Renderer, cpart_r(C), cpart_g(C), cpart_b(C),
                           cpart_a(C));
    SDL_RenderClear(Renderer);
  }
  void putlogpixel(double X, double Y, unsigned Color) override;
};

void SDLSurface::putlogpixel(double X, double Y, unsigned Color) {
  int Width, Height;
  SDL_GetRendererOutputSize(Renderer, &Width, &Height);
  double WidthD = Width, HeightD = Height;
  int LogX = std::clamp(rint((X + 1.0) * Width / 2.0), 0.0, WidthD);
  int LogY = std::clamp(rint((Y + 1.0) * Height / 2.0), 0.0, HeightD);
  putpixel(LogX, LogY, Color);
}

class ViewPort : public IViewPort {
  int width, height;
  SDL_Window *screen;
  SDL_Renderer *ren;
  std::function<void(ISurface *)> callback;
  std::function<void(KeyPressed)> keybind;

public:
  ViewPort(const char *Name, int W, int H,
           std::function<void(ISurface *)> Callback, bool Resizable)
      : width{W}, height{H}, callback{Callback}, keybind{nullptr} {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
      throw std::runtime_error(SDL_GetError());

    atexit(SDL_Quit);
    screen = SDL_CreateWindow(
        Name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_SHOWN | (Resizable ? SDL_WINDOW_RESIZABLE : 0));
    if (screen == nullptr)
      throw std::runtime_error(SDL_GetError());

    ren = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
    if (ren == nullptr)
      throw std::runtime_error(SDL_GetError());
  }

  pollres poll() override;
  void dump(const char *name) const override;
  void bindkeys(std::function<void(KeyPressed)> k) override { keybind = k; }

  ~ViewPort() {
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(screen);
  }
};

pollres ViewPort::poll() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT)
      return pollres::STOP;
    if (keybind && (event.type == SDL_KEYDOWN)) {
      SDL_Keycode kc = event.key.keysym.sym;
      switch (kc) {
      case SDLK_UP:
        keybind(KeyPressed{KEY_UP});
        break;
      case SDLK_DOWN:
        keybind(KeyPressed{KEY_DOWN});
        break;
      case SDLK_LEFT:
        keybind(KeyPressed{KEY_LEFT});
        break;
      case SDLK_RIGHT:
        keybind(KeyPressed{KEY_RIGHT});
        break;
      case SDLK_p:
        keybind(KeyPressed{'p'});
        break;
      case SDLK_d:
        keybind(KeyPressed{'d'});
        break;
      case SDLK_r:
        keybind(KeyPressed{'r'});
        break;
      case SDLK_s:
        keybind(KeyPressed{'s'});
        break;
      case SDLK_PLUS:
      case SDLK_KP_PLUS:
        keybind(KeyPressed{'+'});
        break;
      case SDLK_MINUS:
      case SDLK_KP_MINUS:
        keybind(KeyPressed{'-'});
        break;
      }
    }
  }

  SDLSurface S{ren};
  callback(&S);
  SDL_RenderPresent(ren);
  return pollres::PROCEED;
}

void ViewPort::dump(const char *Name) const {
  SDL_Texture *Texture;
  Texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_TARGET, width, height);

  SDL_SetRenderTarget(ren, Texture);
  SDLSurface S(ren);
  callback(&S);
  int W = width, H = height;
  SDL_QueryTexture(Texture, NULL, NULL, &W, &H);
  SDL_Surface *Surface = SDL_CreateRGBSurface(0, W, H, 32, 0, 0, 0, 0);
  SDL_RenderReadPixels(ren, NULL, Surface->format->format, Surface->pixels,
                       Surface->pitch);
  SDL_SaveBMP(Surface, Name);
  SDL_FreeSurface(Surface);
  SDL_SetRenderTarget(ren, NULL);
  SDL_DestroyTexture(Texture);
}

} // namespace DrawUtil
