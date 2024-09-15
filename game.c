#include "gltron.h"

void initClientData() {
  /* for each player */
  /*   init camera (if any) */

  /* needs read access to server data
     to initialize camera */

  int i;
  int camType;
  Camera *cam;
  Data *data;

  for(i = 0; i < game->players; i++) {
    cam = game->player[i].camera;
    data = game->player[i].data;

    camType = (game->player[i].ai->active == 1) ? 0 : game->settings->camType;
    initCamera(cam, data, camType);

    memcpy(game->player[i].pColorDiffuse, 
	   colors_model_diffuse[i], sizeof( float[4] ));
    memcpy(game->player[i].pColorSpecular, 
	   colors_model_specular[i], sizeof( float[4] ));
    memcpy(game->player[i].pColorAlpha, colors_alpha[i], sizeof( float[4] ));
  }
}

void initDisplay(gDisplay *d, int type, int p, int onScreen) {
  int field;
  field = game->screen->vp_w / 32;
  d->h = game->screen->h;
  d->w = game->screen->w;
  d->vp_x = game->screen->vp_x + vp_x[type][p] * field;
  d->vp_y = game->screen->vp_y + vp_y[type][p] * field;
  d->vp_w = vp_w[type][p] * field;
  d->vp_h = vp_h[type][p] * field;
  d->blending = 1;
  d->fog = 0;
  d->wall = 1;
  d->onScreen = onScreen;
}  

void changeDisplay() {
  int i;
  for(i = 0; i < game->players; i++)
    game->player[i].display->onScreen = 0;
  for(i = 0; i < vp_max[game->settings->display_type]; i++)
       initDisplay(game->player[ game->settings->content[i] ].display, 
		   game->settings->display_type, i, 1);
}

#warning "audio engine"

/*
void playEngine(void *data, Uint8 *stream, int len) {
#ifdef SOUND
  mixEngineSound(0, stream, len);
#endif
}
*/

void initGame() { /* called when game mode is entered */
  SystemHidePointer();
  SystemWarpPointer(MOUSE_ORIG_X, MOUSE_ORIG_Y);
  game2->time.offset = SystemGetElapsedTime() - game2->time.current;
#ifdef SOUND
#warning "audio engine"
  // Mix_SetPostMix(playEngine, NULL);
  Audio_EnableEngine();
#endif
}

void exitGame() {
  SystemUnhidePointer();
#ifdef SOUND
#warning "audio engine"
  // Mix_SetPostMix(NULL, NULL);
  Audio_DisableEngine();
#endif
}

void defaultDisplay(int n) {
  printf("set display to %d\n", n);
  game->settings->display_type = n;
  game->settings->content[0] = 0;
  game->settings->content[1] = 1;
  game->settings->content[2] = 2;
  game->settings->content[3] = 3;
  changeDisplay();
}

void initGameScreen() {
  gDisplay *d;
  d = game->screen;
  d->h = game->settings->height; d->w = game->settings->width;
  d->vp_x = 0; d->vp_y = 0;
  d->vp_w = d->w; d->vp_h = d->h;
}

void cycleDisplay(int p) {
  int q;
  q = (game->settings->content[p] + 1) % game->players;
  while(q != game->settings->content[p]) {
    if(game->player[q].display->onScreen == 0)
      game->settings->content[p] = q;
    else q = (q + 1) % game->players;
  }
  changeDisplay();
}

void gameMouseMotion(int x, int y) {
  if(x != MOUSE_ORIG_X || y != MOUSE_ORIG_Y) {
    game2->input.mousex += x - MOUSE_ORIG_X;
    game2->input.mousey += y - MOUSE_ORIG_Y;
    /* fprintf(stderr, "Mouse: dx: %d\tdy: %d\n", 
       x - MOUSE_ORIG_Y, y - MOUSE_ORIG_Y); */
    /* 
       cam_phi += - (x - MOUSE_ORIG_X) * MOUSE_CX;
       cam_chi += (y - MOUSE_ORIG_Y) * MOUSE_CY;
       if(cam_chi < CAM_CHI_MIN) cam_chi = CAM_CHI_MIN;
       if(cam_chi > CAM_CHI_MAX) cam_chi = CAM_CHI_MAX;
    */
    SystemWarpPointer(MOUSE_ORIG_X, MOUSE_ORIG_Y);
  }
}

void gameMouse(int buttons, int state, int x, int y) {
  if(state == SYSTEM_MOUSEPRESSED) {
    if(buttons == SYSTEM_MOUSEBUTTON_LEFT) game2->input.mouse1 = 1;
    if(buttons == SYSTEM_MOUSEBUTTON_RIGHT) game2->input.mouse2 = 1;
  } else if(state == SYSTEM_MOUSERELEASED) {
    if(buttons == SYSTEM_MOUSEBUTTON_LEFT) game2->input.mouse1 = 0;
    if(buttons == SYSTEM_MOUSEBUTTON_RIGHT) game2->input.mouse2 = 0;
  }

  /*
  if(game->settings->camType == CAM_TYPE_MOUSE) 
    if(state == SYSTEM_MOUSEPRESSED) {
      if(buttons == SYSTEM_MOUSEBUTTON_LEFT) {
	cam_r -= CAM_DR;
	if(cam_r < CAM_R_MIN) cam_r = CAM_R_MIN;
      } else if(buttons == SYSTEM_MOUSEBUTTON_RIGHT) {
	cam_r += CAM_DR;
	if(cam_r > CAM_R_MAX) cam_r = CAM_R_MAX;
      }
    }
  */
  /* fprintf(stderr, "new cam_r: %.2f\n", cam_r); */
}






