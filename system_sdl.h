#ifndef SYSTEM_H
#define SYSTEM_H

#include "gltron.h"
#include <SDL/SDL.h>

#define SYSTEM_KEY_DOWN SDLK_DOWN
#define SYSTEM_KEY_UP SDLK_UP
#define SYSTEM_KEY_LEFT SDLK_LEFT
#define SYSTEM_KEY_RIGHT SDLK_RIGHT
#define SYSTEM_KEY_F1 SDLK_F1
#define SYSTEM_KEY_F2 SDLK_F2
#define SYSTEM_KEY_F3 SDLK_F3
#define SYSTEM_KEY_F5 SDLK_F5
#define SYSTEM_KEY_F10 SDLK_F10

#define SYSTEM_RGBA 1
#define SYSTEM_DOUBLE 2
#define SYSTEM_DEPTH 4
#define SYSTEM_FULLSCREEN 8

/* system specific functions (basically, an SDL/glut wrapper) */

extern void SystemInit(int *argc, char *argv[]);
extern void SystemPostRedisplay();
extern int SystemGetElapsedTime();
extern void SystemSwapBuffers();
extern void SystemWarpPointer(int x, int y);
extern void SystemMainLoop();
extern void SystemRegisterCallbacks(callbacks*);

extern void SystemInitWindow(int x, int y, int w, int h);
extern void SystemInitDisplayMode(int flags, unsigned char fullscreen);
extern int SystemCreateWindow(char *name);
extern void SystemDestroyWindow(int id);
extern void SystemReshapeFunc(void(*reshape)(int, int));

#endif










