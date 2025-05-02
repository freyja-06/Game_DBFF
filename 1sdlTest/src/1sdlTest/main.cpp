#include "Game.hpp" 
#include <SDL_ttf.h>
#include <iostream>
#include "AudioManager.h"

int main(int argc, char* args[]) {
	const int SCREEN_WIDTH = 1026; 
	const int SCREEN_HEIGHT = 578; 
	const char* WINDOW_TITLE = "Dragon ball";

	if (TTF_Init() == -1) {
		std::cout << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
	}
	else {
		std::cout << "SDL_ttf initialized!" << std::endl;
	}

	if (!AudioManager::Init()) {
		std::cerr << "Audio Manager failed to initialize!" << std::endl;
	}


	Game* game =  new Game();

	if (game) {
		game->init(WINDOW_TITLE,
				   SDL_WINDOWPOS_CENTERED, 
				   SDL_WINDOWPOS_CENTERED, 
				   SCREEN_WIDTH,           
				   SCREEN_HEIGHT,          
				   false);                 

		const int FPS = 180;
		const int FRAME_DELAY = 1000 / FPS; 
		Uint32 frameStart;
		int frameTime;

		std::cout << "Bắt đầu vòng lặp Game..." << std::endl;

		
		while (game->running()) {

			frameStart = SDL_GetTicks(); 

			game->handleEvents(); 
			game->update();   
			game->render(); 

			
			frameTime = SDL_GetTicks() - frameStart;

			if (FRAME_DELAY > frameTime) {
				SDL_Delay(FRAME_DELAY - frameTime);
			}
		}

		std::cout << "Kết thúc vòng lặp Game." << std::endl;

		game->clean();
		delete game;
		game = nullptr;
		AudioManager::CleanUp();
	}
	else {
		std::cerr << "Lỗi: Không thể tạo đối tượng Game!" << std::endl;
		return 1; 
	}

	return 0;
}