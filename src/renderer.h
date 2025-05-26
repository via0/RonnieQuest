#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include "game.h"

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  int screen_width;
  int screen_height;
} Renderer;

int init_renderer(Renderer* renderer);
void render_wall(Renderer* renderer, Wall* wall);
void render_pin(Renderer* renderer, Pin* pin);
void render_live_pin(Renderer* renderer, Pin* pin);
void render_dead_pin(Renderer* renderer, Pin* pin);
void render_ball(Renderer* renderer, Ball* ball);
void render_game(Renderer* renderer, GameState* state);
void cleanup_renderer(Renderer* renderer);

void render_tile(Renderer* renderer, Tile* tile);

#endif
