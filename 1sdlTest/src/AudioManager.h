#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL_mixer.h>
#include <string>
#include <map>
#include <iostream> 

class AudioManager {
public:
	static bool Init();

	static bool LoadMusic(const std::string& id, const std::string& path);
	static bool LoadChunk(const std::string& id, const std::string& path);

	static void PlayMusic(const std::string& id, int loops = -1);
	static void PauseMusic();
	static void ResumeMusic();
	static void StopMusic();
	static void SetMusicVolume(int volume);

	static int PlayChunk(const std::string& id, int channel = -1, int loops = 0);
	static void SetSFXVolume(int volume);

	static void CleanUp();

private:
	AudioManager() {}

	static std::map<std::string, Mix_Music*> loadedMusic;
	static std::map<std::string, Mix_Chunk*> loadedChunks;
};

#endif 