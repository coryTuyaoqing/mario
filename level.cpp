#include "level.h"
#include "collider.h"
#include "brick.h"
#include "camera.h"
#include "mario.h"
#include "load_screen.h"
#include <sys/types.h>
#include "dirent.h"
#include <iostream>

void Level::reset()
{
	for (int i = 0; i < MAX_LEVEL_LAYER; i++)
		for (int j = 0; j < MAX_LEVEL_RANGE; j++)
			mp[i][j].clear();
	for (int i = 0; i < MAX_LEVEL_RANGE; i++)
		actors[i].clear();
	for (int i = 0; i < MAX_LEVEL_RANGE; i++)
		unrun_actors[i].clear();
}

void Level::start(const char* path)
{
	reset();
	FILE* fp = fopen(path, "r");
	char s1[50];
	std::string s2;
	while (true) {
		fscanf(fp, "%s", s1);
		s2 = s1;
		if (s2 == "end") break;
		if (s2 == "map_range") {
			fscanf(fp, "%d", &map_range);
			continue;
		}
		if (s2 == "limit_time") {
			fscanf(fp, "%d", &limit_time);
			continue;
		}
	}
	while (true) {
		int x, y, id;
		fscanf(fp, "%d%d", &x, &y);
		if (x < 0 && y < 0) break;
		fscanf(fp, "%d", &id);
		std::string name = gp_type[id];
		if (name == "Brick") {
			Brick* brick = new Brick(fp);
			brick->Collider::setpos(x, y, 1, 1);
			assert(x < MAX_LEVEL_RANGE);
			mp[0][x].push_back(brick);
		}
	}
	Mario* m = new Mario;
	actors[4].push_back(m);
	fclose(fp);
	camera.start();
}

void Level::restart()
{
	start((LEVEL_NAME + ".mio").c_str());
}

void Level::death()
{
	death_sound.Play(0);
	death_time = clock();
	freeze = true;
	main_theme.Stop();
	LIVES--;
	restart();
}

bool Level::update()
{
	if (!isrun) return false;
	if (death_time && clock() - death_time > 3000) {
		death_time = 0;
		level.stop();
		if (LIVES) {
			load_screen.start("begin");
		}
		else {
			load_screen.start("game_over");
		}
		
	}
	if (freeze) return camera.update();
	if (main_theme.GetPlayStatus() == MUSIC_MODE_STOP) {
		main_theme.Play(0);
	}
	for (int i = 0; i < MAX_LEVEL_LAYER; i++) {
		for (Collider* c : level.actors[i]) {
			c->calc();
		}
	}
	last_time = clock();
	return camera.update();
}

void Level::start()
{
	isrun = true;
	camera.start();
	if (!freeze) {
		start_time = clock();
		last_time = clock();
		main_theme.Play(0);
	}
	
}

void Level::stop()
{
	isrun = false;
	main_theme.Stop();
}

bool Level::running()
{
	return isrun;
}

Level::Level()
{
	main_theme.OpenFile("resources\\music\\main_theme.mp3");
	main_theme.SetVolume(0.5);
	death_sound.OpenFile("resources\\music\\death.wav");
}

Level level;