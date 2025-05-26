#include "game.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <stdlib.h>
#include <math.h>

void init_game(GameState* state) {
  // Initialize user inputs
  state->key_angle_left_pressed  = 0;
  state->key_angle_right_pressed = 0;
  state->key_walk_left_pressed   = 0;
  state->key_walk_right_pressed  = 0;
  state->key_space_pressed       = 0;

  // locate Initial Ronnie Coordinates (IRC)
  state->ronnie.x = 50;
  state->ronnie.y = 100;

  // create initial Ronnie Chamber
  state->chambers[0] = malloc(sizeof(Chamber));
  state->chambers[0]->x = 0;
  state->chambers[0]->y = 0;
  state->chambers[0]->heightInTiles = 5;
  state->chambers[0]->widthInTiles = 5;

  // Initialize ball in bottom center
  state->ball.x = (float) (0.5 * SCREEN_WIDTH);  // middle of screen
  state->ball.y = (float) (0.9 * SCREEN_HEIGHT);  // near bottom
  state->ball.velocity_x = 0.0f;
  state->ball.velocity_y = 0.0f;
  state->ball.angle_rad  = 0.0f;
  state->ball.radius     = BALL_RADIUS;

  // Initialize wall on left and right side of alley
  state->walls[0].ori_x     = 0.2 * SCREEN_WIDTH;
  state->walls[0].ori_y     = 0;
  state->walls[0].width     = 10;
  state->walls[0].height    = SCREEN_HEIGHT;
  state->walls[0].angle_rad = 0.0f;
  state->walls[0].defined   = 1;

  state->walls[1].ori_x     = (0.8 * SCREEN_WIDTH) - 10;
  state->walls[1].ori_y     = 0;
  state->walls[1].width     = 10;
  state->walls[1].height    = SCREEN_HEIGHT;
  state->walls[1].angle_rad = 0.0f;
  state->walls[1].defined   = 1;

  init_pins(state);
}

// The way this should probably work is reading pin data from a file
// Same as walls, maybe like a levels.h where level data contains initial values
// for wall positions, pin positions
void init_pins(GameState* state){
  for(int i = 0; i < NUM_PINS; i++){
    state->pins[i].radius  = PIN_RADIUS;
    state->pins[i].height  = PIN_HEIGHT;
    state->pins[i].defined = 1;
    state->pins[i].alive   = 1;
  }

  // 6 7 8 9
  //  3 4 5
  //   1 2
  //    0

  state->pins[0].x       = SCREEN_WIDTH / 2;
  state->pins[0].y       = SCREEN_HEIGHT / 2;

  state->pins[1].x       = state->pins[0].x - (PIN_SPACING / 2);
  state->pins[1].y       = state->pins[0].y - (PIN_SPACING * sqrt(3) / 2);

  state->pins[2].x       = state->pins[1].x + PIN_SPACING;
  state->pins[2].y       = state->pins[1].y;

  state->pins[3].x       = state->pins[1].x - (PIN_SPACING / 2);
  state->pins[3].y       = state->pins[1].y - (PIN_SPACING * sqrt(3) / 2);

  state->pins[4].x       = state->pins[3].x + PIN_SPACING;
  state->pins[4].y       = state->pins[3].y;

  state->pins[5].x       = state->pins[4].x + PIN_SPACING;
  state->pins[5].y       = state->pins[4].y;

  state->pins[6].x       = state->pins[3].x - (PIN_SPACING / 2);
  state->pins[6].y       = state->pins[3].y - (PIN_SPACING * sqrt(3) / 2);

  state->pins[7].x       = state->pins[6].x + PIN_SPACING;
  state->pins[7].y       = state->pins[6].y;

  state->pins[8].x       = state->pins[7].x + PIN_SPACING;
  state->pins[8].y       = state->pins[7].y;

  state->pins[9].x       = state->pins[8].x + PIN_SPACING;
  state->pins[9].y       = state->pins[8].y;
}

void update_game(GameState* state, float delta_time) {
  // This is where you'll put your physics code!
  ball_update(state); // update ball position based on current velocity and user input
  for (Uint8 i = 0; i < MAX_WALLS; i++){
    if(state->walls[i].defined && ball_isCollidingWithWall(state, &state->walls[i])) {
      // TODO: angle of incidence, angle of reflection calculation
      printf("Ball is colliding w wall %d", i);
      state->ball.angle_rad = (2.0f * state->walls[i].angle_rad) - state->ball.angle_rad;
      ball_updateVelocity(&state->ball);
    }
  }

  for (Uint8 i = 0; i < NUM_PINS; i++){
    if(state->pins[i].defined && state->pins[i].alive && ball_isCollidingWithPin(&state->ball, &state->pins[i])) {
      state->pins[i].alive = 0;
      state->pins[i].angle_rad = ball_getPinCollisionAngle(&state->ball, &state->pins[i]);
    }
  }

  ronnie_update(state); // update ronnie position based on user input
}

void ronnie_update(GameState* state){
  if(!ball_isMoving(state)){
    if(state->key_space_pressed){ // launch ball based on current angle
      ball_updateVelocity(&state->ball);
    } else {
      if(state->key_walk_left_pressed)   state->ball.x -= 5;
      if(state->key_walk_right_pressed)  state->ball.x += 5;
      if(state->key_angle_left_pressed)  state->ball.angle_rad += (M_PI * 0.05f);
      if(state->key_angle_right_pressed) state->ball.angle_rad -= (M_PI * 0.05f);
    }
  } else {
    state->ball.x += state->ball.velocity_x;
    state->ball.y += state->ball.velocity_y;
  }

  if(state->key_walk_left_pressed)  state->ronnie.x -= 5;
  if(state->key_walk_right_pressed) state->ronnie.x += 5;
  if(state->key_walk_up_pressed)    state->ronnie.y -= 5;
  if(state->key_walk_down_pressed)  state->ronnie.y += 5;
}

// TODO: this function is doing too much, updating ball based on velocity
// should be diffeent from updating ball based on walking and both should
// be different from updating ball velocity itself
void ball_update(GameState* state){
  if(!ball_isMoving(state)){
    if(state->key_space_pressed){ // launch ball based on current angle
      ball_updateVelocity(&state->ball);
    } else {
      if(state->key_walk_left_pressed)   state->ball.x -= 5;
      if(state->key_walk_right_pressed)  state->ball.x += 5;
      if(state->key_angle_left_pressed)  state->ball.angle_rad += (M_PI * 0.05f);
      if(state->key_angle_right_pressed) state->ball.angle_rad -= (M_PI * 0.05f);
    }
  } else {
    state->ball.x += state->ball.velocity_x;
    state->ball.y += state->ball.velocity_y;
  }
}

void ball_updateVelocity(Ball* ball){
  ball->velocity_y = -5.0f * cos(ball->angle_rad);
  ball->velocity_x = -5.0f * sin(ball->angle_rad);
}

bool ball_isMoving(GameState* state){
  return (((state->ball.velocity_x * state->ball.velocity_x) +
          (state->ball.velocity_y * state->ball.velocity_y))
          > (EPSILON * EPSILON));
}

void handle_input(GameState* state, SDL_Event* event) {
  // This is where you'll handle keyboard/mouse input
  // For launching the ball, setting angle, etc.

  if (event->type == SDL_KEYDOWN) {
    SDL_Event quit_event;

    switch (event->key.keysym.sym) {
      case SDLK_ESCAPE:
        quit_event.type = SDL_QUIT;
        SDL_PushEvent(&quit_event);
        break;

      case SDLK_r:
        // reset game
        init_game(state);
        break;

      case SDLK_SPACE:
        state->key_space_pressed = 1;
        break;
      case SDLK_LEFT:
        state->key_angle_left_pressed = 1;
        break;
      case SDLK_RIGHT:
        state->key_angle_right_pressed = 1;
        break;
      case SDLK_a:
        state->key_walk_left_pressed = 1;
        break;
      case SDLK_d:
        state->key_walk_right_pressed = 1;
        break;
      case SDLK_w:
        state->key_walk_up_pressed = 1;
        break;
      case SDLK_s:
        state->key_walk_down_pressed = 1;
        break;
    }
  }

  if (event->type == SDL_KEYUP) {
    switch (event->key.keysym.sym) {
      case SDLK_SPACE:
        state->key_space_pressed = 0;
        break;
      case SDLK_LEFT:
        state->key_angle_left_pressed = 0;
        break;
      case SDLK_RIGHT:
        state->key_angle_right_pressed = 0;
        break;
      case SDLK_a:
        state->key_walk_left_pressed = 0;
        break;
      case SDLK_d:
        state->key_walk_right_pressed = 0;
        break;
      case SDLK_w:
        state->key_walk_up_pressed = 0;
        break;
      case SDLK_s:
        state->key_walk_down_pressed = 0;
        break;
    }
  }
}


// largely borrowed from jeffreythompson.org/collision-detection/circle-rect.php
bool ball_isCollidingWithWall(GameState* state, Wall *wall) {
  // determine the point on the rectangle (wall) closest to the center of the ball
  float testX;
  float testY;

  if (state->ball.x < wall->ori_x)
    testX = wall->ori_x;                // center of ball is to the left of the rectangle
  else if (state->ball.x > wall->ori_x + wall->width)
    testX = wall->ori_x + wall->width;  // center of ball is to the right of the rectangle
  else
    testX = state->ball.x;              // center of ball lies between the left and right coordinates of the rectangle

  if (state->ball.y < wall->ori_y)
    testY = wall->ori_y;                // center of ball is above the rectangle
  else if (state->ball.y > wall->ori_y + wall->height)
    testY = wall->ori_y + wall->height; // center of ball is below the rectangle
  else
    testY = state->ball.y;              // center of ball lies between the top and bottom coordinates of the rectangle

  float distX = state->ball.x - testX;
  float distY = state->ball.y - testY;

  //printf("distX: %f\ndistY: %f\nradius: %f", distX, distY, (float)state->ball.radius);
  return ((distX * distX) + (distY * distY)) <= (state->ball.radius * state->ball.radius);
}

bool ball_isCollidingWithPin(Ball* ball, Pin* pin) {
  float distX = ball->x - pin->x;
  float distY = ball->y - pin->y;

  float distSquared = (distX * distX) + (distY * distY);
  float combinedRadiusSquared = (ball->radius + pin->radius) * (ball->radius + pin->radius);

  return distSquared <= combinedRadiusSquared;
}


float ball_getPinCollisionAngle(Ball* ball, Pin* pin) {

  //      ^ 0 degrees
  // 90 <  > 270
  //     v 180
  //
  //  which means going more positive x in this frame means more north along the screen means smaller y coordinate
  float distX = -(pin->y - ball->y);
  //  and also going more positive y in this frame means more left along the screen means smaller x coordinate
  float distY = -(pin->x - ball->x);

  float angle_rad = atan(distY / distX);

  // angle_rad is between -90 and +90; so if we're in quadrant II or III, correct this
  // we're in quadrant II or III if our distX is negative
  if (distX < EPSILON_N)
    angle_rad += M_PI;

  return angle_rad;
}

void cleanup_game(GameState* state){
  for(int i = 0; i < MAX_CHAMBERS_ON_SCREEN; i++){
    if(state->chambers[i])
      free(state->chambers[i]);
  }
}
