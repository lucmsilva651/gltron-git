#include "game/game.h"
#include "video/video.h"
#include "audio/audio.h"
#include "scripting/nebu_scripting.h"
#include "Nebu_base.h"
#include "game/camera.h"
#include "game/game_level.h"
#include "configuration/settings.h"
#include "game/event.h"
#include "audio/audio.h"
#include "audio/sound_glue.h"

#include "base/nebu_math.h"
#include <assert.h>
#include <string.h>

#include "base/nebu_debug_memory.h"

void Player_GetPosition(Player *pPlayer, float *pX, float *pY)
{
	vec2 v;
	vec2_Add(&v,
		&pPlayer->data->trails[pPlayer->data->trailOffset].vStart,
		&pPlayer->data->trails[pPlayer->data->trailOffset].vDirection);
	*pX = v.v[0];
	*pY = v.v[1];
}

void getPositionFromIndex(float *x, float *y, int player) {
	getPositionFromData(x, y, game->player[player].data);
}

void getPositionFromData(float *x, float *y, Data *data) {
	vec2 v;
	vec2_Add(&v,
		&data->trails[data->trailOffset].vStart,
		&data->trails[data->trailOffset].vDirection);
	*x = v.v[0];
	*y = v.v[1];
}

void initGameLevel(void) {
	if(game2->level)
		game_FreeLevel(game2->level);
	game2->level = game_CreateLevel();

	if(game2->level->scalable)
		game_ScaleLevel(game2->level, getSettingf("grid_size"));
}

void game_CreatePlayers(int players, Game **ppGame, Game2 **ppGame2)
{
	int i;

	// Game
	*ppGame = (Game*) malloc(sizeof(Game));
	(*ppGame)->pauseflag = PAUSE_NO_GAME;
	(*ppGame)->players = players;
	(*ppGame)->running = 0;
	(*ppGame)->winner = -1;
	(*ppGame)->player = (Player *) malloc(players * sizeof(Player));
	for(i = 0; i < players; i++)
	{
		Player *p = (*ppGame)->player + i;
		p->ai = (AI*) malloc(sizeof(AI));
		p->data = (Data*) malloc(sizeof(Data));
		p->data->trails = (segment2*) malloc(MAX_TRAIL * sizeof(segment2));
		p->data->trailOffset = 0;
	}
	// Game2
	*ppGame2 = (Game2*) malloc(sizeof(Game2));
	// TODO: proper member initialization
	memset(*ppGame2, 0, sizeof(Game2));
	(*ppGame2)->mode = GAME_SINGLE;
}

void game_FreePlayers(Game *pGame, Game2 *pGame2)
{
	int i;

	// Game
	for(i = 0; i < pGame->players; i++)
	{
		Player *p = pGame->player + i;
		free(p->ai);
		free(p->data->trails);
		free(p->data);
	}
	free(pGame->player);
	free(pGame);
	// Game2
	if(game2->level)
		game_FreeLevel(game2->level);
	free(pGame2);
}

void resetPlayerData(void) {
	int i;
	Data *data;
	AI *ai;
	int not_playing = 0;

	int *startIndex;
	startIndex = malloc( game->players * sizeof(int) );
	nebu_RandomPermutation(game->players, startIndex);

	for(i = 0; i < game->players; i++) {
		float x, y;

		data = game->player[i].data;
		ai = game->player[i].ai;
		/* init ai */

		switch(i) {
			// TODO: this limits the number of players to 4
		case 0: ai->active = getSettingi("ai_player1"); break;
		case 1: ai->active = getSettingi("ai_player2"); break;
		case 2: ai->active = getSettingi("ai_player3"); break;
		case 3: ai->active = getSettingi("ai_player4"); break;
		default:
			fprintf(stderr, "[error] player index #%d not caught!\n", i);
			ai->active = AI_NONE;
		}
		ai->tdiff = 0;

		/* arrange players in circle around center */

		/* randomize position on the grid */
		x = game2->level->spawnPoints[ startIndex[i] ].v.v[0];
		y = game2->level->spawnPoints[ startIndex[i] ].v.v[1];
		/* randomize starting direction */
		data->dir = game2->level->spawnPoints[ startIndex[i] ].dir;
		if(data->dir == -1) 
			data->dir = nebu_rand() & 3;
		/* data->dir = startdir[i]; */
		data->last_dir = data->dir;

		/* if player is playing... */
		if(ai->active != AI_NONE) {
			data->speed = getSettingf("speed");

			data->energy = 0;
			if(getSettingf("booster_on") || getSettingf("wall_buster_on"))
			{
				data->energy = getSettingf("energy");
			}
			data->boost_enabled = 0;
			data->wall_buster_enabled = 0;

			data->trail_height = TRAIL_HEIGHT;
		} else {
			data->speed = SPEED_GONE;
			data->trail_height = 0;
			not_playing++;
		}
		// data->trail = data->trails;
		data->trailOffset = 0;

		data->trails[ data->trailOffset ].vStart.v[0] = x;
		data->trails[ data->trailOffset ].vStart.v[1] = y;
		
		data->trails[ data->trailOffset ].vDirection.v[0] = 0;
		data->trails[ data->trailOffset ].vDirection.v[1] = 0;

		data->impact_radius = 0.0;
		data->turn_time = 0;
		if(ai->active != AI_NONE) {
			data->exp_radius = 0;
		} else {
			data->exp_radius = EXP_RADIUS_MAX;
		}

		{
			int camType;
			camType = (game->player[i].ai->active == AI_COMPUTER) ? 
				CAM_CIRCLE : gSettingsCache.camType;
			initCamera(&gPlayerVisuals[i].camera, data, camType);
		}
	}

	free(startIndex);

	game->running = game->players - not_playing; /* not everyone is alive */
	/* printf("starting game with %d players\n", game->running); */
	game->winner = -1;
}

void game_ResetData(void) {
	game->pauseflag = PAUSE_GAME_STARTING;

	game2->rules.speed = getSettingf("speed");
	game2->rules.eraseCrashed = getSettingi("erase_crashed");

	/* time management */
	game2->time.lastFrame = 0;
	game2->time.current = 0;
	game2->time.offset = nebu_Time_GetElapsed();
	/* TODO: fix that */
	game2->players = game->players;
	/* event management */
	game2->events.next = NULL;
	/* TODO: free any old events that might have gotten left */

	scripting_Run("console_Clear()");

	resetPlayerData();
}

void Time_Idle(void) {
	game2->time.lastFrame = game2->time.current;
	game2->time.current = nebu_Time_GetElapsed() - game2->time.offset;
	game2->time.dt = game2->time.current - game2->time.lastFrame;
	/* fprintf(stderr, "dt: %d\n", game2->time.dt); */
}

void resetScores(void) {
	int i;
	for(i = 0; i < game->players; i++)
		game->player[i].data->score = 0;
}

void doCrashPlayer(GameEvent *e) {
	int j;

	Audio_CrashPlayer(e->player);
	Audio_StopEngine(e->player);

	for(j = 0; j < game->players; j++) 
		if(j != e->player && PLAYER_IS_ACTIVE(&(game->player[j])))
			game->player[j].data->score++;

	game->player[e->player].data->speed = SPEED_CRASHED;
}

void newTrail(Data* data) {
	segment2 *s;
	float x, y;

	getPositionFromData(&x, &y, data);

	data->trailOffset++;
	s = data->trails + data->trailOffset;

	s->vStart.v[0] = x;
	s->vStart.v[1] = y;
	s->vDirection.v[0] = 0;
	s->vDirection.v[1] = 0;
	
}
      
void doTurn(GameEvent *e, int direction) {
	Data *data = game->player[e->player].data;
	newTrail(data);
	data->last_dir = data->dir;
	data->dir = (data->dir + direction) % 4;
	data->turn_time = game2->time.current;
}
