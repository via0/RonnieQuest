#include <SDL2/SDL.h>
#include "game.h"
#include "renderer.h"

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

int main(int argc, char* argv[]) {
    GameState game_state;
    Renderer renderer;
    
    // Initialize SDL and renderer
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

#ifdef DEBUG
    if (!init_debug()) {
      // init debug failed, handle error
      cleanup_renderer(&renderer);
      SDL_Quit();
      return 1;
    }
#endif
    
    if (!init_renderer(&renderer)) {
        SDL_Quit();
        return 1;
    }
    
    // Initialize game state
    init_game(&game_state);
    
    // Main game loop
    int running = 1;
    Uint32 previous_frame_time = SDL_GetTicks();
    
    while (running) {
        Uint32 frame_start_time = SDL_GetTicks();
        float delta_time = (frame_start_time - previous_frame_time) / 1000.0f;
        previous_frame_time = frame_start_time;
        
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            handle_input(&game_state, &event);
        }
        
        // Update game state
        update_game(&game_state, delta_time);
        
        // Render
        render_game(&renderer, &game_state);
        
        // Cap frame rate
        Uint32 frame_time = SDL_GetTicks() - frame_start_time;
        if (frame_time < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frame_time);
        }
    }
    
    // Cleanup
    cleanup_renderer(&renderer);
#ifdef DEBUG
    cleanup_debug();
#endif
    SDL_Quit();
    
    return 0;
}
