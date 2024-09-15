#include "system.h"
#include "callbacks.h"

callbacks *last_callback = NULL;
callbacks *current_callback = NULL;

void exitCallback(callbacks *cb) {
  if(cb != NULL)
    if(cb->exit != NULL)
    (cb->exit)(); /* give them the chance to quit */
}

void initCallback(callbacks *cb) {
  if(cb->init != NULL)
    (cb->init)();
  if(cb->initGL != NULL)
    (cb->initGL)();
}


void switchCallbacks(callbacks *new) {
	// if(current_callback)
	// fprintf(stderr, "callbacks: exiting %s\n", current_callback->name);
  exitCallback(current_callback);
  SystemRegisterCallbacks(new);
	// fprintf(stderr, "callbacks: initializing %s\n", new->name);
  initCallback(new);

  last_callback = current_callback;
  current_callback = new;
}
  
void updateCallbacks() {
  /* called when the window is recreated */
  exitCallback(current_callback);
  SystemRegisterCallbacks(current_callback);
  initCallback(current_callback);
}

void restoreCallbacks() {
  if(last_callback == NULL) {
    fprintf(stderr, "fatal: no last callback present, exiting\n");
    exit(1); // OK: programmer error, critical
  }

  exitCallback(last_callback);
  current_callback = last_callback;
  SystemRegisterCallbacks(current_callback);
  initCallback(current_callback);
}

#define N_CALLBACKS 7
callbacks *callbackList[N_CALLBACKS] = {
	&gameCallbacks, &guiCallbacks, &pauseCallbacks, &configureCallbacks,
	&promptCallbacks, &creditsCallbacks, &timedemoCallbacks
};

void setCallback(const char *name) {
	int i;

	for(i = 0; i < N_CALLBACKS; i++) {
		if(strcmp( callbackList[i]->name, name ) == 0)
			break;
	}
	if(i == N_CALLBACKS) {
		fprintf(stderr, "fatal: no callback named '%s' found\n");
		exit(1); // OK: programmer error, critical
	}
	switchCallbacks(callbackList[i]);
}


