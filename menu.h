#ifndef MENU_H
#define MENU_H

enum {
  MENU_ACTION = 1,
  MENU_LEFT = 2,
  MENU_RIGHT = 4
};

extern float menu_fgColor[4];
extern float menu_hlColor1[4];
extern float menu_hlColor2[4];

typedef struct {
  /* fonttex *font; */
  float fgColor[4]; /* entries */
  float hlColor1[4]; /* the highlighted one */
  float hlColor2[4];
  char szCaption[64];
} mDisplay;

typedef struct Menu {
  int nEntries;
  int iHighlight;
  mDisplay display;
  char szName[64];
  char szCapFormat[64];
  struct Menu** pEntries;
  struct Menu* parent;
  void* param; /* reserved to bind parameters at runtime */
} Menu;

typedef struct {
  void* data;
  void* next;
} node;

/* fixme: this variable is a hack */
extern int menutime;

#endif
