#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

#define MAX_WALLS  10 // maximum number of walls a level can have
#define NUM_PINS   10 // 10 pins in a bowling game, of course!
#define NUM_FRAMES 10 // 10 frames in a game, of course!!

#define PIN_RADIUS  5
#define PIN_HEIGHT  (3 * PIN_RADIUS) // THAT'S QUITE BIG
#define PIN_SPACING 25 // WE'LL PLAY THIS BY EAR
//#define PIN_SPACING (PIN_RADIUS * 12 / 5) // WE'LL PLAY THIS BY EAR

#define BALL_RADIUS 10

#define EPSILON    0.0001f // floating point zero
#define EPSILON_N -0.0001f // floating point negative 0

#define TILE_SIDE_LENGTH 32 // each tile is 32x32 pixels
#define MAX_CHAMBERS_ON_SCREEN 10 // way overkill baby cmon now

typedef enum {
  DOOR_NONE,
  DOOR_NORTH,
  DOOR_SOUTH,
  DOOR_EAST,
  DOOR_WEST
} DoorType;

typedef enum {
  CHAMBER_HALLWAY_HORIZONTAL,
  CHAMBER_HALLWAY_VERTICAL,
  CHAMBER_ROOM
} ChamberType;

typedef struct {
  // upper left corner of tile
  int16_t x;
  int16_t y;
  DoorType doorType;
} Tile;

typedef struct {
  uint8_t widthInTiles;
  uint8_t heightInTiles;
  // upper left corner of chamber (location of first tile)
  int16_t x;
  int16_t y;
  ChamberType chamberType;

  // array of tiles (dynamically allocated)
  Tile* tiles;
} Chamber;

typedef struct {
  int16_t x;
  int16_t y;
} Ronnie;

typedef struct {
  int ori_x;
  int ori_y;
  int width;
  int height;
  float angle_rad; // 0 = north

  bool defined;
} Wall;

typedef struct {
  float x;
  float y;
  float velocity_x;
  float velocity_y;
  Uint8 radius;
  float angle_rad; // 0 = north
} Ball;

typedef struct {
  int x;
  int y;
  Uint8 radius;
  Uint8 height; // when it's knocked over, this is length of the pin
  float angle_rad; // the angle at which the dead pin is rotated; 0 = north
  bool alive;   // has this pin been defeated?
  bool defined; // should this pin be rendered at all?
} Pin;

typedef struct {
  uint8_t key_space_pressed;
  uint8_t key_angle_left_pressed;
  uint8_t key_angle_right_pressed;
  uint8_t key_walk_left_pressed;
  uint8_t key_walk_right_pressed;
  uint8_t key_walk_up_pressed;
  uint8_t key_walk_down_pressed;

  Wall walls[MAX_WALLS];
  Ball ball;
  Pin  pins[NUM_PINS];

  Chamber* chambers[MAX_CHAMBERS_ON_SCREEN];
  uint8_t numValidChambers; // how many chambers exist rn (right now)

  Ronnie ronnie;
} GameState;

void init_game(GameState* state);
void init_pins(GameState* state);
void update_game(GameState* state, float delta_time);
void handle_input(GameState* state, SDL_Event* event);

void ball_update(GameState* state);
void ball_updateVelocity(Ball* ball);
bool ball_isMoving(GameState* state);
bool ball_isCollidingWithWall(GameState* state, Wall* wall);
bool ball_isCollidingWithPin(Ball* ball, Pin* pin);
float ball_getPinCollisionAngle(Ball* ball, Pin* pin);

Chamber* chamber_create(int16_t x, int16_t y, uint8_t sideLength, ChamberType chamberType);
void chamber_delete(Chamber* chamber); // deallocate chamber memory

void ronnie_update(GameState* state);

void cleanup_game(GameState* state);

#endif
