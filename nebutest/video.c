#include "video/nebu_video_system.h"
#include "video/nebu_renderer_gl.h"
#include "base/nebu_system.h"

void idle() {
	static int count = 0;
	nebu_System_Sleep(10);
	nebu_System_PostRedisplay();
	if((count++) == 200)
		nebu_System_ExitLoop(0);
	
}

void display() {
	glClearColor(0, .3f, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	nebu_System_SwapBuffers();
}