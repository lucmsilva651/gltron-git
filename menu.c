#include "gltron.h"
#include <string.h>

#define MENU_BUFSIZE 100

#define MENU_TEXT_START_X 8
#define MENU_TEXT_START_Y 3
#define MENU_TEXT_SCREEN_SIZE 32
#define MENU_TEXT_LINEHEIGHT 2.0

Menu *pCurrent;

void changeAction(char *name) {
#ifdef SOUND
  if(strstr(name, "playMusic") == name) {
    if(game->settings->playMusic == 0)
      stopSound();
    else playSound();
  }
#endif
  if(strstr(name, "use_mipmaps") == name) {
    deleteFonts();
    deleteTextures(game->screen);
    initFonts();
    initTexture(game->screen);
  }
  if(strstr(name, "game_speed") == name)
    game->settings->current_speed = 
      default_speeds[ game->settings->game_speed ];
  if(strstr(name, "resetScores") == name)
    resetScores();
  if(strstr(name, "ai_player") == name) {
    int c;
    int *v;

    /* printf("changing AI status\n"); */
    c = name[9] - '0';
    v = getVi(name);
    game->player[c - 1].ai->active = *v;
    /* printf("changed AI status for player %c\n", c + '0'); */
  }
  if(strstr(name, "windowMode") == name) {
    initGameScreen();
    shutdownDisplay(game->screen);
    setupDisplay(game->screen);

    updateCallbacks();
    changeDisplay();
  }
}

void menuAction(Menu *activated) {
  int x, y;
  char c;
  int *piValue;
  if(activated->nEntries > 0) {
    pCurrent = activated;
    pCurrent->iHighlight = 0;
  } else {
    switch(activated->szName[1]) { /* second char */
    case 'q': saveSettings(); exit(0); break;
    case 'r': 
      initData();
      switchCallbacks(&pauseCallbacks);
      break;
    case 'v':
      sscanf(activated->szName, "%cv%dx%d ", &c, &x, &y);
      game->settings->width = x;
      game->settings->height = y;

      initGameScreen();
      shutdownDisplay(game->screen);
      setupDisplay(game->screen);

      updateCallbacks();
      changeDisplay();
      break;
    case 't':
      switch(activated->szName[2]) {
      case 'i':
	piValue = getVi(activated->szName + 4);
	if(piValue != 0) {
	  *piValue = (*piValue - 1) * (-1);
	  initMenuCaption(activated);
	  changeAction(activated->szName + 4);
	}
	break;
      case 'l': {
	char buf[64];
	int max_value;
	int dummy;
	sscanf(activated->szName, "stl_%d_%d_%s", &dummy, &max_value, buf);
	piValue = getVi(buf);
	(*piValue)++;
	if(*piValue > max_value) *piValue = 0;
	initMenuCaption(activated);
	changeAction(buf);
	break;
      }
      }
      break;
    case 'p':
      changeAction(activated->szName + 4);
    case 'c':
      chooseCallback(activated->szName + 3);
      break;
    default: printf("got action for menu %s\n", activated->szName); break;
    }
  }
}

static char *speed_list[] = {  "boring", "normal", "fast", "crazy", "custom" };
static char **clists[] = { speed_list };

void initMenuCaption(Menu *m) {
  int *piValue;

  /* TODO support all kinds of types */
  switch(m->szName[0]) {
  case 's':
    switch(m->szName[1]) {
    case 't':
      switch(m->szName[2]) {
      case 'i':
	/* printf("dealing with %s\n", m->szName); */
	piValue = getVi(m->szName + 4);
	if(piValue != 0) {
	  if(*piValue == 0) sprintf(m->display.szCaption,
				    m->szCapFormat, "off");
	  else sprintf(m->display.szCaption, m->szCapFormat, "on");
	  /* printf("changed caption to %s\n", m->display.szCaption); */
	} /* else printf("can't find value for %s\n", m->szName + 4); */
	break;
      case 'l':
	{
	  int clist_index;
	  int dummy;
	  char buf[64];
	  sscanf(m->szName, "stl_%d_%d_%s ", &clist_index, &dummy, buf);
	  piValue = getVi(buf);
	  /* TODO: bounds checking */
	  sprintf(m->display.szCaption, m->szCapFormat,
		  (clists[clist_index])[*piValue]);
	  break;
	}
      }
    }
    break;
    /* c entries change the callback */

  default:
    sprintf(m->display.szCaption, "%s", m->szCapFormat);
  }
}

void getNextLine(char *buf, int bufsize, FILE* f) {
  fgets(buf, bufsize, f);
  while((buf[0] == '\n' || buf[0] == '#') && /* ignore empty lines, comments */
	fgets(buf, bufsize, f));
}

Menu* loadMenu(FILE* f, char* buf, Menu* parent, int level) {
  Menu* m;
  int i;


  if(level > 4) {
    printf("recursing level > 4 - aborting\n");
    exit(1);
  }

  m = (Menu*) malloc(sizeof(Menu));
  m->parent = parent;
  getNextLine(buf, MENU_BUFSIZE, f);
  sscanf(buf, "%d ", &(m->nEntries));

  getNextLine(buf, MENU_BUFSIZE, f);
  buf[31] = 0; /* enforce menu name limit; */
  sprintf(m->szName, "%s", buf);
  if(*(m->szName + strlen(m->szName) - 1) == '\n')
    *(m->szName + strlen(m->szName) - 1) = 0;
  

  getNextLine(buf, MENU_BUFSIZE, f);
  buf[31] = 0; /* enforce menu caption limit; */
  sprintf(m->szCapFormat, "%s", buf);
  /* remove newline */
  for(i = 0; *(m->szCapFormat + i) != 0; i++)
    if (*(m->szCapFormat + i) == '\n') {
      *(m->szCapFormat + i) = 0;
      break;
    }

  initMenuCaption(m);
	
  /* printf("menu '%s': %d entries\n", m->szName, m->nEntries); */
  if(m->nEntries > 0) {
    m->pEntries = malloc(sizeof(Menu*) * m->nEntries);
    for(i = 0; i < m->nEntries; i++) {
      /* printf("loading menu number %d\n", i); */
      if(i > 10) {
	printf("item limit reached - aborting\n");
	exit(1);
      }
      *(m->pEntries + i) = loadMenu(f, buf, m, level + 1);
    }
  }

  return m;
}

Menu** loadMenuFile(char *filename) {
  char buf[MENU_BUFSIZE];
  FILE* f;
  Menu* m;
  Menu** list = NULL;
  int nMenus;
  int i, j;
  node *head;
  node *t;
  node *z;
  int sp = 0;

  if((f = fopen(filename, "r")) == NULL)
    return 0;
  /* read count of Menus */
  getNextLine(buf, MENU_BUFSIZE, f);
  sscanf(buf, "%d ", &nMenus);
  if(nMenus <= 0) return 0;

  /* allocate space for data structures */
  list = (Menu**) malloc(sizeof(Menu*) * nMenus);

  /* load data */
  for(i = 0; i < nMenus; i++) {
    /* printf("loading menu set %d\n", i); */
    if(i > 10) exit(1);
    *(list + i) = loadMenu(f, buf, NULL, 0);
  }
  fclose(f);

  /* TODO(3): now since I eliminated the need for cx/cy, why */
  /* do I need to traverse the Menu Tree? Just to set the colors??? */

  /* traverse Menu Tree and set Menu Color to some boring default */
  /* printf("finished parsing file - now traversing menus\n"); */
  /* setup stack */
  z = (node*) malloc(sizeof(node));
  z->next = z;
  head = (node*) malloc(sizeof(node));
  head->next = z;
  
  for(i = 0; i < nMenus; i++) {
    t = (node*) malloc(sizeof(node));
    t->data = *(list + i);
    t->next = head->next;
    head->next = t;
    sp++;
    while(head->next != z) {
      t = head->next;
      head->next = t->next;
      m = (Menu*) t->data;
      free(t);
      /* printf("stack count: %d\n", --sp); */
      /* printf("visiting %s\n", m->szName); */
      /* visit m */

      /* TODO(0): put the color defaults somewhere else */

      m->display.fgColor[0] = 0.0;
      m->display.fgColor[1] = 0.0;
      m->display.fgColor[2] = 0.0;
      m->display.fgColor[3] = 1.0;
      m->display.hlColor[0] = 255.0 / 255.0;
      m->display.hlColor[1] = 20.0 / 255.0;
      m->display.hlColor[2] = 20.0 / 255.0;
      m->display.hlColor[3] = 1.0;

      /* push all of m's submenus */
      for(j = 0; j < m->nEntries; j++) {
	t = (node*) malloc(sizeof(node));
	t->data = *(m->pEntries + j);
	t->next = head->next;
	head->next = t;
	/* printf("pushing %s\n", ((Menu*)t->data)->szName); */
	/* printf("stack count: %d\n", ++sp); */
	
      }
    }
  }
  return list;
}

void drawMenu(gDisplay *d) {
  /* draw Menu pCurrent */
  int i;
  int x, y, size, lineheight;

  rasonly(d);

  x = d->vp_w / MENU_TEXT_START_X;
  size = d->vp_w / MENU_TEXT_SCREEN_SIZE;
  y = 2 * d->vp_h / MENU_TEXT_START_Y;
  lineheight = size * MENU_TEXT_LINEHEIGHT;

  /* draw the entries */
  for(i = 0; i < pCurrent->nEntries; i++) {
    if(i == pCurrent->iHighlight)
      glColor4fv(pCurrent->display.hlColor);
    else glColor4fv(pCurrent->display.fgColor);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    rasonly(d);
    drawText(x, y, size,
	     ((Menu*)*(pCurrent->pEntries + i))->display.szCaption);
    y -= lineheight;
  }
}


















