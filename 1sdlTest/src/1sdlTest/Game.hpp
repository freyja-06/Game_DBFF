#ifndef GAME_HPP
#define GAME_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <SDL_ttf.h> 
#include <iostream> 
#include <random>   
#include <string>
#include <fstream>
#include <sstream> 



class Manager;
class ColliderComponent;
class Entity;

class SpriteComponent;
class TransformComponent;

enum class GameState {
	MainMenu,
	Playing,
	Paused,
	GameOver,
	Exiting
};

class Game {
public:
	Game();  
	~Game();
	
	void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

	void handleEvents();

	void update();

	void render();

	void clean();

	bool running() const { return isRunning; }

	static SDL_Renderer* renderer; 
	static SDL_Event event; 
	static Manager manager;   
	GameState getCurrentState() const { return currentState; }

private:
	GameState currentState = GameState::MainMenu;

	Entity* mainMenuBackground = nullptr;
	Entity* mainMenuNewGameButton = nullptr;
	Entity* mainMenuExitButton = nullptr;

	Entity* pauseButton = nullptr; 
	Entity* pauseMenuBackground = nullptr;
	Entity* pauseContinueButton = nullptr;
	Entity* pauseNewGameButton = nullptr;
	Entity* pauseExitButton = nullptr;

	Entity* gameOverText = nullptr; 
	Entity* gameOverRestartButton = nullptr;
	Entity* gameOverMenuButton = nullptr;
	Entity* gameBackground = nullptr;

	TTF_Font* gameFont = nullptr; 
	SDL_Color textColor = { 255, 255, 255, 255 };
	SDL_Color highScoreColor = { 255, 215, 0, 255 };
	SDL_Color HSColorMenu = { 255, 63, 0, 255 }; 
	SDL_Color gameOverColor = { 255, 0, 0, 255 };

	void CreateMainMenu();
	void DestroyMainMenu();

	void CreatePlayingUI();
	void DestroyPlayingUI();

	void CreatePauseMenu();
	void DestroyPauseMenu();

	void CreateGameOverMenu();
	void DestroyGameOverMenu();

	void HandleMouseClick(int mouseX, int mouseY);

	void StartNewGame();
	void PauseGame();
	void ResumeGame();
	void ExitGame();
	void GoToMainMenu();
	void ResetGame();
	void loadAssets();
	void LoadHighScore();
	void UpdateScoreDisplay();
	void UpdateHighScoreDisplay();
	void SaveHighScore();

	void SpawnStar();	
	float starSpawnTimer = 0.0f;
	float starSpawnInterval = 2.0f;
	std::uniform_int_distribution<int> starPosDist;
	int currentScore = 0;
	int highScore = 0;
	const std::string highScoreFile = "highscore.txt";

	Entity* scoreTextEntity = nullptr;
	Entity* highScoreTextEntity = nullptr;
	Entity* menuHighScoreTextEntity = nullptr; 

	
	void SpawnFireball();
	float fireballSpawnTimer = 0.0f;
	float fireballSpawnInterval = 0.75f;
	std::mt19937 rng; 
	std::uniform_int_distribution<int> fireballPosDist;

	bool isRunning = false;     
	SDL_Window* window = nullptr; 

	Uint32 lastFrameTime = 0; 
	float deltaTime = 0.0f; 

	static const int SCREEN_WIDTH = 1026;
	static const int SCREEN_HEIGHT = 578;

	Entity* player = nullptr; 

	
};

#endif 