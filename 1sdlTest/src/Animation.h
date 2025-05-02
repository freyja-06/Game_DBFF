#pragma once

struct Animation {
	int index = 0;  
	int frames = 1; 
	int speed = 100;
	bool loop = true; 

	Animation() = default;
	Animation(int i, int f, int s) : index(i), frames(f), speed(s), loop(true) {}
	Animation(int i, int f, int s, bool l) : index(i), frames(f), speed(s), loop(l) {}
};