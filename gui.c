#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gltron.h"

void guiProjection(int x, int y) {
  checkGLError("gui.c guiProj - start");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /*glOrtho(0, 0, x, y, -1, 1); */
  checkGLError("gui.c guiProj - proj");
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(game->screen->vp_x, game->screen->vp_y,
	     x, y);
  checkGLError("gui.c guiProj - end");
}

void drawGuiBackground() {
  checkGLError("gui background start");

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(game->screen);

  if(game->settings->softwareRendering) {
    glRasterPos2i(0, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDrawPixels(game->screen->vp_w, game->screen->vp_h,
		 GL_RGB, GL_UNSIGNED_BYTE,
		 game->screen->pixelGui);
  } else {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_GUI]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0, 0.0);
    glVertex2f(0, 0);

    glTexCoord2f(1.0, 0.0);
    glVertex2f(game->screen->vp_w, 0);

    glTexCoord2f(1.0, .75);
    glVertex2f(game->screen->vp_w, game->screen->vp_h);

    glTexCoord2f(0.0, .75);
    glVertex2f(0, game->screen->vp_h);

    glEnd();
  }
}

void drawGuiLogo() {
  float pos[] = { 512 - 10 - 256, 384 - 64 };
  float size[] = { 256, 64 };
  float glpos = 64;
  float glsize = 32;
  float font_shift[] = { 0.5, 0.00 };

  checkGLError("gui logo start");
  
  rasonly(game->screen);

  pos[0] *= game->screen->vp_w / 512.0;
  pos[1] *= game->screen->vp_h / 384.0;
  size[0] *= game->screen->vp_w / 512.0;
  size[1] *= game->screen->vp_h / 384.0;
  glpos *= game->screen->vp_w / 512.0;
  glsize *= game->screen->vp_w / 512.0;
  
  glEnable(GL_TEXTURE_2D);
  if(game->settings->show_gl_logo == 1) {
    glPushMatrix();
    glTranslatef(pos[0] - glpos + glsize * font_shift[0], 
		 pos[1] + glsize * font_shift[1], 0);
    glScalef(glsize, glsize, glsize);
    glColor3f(0.2, 0.4, 0.8);
    ftxRenderString(gameFtx, "gl", 2);
    glPopMatrix();
  }
  glBindTexture(GL_TEXTURE_2D, game->screen->textures[TEX_LOGO]);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);

  glTexCoord2f(0.0, 0.0);
  glVertex2f(pos[0], pos[1]);

  glTexCoord2f(1.0, 0.0);
  glVertex2f(pos[0] + size[0], pos[1]);

  glTexCoord2f(1.0, 1.0);
  glVertex2f(pos[0] + size[0], pos[1] + size[1]);

  glTexCoord2f(0.0, 1.0);
  glVertex2f(pos[0], pos[1] + size[1]);

  glEnd();

  glDisable(GL_BLEND);

  checkGLError("gui background end");
}
  
void displayGui() {
  drawGuiBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();
  drawMenu(game->screen);

  SystemSwapBuffers();  
}

void displayConfigure() {
  char message[] = "Press a key for this action!";
  drawGuiBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();
  drawMenu(game->screen);

  rasonly(game->screen);
  glColor3f(1.0, 1.0, 1.0);
  drawText(guiFtx, game->screen->vp_w / 6, 20,
	   game->screen->vp_w / (6.0 / 4.0 * strlen(message)), message);
  SystemSwapBuffers();
}

void idleGui() {
#ifdef SOUND
  soundIdle();
#endif
  SystemPostRedisplay(); /* animate menu */
}

void keyboardConfigure(int key, int unicode, int x, int y) {
  *configureKeyEntry = key;
  initMenuCaption(configureKeyMenu);
#ifdef SOUND
   playMenuFX(fx_action);
#endif
   drawGuiBackground();
   if(!game->settings->softwareRendering)
     drawGuiLogo();
   drawMenu(game->screen);
   rasonly(game->screen);
   SystemSwapBuffers();
  restoreCallbacks();
}

#ifdef __NETWORK__
static int nbreads = 0;
void readNickname(int key, int unicode, int x, int y)
{
  fprintf(stderr, "%c", key);
  if( nbreads == 0 )
    {
      strcpy(game->settings->nickname, "");
    }
  nbreads++;
  switch(key)
    {
    case 13:
      nbreads=0;   
      restoreCallbacks();
      break;
    case ';':
       sprintf(game->settings->nickname, "%s%c", game->settings->nickname, '.');
       break;
    case 304:
      break;
    default:
      if( key > 'A' && key < 'z' )
	{
	  sprintf(game->settings->nickname, "%s%c", game->settings->nickname, key);
	} else {
	  nbreads=0;
	  restoreCallbacks();
	}      
      break;
    }
  initMenuCaptions();
#ifdef SOUND
  playMenuFX(fx_action);
#endif
   drawGuiBackground();
   if(!game->settings->softwareRendering)
     drawGuiLogo();
   drawMenu(game->screen);
   rasonly(game->screen);
   SystemSwapBuffers();
}

void readServer(int key, int unicode, int x, int y)
{

  if( nbreads == 0 )
    {
      strcpy(game->settings->server, "");
    }
  switch(key)
    {
    case 13://return
    case SDLK_ESCAPE://escape
      nbreads=0;
      restoreCallbacks();
      break;
      
    case SDLK_BACKSPACE://backspace
      if( nbreads > 0 )
	{
	  nbreads--;
	  game->settings->server[nbreads]='\0';
	}
      break;
    default:
      if( unicode < 0x80 && unicode > 0 )
	{
	  sprintf(game->settings->server, "%s%c", game->settings->server, unicode);
	  nbreads++;
	}
  break;
}
  initMenuCaptions();
#ifdef SOUND
  playMenuFX(fx_action);
#endif
   drawGuiBackground();
   if(!game->settings->softwareRendering)
     drawGuiLogo();
   drawMenu(game->screen);
   rasonly(game->screen);
   SystemSwapBuffers();
}

void readTracker(int key, int unicode, int x, int y)
{

  if( nbreads == 0 )
    {
      strcpy(game->settings->tracker, "");
    }
  switch(key)
    {
    case 13://return
    case SDLK_ESCAPE://escape
      nbreads=0;
      restoreCallbacks();
      break;
      
    case SDLK_BACKSPACE://backspace
      if( nbreads > 0 )
	{
	  nbreads--;
	  game->settings->tracker[nbreads]='\0';
	}
      break;
    default:
      if( unicode < 0x80 && unicode > 0 )
	{
	  sprintf(game->settings->tracker, "%s%c", game->settings->tracker, unicode);
	  nbreads++;
	}
  break;
}
  initMenuCaptions();
#ifdef SOUND
  playMenuFX(fx_action);
#endif
   drawGuiBackground();
   if(!game->settings->softwareRendering)
     drawGuiLogo();
   drawMenu(game->screen);
   rasonly(game->screen);
   SystemSwapBuffers();
}

void readPort(int key, int unicode, int x, int y)
{

  printf("entering readPort\n");
  if( nbreads == 0 )
    {
      strcpy(game->settings->port, "");
    }
  switch(key)
    {
    case 13://return
    case SDLK_ESCAPE://escape
      nbreads=0;
      restoreCallbacks();
      break;
      
    case SDLK_BACKSPACE://backspace
      if( nbreads > 0 )
	{
	  nbreads--;
	  game->settings->port[nbreads]='\0';
	}
      break;
    default:
      if( unicode < 0x80 && unicode > 0 )
	{
	  sprintf(game->settings->port, "%s%c", game->settings->port, unicode);
	  nbreads++;
	}
  break;
}
  initMenuCaptions();
#ifdef SOUND
  playMenuFX(fx_action);
#endif
   drawGuiBackground();
   if(!game->settings->softwareRendering)
     drawGuiLogo();
   drawMenu(game->screen);
   rasonly(game->screen);
   SystemSwapBuffers();
}

void readTport(int key, int unicode, int x, int y)
{

  printf("entering readPort\n");
  if( nbreads == 0 )
    {
      strcpy(game->settings->tport, "");
    }
  switch(key)
    {
    case 13://return
    case SDLK_ESCAPE://escape
      nbreads=0;
      restoreCallbacks();
      break;
      
    case SDLK_BACKSPACE://backspace
      if( nbreads > 0 )
	{
	  nbreads--;
	  game->settings->tport[nbreads]='\0';
	}
      break;
    default:
      if( unicode < 0x80 && unicode > 0 )
	{
	  sprintf(game->settings->tport, "%s%c", game->settings->tport, unicode);
	  nbreads++;
	}
  break;
}
  initMenuCaptions();
#ifdef SOUND
  playMenuFX(fx_action);
#endif
   drawGuiBackground();
   if(!game->settings->softwareRendering)
     drawGuiLogo();
   drawMenu(game->screen);
   rasonly(game->screen);
   SystemSwapBuffers();
}


void displayNickname() {
  char message[] = "Enter your nickname...";
  drawGuiBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();
  drawMenu(game->screen);

  rasonly(game->screen);
  glColor3f(1.0, 1.0, 1.0);
  drawText(guiFtx, game->screen->vp_w / 6, 20,
	   game->screen->vp_w / (6.0 / 4.0 * strlen(message)), message);
  SystemSwapBuffers();
}

void displayServer() {
  char message[] = "Enter server name or server IP...";
  drawGuiBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();
  drawMenu(game->screen);

  rasonly(game->screen);
  glColor3f(1.0, 1.0, 1.0);
  drawText(guiFtx, game->screen->vp_w / 6, 20,
	   game->screen->vp_w / (6.0 / 4.0 * strlen(message)), message);
  SystemSwapBuffers();
}

void displayTracker() {
  char message[] = "Enter tracker name or tracker IP...";
  drawGuiBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();
  drawMenu(game->screen);

  rasonly(game->screen);
  glColor3f(1.0, 1.0, 1.0);
  drawText(guiFtx, game->screen->vp_w / 6, 20,
	   game->screen->vp_w / (6.0 / 4.0 * strlen(message)), message);
  SystemSwapBuffers();
}



void displayPort() {
  char message[] = "Enter port...";
  drawGuiBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();
  drawMenu(game->screen);

  rasonly(game->screen);
  glColor3f(1.0, 1.0, 1.0);
  drawText(guiFtx, game->screen->vp_w / 6, 20,
	   game->screen->vp_w / (6.0 / 4.0 * strlen(message)), message);
  SystemSwapBuffers();
}

void displayTport() {
  char message[] = "Enter port of tracker ...";
  drawGuiBackground();
  if(!game->settings->softwareRendering)
    drawGuiLogo();
  drawMenu(game->screen);

  rasonly(game->screen);
  glColor3f(1.0, 1.0, 1.0);
  drawText(guiFtx, game->screen->vp_w / 6, 20,
	   game->screen->vp_w / (6.0 / 4.0 * strlen(message)), message);
  SystemSwapBuffers();
}
#endif

void keyboardGui(int key, int unicode, int x, int y) {
  int i;
  switch(key) {
  case 27:
#ifdef SOUND
    if(game->settings->playEffects)
      playMenuFX(fx_action);
#endif
    if(pCurrent->parent == NULL)
      restoreCallbacks();
    else
      pCurrent = pCurrent->parent;
    break;
  case 13: case ' ':
#ifdef SOUND
    if(game->settings->playEffects)
      playMenuFX(fx_action);
#endif
    menuAction(*(pCurrent->pEntries + pCurrent->iHighlight), MENU_ACTION);
    break;
  case SYSTEM_KEY_LEFT:
    menuAction(*(pCurrent->pEntries + pCurrent->iHighlight), MENU_LEFT);
    break;
  case SYSTEM_KEY_RIGHT:
    menuAction(*(pCurrent->pEntries + pCurrent->iHighlight), MENU_RIGHT);
    break;
    /* case 'q': SystemExit(); break; */
  case SYSTEM_KEY_DOWN:
#ifdef SOUND
    if(game->settings->playEffects)
      playMenuFX(fx_highlight);
#endif
    pCurrent->iHighlight = (pCurrent->iHighlight + 1) % pCurrent->nEntries;
    break;
  case SYSTEM_KEY_UP:
#ifdef SOUND
    if(game->settings->playEffects)
      playMenuFX(fx_highlight);
#endif
    pCurrent->iHighlight = (pCurrent->iHighlight - 1) % pCurrent->nEntries;
    if(pCurrent->iHighlight < 0)
      pCurrent->iHighlight = pCurrent->nEntries - 1;
    break;
  case SYSTEM_KEY_F11: doBmpScreenShot(); break;
  case SYSTEM_KEY_F12: doScreenShot(); break;
    /* debug code follows */
  case 'l':
    printf("%d entries:\n", pCurrent->nEntries);
    for(i = 0; i < pCurrent->nEntries; i++)
      printf("printing '%s' - %d entries\n",
	     ((Menu*)*(pCurrent->pEntries + i))->szName,
	     ((Menu*)*(pCurrent->pEntries + i))->nEntries);
    break;
  default: printf("got key %d\n", key);
  }
  SystemPostRedisplay();
}

void initGui() {
  menutime = SystemGetElapsedTime();
  if(pCurrent == NULL) {
    pCurrent = *pMenuList; /* erstes Menu ist RootMenu - Default pCurrent */
    pCurrent->iHighlight = 0;
  }
}

void exitGui() {
  glShadeModel( game->screen->shademodel );
}

void initGLGui() {
  glShadeModel(GL_FLAT);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  SystemPostRedisplay();
}

static int current_highlight = -1;
	
void guiMouse(int buttons, int state, int x, int y) {
  fprintf(stderr, "Mouse buttons: %d, State %d, Position (%d, %d)\n",
	  buttons, state, x, y); 

  /* fprintf(stderr, "testing for state == %d\n", SYSTEM_MOUSEPRESSED); */
  if (state == SYSTEM_MOUSEPRESSED) {	
#ifdef SOUND
    if(game->settings->playEffects)
      playMenuFX(fx_action);
#endif	
    menuAction(*(pCurrent->pEntries + pCurrent->iHighlight), MENU_ACTION);
    SystemPostRedisplay();
  }
}

void guiMouseMotion(int mx, int my) {
  /* fprintf(stderr, "Mouse motion at (%d, %d)\n", x, y); */
 
  /* I am using the calculation in drawMenu, perhaps
     these values should be bound to Menu structure in the future. */

  /* TODO: this is cut-and-paste from menu.c: VERY UGLY */
  int i;
  int x, y, size, lineheight;
  int hsize, vsize;
  int maxw = 0;

#define MENU_TEXT_START_X 0.08
#define MENU_TEXT_START_Y 0.40

#define MENU_WIDTH 0.80
#define MENU_HEIGHT 0.40

#define MENU_TEXT_LINEHEIGHT 1.5
	
  gDisplay *d = game->screen; 
	
  x = (int) (d->vp_w * MENU_TEXT_START_X);
  y = (int) (d->vp_h * MENU_TEXT_START_Y);

  /* transform mouse y-coordinate */

  my = d->vp_h - my;

  /* new stuff: calculate menu dimensions */
  for(i = 0; i < pCurrent->nEntries; i++) {
    int len;
    len =  strlen(((Menu*)*(pCurrent->pEntries + i))->display.szCaption);
    if(len > maxw)
      maxw = len;
}
  /* adjust size so menu fits into MENU_WIDTH/HEIGHT */

  hsize = (int) ((float)d->vp_w * MENU_WIDTH / (float)maxw );
  vsize = (int) ((float)d->vp_h * MENU_HEIGHT / 
		 ( (float)pCurrent->nEntries * MENU_TEXT_LINEHEIGHT));

  size = (hsize < vsize) ? hsize : vsize;

  lineheight = (int)( (float) size * MENU_TEXT_LINEHEIGHT);  

  for (i = 0; i < pCurrent->nEntries; i++) {
    if (current_highlight != i)
      if (my < y + lineheight && my > y ) {
	current_highlight = i;

#ifdef SOUND
	if(game->settings->playEffects)
	  playMenuFX(fx_highlight);
#endif
	pCurrent->iHighlight = i;
	SystemPostRedisplay ();
	break;	
      }
 		
    y -= lineheight;
  }
}

callbacks configureCallbacks = {
  displayConfigure, idleGui, keyboardConfigure, initGui, exitGui, initGLGui,
  NULL, NULL
};

callbacks guiCallbacks = {
  displayGui, idleGui, keyboardGui, initGui, exitGui, initGLGui, 
  guiMouse, guiMouseMotion
};

#ifdef __NETWORK__
callbacks nicknameCallbacks = {
  displayNickname, idleGui, readNickname, initGui, exitGui, initGLGui,
  NULL, NULL
};

callbacks serverCallbacks = {
  displayServer, idleGui, readServer, initGui, exitGui, initGLGui,
  NULL, NULL
};

callbacks portCallbacks = {
  displayPort, idleGui, readPort, initGui, exitGui, initGLGui,
  NULL, NULL
};

callbacks trackerCallbacks = {
  displayTracker, idleGui, readTracker, initGui, exitGui, initGLGui,
  NULL, NULL
};

callbacks tportCallbacks = {
  displayTport, idleGui, readTport, initGui, exitGui, initGLGui,
  NULL, NULL
};
#endif
