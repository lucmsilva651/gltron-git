#ifndef DATA_H
#define DATA_H

/* data structures */

#include "model.h"

typedef struct callbacks {
  void (*display)(void);
  void (*idle)(void);
  void (*keyboard)(int, int, int);
  void (*init)(void);
  void (*exit)(void);
  void (*initGL)(void);
} callbacks;

typedef struct line {
  float sx, sy, ex, ey;
} line;

typedef struct Model {
  Mesh* mesh; /* model */
  float color_alpha[4]; /* alpha trail */
  float color_trail[4]; /* solid edges of trail */
  float color_model[4]; /* model color */
} Model;

typedef struct Data {
  float posx; float posy;

  int dir; int last_dir;
  int turn_time;
  
  int score;
  float speed; /* set to -1 when dead */
  float trail_height; /* countdown to zero when dead */
  float exp_radius; /* explosion of the cycle model */
  line *trails;
  line *trail; /* current trail */
} Data;

typedef struct Camera {
  float cam[3];
  float target[3];
  float angle;
  int camType;
} Camera;

typedef struct AI {
  int active;
  int tdiff; /*  */
  int moves;
  int danger;
} AI;

typedef struct gDisplay {
  int win_id;     /* nur das globale Window hat eine */
  int h, w;       /* window */
  int vp_x, vp_y; /* viewport */
  int vp_h, vp_w;
  int blending;
  int fog;
  int wall;
  int onScreen;

  unsigned int texFloor; 
  unsigned int texWall_1;
  unsigned int texWall_2;
  unsigned int texWall_3;
  unsigned int texWall_4;
  unsigned int texGui;
  unsigned int texCrash;
} gDisplay;

typedef struct Player {
  Model *model;
  Data *data;
  Camera *camera;
  gDisplay *display;
  AI *ai;
} Player;

/* if you want to add something and make it permanent (via
   .gltronrc) then
   1) add it to Settings in data.h
   2) add it to settings.txt
   3) add pointer to initSettingsData() in settings.c
   4) add a default to initMainGameSettings() in settings.c
   5) make a menu entry in menu.txt
*/
typedef struct Settings {
  /* these settings affect the visuals and sound etc.
     and are client side only */

  int show_help;
  int show_fps;
  int show_wall;
  int show_2d;
  int show_alpha;
  int show_floor_texture;
  int show_glow;
  int show_ai_status;
  int show_model;
  int show_crash_texture;
  int model_backwards;
  int turn_cycle; /* smooth turning */
  int line_spacing; /* space between lines when the floor texture is
		       disabled */
  int use_mipmaps; /* enable / disable mipmapping */

  int camType;
  int mouse_warp;

  int display_type; /* 0-2 -> 1, 2 or 4 displays on the screen */
  int content[4]; /* max. 4 individual viewports on the screen */
  int windowMode; /* 0: fullscreen, non-zero: window mode */

  int fov;
  int width;
  int height;

  int playMusic;
  int playEffects;

  /* these two are ignored in multiplayer mode */
  int fast_finish;
  int screenSaver; /* 1: all for players are AIs when the game starts */

  /* no AI in multiplayer yet */
  int ai_player1;
  int ai_player2;
  int ai_player3;
  int ai_player4;

  /* these gettings affect the gameplay */
  int erase_crashed;
  int game_speed;
  float speed;
  float current_speed;
  int grid_size;
} Settings;

typedef struct Game {
  gDisplay *screen;
  Settings *settings;
  Player *player;
  int players; /* number of players - currently limited to 4 somewhere */
  int winner; /* who won this round */
  int pauseflag; /* if the game is finished: the PAUSE_GAME_FINISHED flag
		    is set */
  int running; /* the amount of players that are still alive */
} Game;

typedef struct settings_int {
  char name[32];
  int *value;
} settings_int;

typedef struct settings_float {
  char name[32];
  float *value;
} settings_float;

#endif
