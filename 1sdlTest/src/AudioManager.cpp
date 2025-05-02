#include "AudioManager.h"

std::map<std::string, Mix_Music*> AudioManager::loadedMusic;
std::map<std::string, Mix_Chunk*> AudioManager::loadedChunks;

bool AudioManager::Init() {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
		return false;
	}
	std::cout << "SDL_mixer initialized!" << std::endl;
	return true;
}

bool AudioManager::LoadMusic(const std::string& id, const std::string& path) {
	Mix_Music* music = Mix_LoadMUS(path.c_str());
	if (!music) {
		std::cerr << "Failed to load music '" << id << "' at path: " << path << "! Mix_Error: " << Mix_GetError() << std::endl;
		return false;
	}
	if (loadedMusic.count(id) && loadedMusic[id]) {
		Mix_FreeMusic(loadedMusic[id]);
	}
	loadedMusic[id] = music;
	std::cout << "Loaded Music ID: '" << id << "' from " << path << std::endl;
	return true;
}

bool AudioManager::LoadChunk(const std::string& id, const std::string& path) {
	Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
	if (!chunk) {
		std::cerr << "Failed to load chunk '" << id << "' at path: " << path << "! Mix_Error: " << Mix_GetError() << std::endl;
		return false;
	}
	if (loadedChunks.count(id) && loadedChunks[id]) {
		Mix_FreeChunk(loadedChunks[id]);
	}
	loadedChunks[id] = chunk;
	std::cout << "Loaded Chunk ID: '" << id << "' from " << path << std::endl;
	return true;
}

void AudioManager::PlayMusic(const std::string& id, int loops) {
	if (loadedMusic.count(id) && loadedMusic[id]) {
		if (Mix_PlayMusic(loadedMusic[id], loops) == -1) {
			std::cerr << "Mix_PlayMusic Error for ID '" << id << "': " << Mix_GetError() << std::endl;
		}
	}
	else {
		std::cerr << "Error: Music with ID '" << id << "' not found or not loaded." << std::endl;
	}
}

void AudioManager::PauseMusic() {
	if (Mix_PlayingMusic()) { 
		Mix_PauseMusic();
	}
}

void AudioManager::ResumeMusic() {
	if (Mix_PausedMusic()) {
		Mix_ResumeMusic();
	}
}

void AudioManager::StopMusic() {
	Mix_HaltMusic();
}

void AudioManager::SetMusicVolume(int volume) {
	if (volume < 0) volume = 0;
	if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME; 
	Mix_VolumeMusic(volume);
}

int AudioManager::PlayChunk(const std::string& id, int channel, int loops) {
	if (loadedChunks.count(id) && loadedChunks[id]) {
		int playedChannel = Mix_PlayChannel(channel, loadedChunks[id], loops);
		if (playedChannel == -1) {
			std::cerr << "Mix_PlayChannel Error for ID '" << id << "': " << Mix_GetError() << std::endl;
		}
		return playedChannel;
	}
	else {
		std::cerr << "Error: Chunk with ID '" << id << "' not found or not loaded." << std::endl;
		return -1;
	}
}

void AudioManager::SetSFXVolume(int volume) {
	if (volume < 0) volume = 0;
	if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
	Mix_Volume(-1, volume);
}



void AudioManager::CleanUp() {
	std::cout << "Cleaning up AudioManager..." << std::endl;
	for (auto it = loadedMusic.begin(); it != loadedMusic.end(); ++it) {
		if (it->second) {
			Mix_FreeMusic(it->second);
		}
	}
	loadedMusic.clear();
	for (auto it = loadedChunks.begin(); it != loadedChunks.end(); ++it) {
		if (it->second) {
			Mix_FreeChunk(it->second);
		}
	}
	loadedChunks.clear();
	Mix_CloseAudio();
	std::cout << "AudioManager cleaned up." << std::endl;
}