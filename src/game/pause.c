#include "game/gltron.h"

/* very brief - just the pause mode */

void idlePause(void) {
	Sound_idle();
	game2->time.dt = 0;
	doCameraMovement();
	{
		int dx, dy;
		nebu_Input_Mouse_GetDelta(&dx,&dy);
		if(dx || dy)
            nebu_Input_Mouse_WarpToOrigin();
	}
	scripting_RunGC();
	nebu_Time_FrameDelay(10);
	nebu_System_PostRedisplay();
}

void keyboardPause(int state, int key, int x, int y) {
	if(state == SYSTEM_KEYSTATE_UP)
		return;

	switch(key) {
	case 27:
		nebu_System_ExitLoop(RETURN_PAUSE_ESCAPE);
		break;
	case SYSTEM_KEY_F1: changeDisplay(0); break;
	case SYSTEM_KEY_F2: changeDisplay(1); break;
	case SYSTEM_KEY_F3: changeDisplay(2); break;
	case SYSTEM_KEY_F4: changeDisplay(3); break;

		// somehow, this breaks the 'keys' array, and saving
		// at the end of the game fails
		// case SYSTEM_KEY_F5: saveSettings(); return;

	case SYSTEM_KEY_F10: nextCameraType(); break;

	case SYSTEM_KEY_F11: doBmpScreenShot(gScreen); break;
	case SYSTEM_KEY_F12: doPngScreenShot(gScreen); break;
	    
	case SYSTEM_KEY_UP: console_Seek(-1); break;
	case SYSTEM_KEY_DOWN: console_Seek(1); break;

	case SYSTEM_KEY_TAB: 
		// nebu_System_ExitLoop(RETURN_MENU_PROMPT);
		break;

	default:
		if(game->pauseflag == PAUSE_GAME_FINISHED) {
			game_ResetData();
			video_ResetData();
		}
		else
		{
			game->pauseflag = PAUSE_GAME_RUNNING;
			nebu_System_ExitLoop(RETURN_GAME_UNPAUSE);
		}
		/* lasttime = SystemGetElapsedTime(); */
		break;
	}
}

void initPause(void) {
	SystemHidePointer();
	nebu_Input_Mouse_WarpToOrigin();

	/* disable game sound effects */
	Audio_DisableEngine();

	/* 
	* TODO: Provide an option to disable game music here. 
	* Game should be totally silent in pause mode. (Nice when 
	* the boss is walking by, phone call, etc...)
	*/

	updateSettingsCache();
}

void exitPause(void) {
}

Callbacks pauseCallbacks = {
	displayGame, idlePause, keyboardPause,
	initPause, exitPause, gameMouse, NULL, "pause"
};

void keyboardPrompt(int state, int key, int x, int y) {
	if(state == SYSTEM_KEYSTATE_UP)
		return;

	switch(key) {
	case 27:
	case SYSTEM_KEY_TAB:
		nebu_System_ExitLoop(RETURN_PAUSE_ESCAPE);
		break;
	case SYSTEM_KEY_RETURN:
		/* promptEvaluate(); */
		break;
	}
}

void initPrompt(void) { }
void exitPrompt(void) { }

Callbacks promptCallbacks = {
	displayGame, idlePause, keyboardPrompt,
	initPrompt, exitPrompt, NULL /* mouse button */, NULL /* mouse motion */, 
	"prompt"
};




