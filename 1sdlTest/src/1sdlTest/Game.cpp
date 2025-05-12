#include "Game.hpp"
#include "ECS.h"
#include "Components.h"
#include "TextureManager.h" 
#include "AudioManager.h"
#include "Collision.h"
#include <functional> 
#include <random>
#include <algorithm> 
#include <iostream> 
#include <SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <iomanip>

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
Manager Game::manager;

SDL_Renderer* SpriteComponent::renderer = nullptr;

Game::Game() : rng(std::random_device{}()) {
}

Game::~Game() {
	
}

void Game::loadAssets() {
	std::cout << "Loading assets..." << std::endl;
	std::cout << "Loading game theme..." << std::endl;
	bool themeLoaded = AudioManager::LoadMusic("theme", "asset/audio/MusicTheme.wav");
	if (!themeLoaded) std::cerr << "----> FAILED TO LOAD GAME THEME <----" << std::endl;

	std::cout << "Loading menu theme..." << std::endl;
	bool menuThemeLoaded = AudioManager::LoadMusic("menu_theme", "asset/audio/menu_theme.wav"); 
	if (!menuThemeLoaded) std::cerr << "----> FAILED TO LOAD MENU THEME <----" << std::endl;

	std::cout << "Loading jump sfx..." << std::endl;
	bool jumpLoaded = AudioManager::LoadChunk("jump", "asset/audio/Jump.wav"); 
	if (!jumpLoaded) std::cerr << "----> FAILED TO LOAD JUMP SFX <----" << std::endl;

	std::cout << "Loading hit sfx..." << std::endl;
	bool hitLoaded = AudioManager::LoadChunk("hit", "asset/audio/player_hit.wav");
	if (!hitLoaded) std::cerr << "----> FAILED TO LOAD HIT SFX <----" << std::endl;

	bool pick = AudioManager::LoadChunk("pickup", "asset/audio/pickup.wav"); 
	if (!pick) std::cerr << "----> FAILED TO LOAD PICKUP SFX <----" << std::endl;
	


	AudioManager::SetMusicVolume(64); 
	AudioManager::SetSFXVolume(96); 

	std::cout << "Asset loading finished." << std::endl;
}


void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int windowFlags = SDL_WINDOW_SHOWN; 
	if (fullscreen) {
		windowFlags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL không thể khởi tạo! SDL_Error: " << SDL_GetError() << std::endl;
		return;
	}
	std::cout << "SDL Subsystems Initialized!" << std::endl;

	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cerr << "IMG_Error: " << IMG_GetError() << std::endl;
		SDL_Quit();
		return; 
	}
	std::cout << "SDL_image Initialized!" << std::endl;

	window = SDL_CreateWindow(title, xpos, ypos, width, height, windowFlags);
	if (!window) {
		std::cerr << "SDL_Error: Cannot create window" << SDL_GetError() << std::endl;
		IMG_Quit();
		SDL_Quit();
		return;
	}
	std::cout << "Cửa sổ đã được tạo." << std::endl;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		std::cerr << "Không thể tạo renderer! SDL Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		IMG_Quit();
		SDL_Quit();
		return;
	}
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	std::cout << "Renderer đã được tạo." << std::endl;

	
	gameFont = TTF_OpenFont("asset/vsou.ttf", 30); 
	if (!gameFont) {
		std::cerr << "Failed to load font 'asset/vsou.ttf'! TTF_Error: " << TTF_GetError() << std::endl;
	}
	else {
		std::cout << "Font 'asset/vsou.ttf' loaded successfully." << std::endl;
	}

	Game::renderer = renderer;
	SpriteComponent::renderer = renderer;

	fireballPosDist = std::uniform_int_distribution<int>(30, SCREEN_WIDTH - 60);
	starPosDist = std::uniform_int_distribution<int>(50, SCREEN_WIDTH - 80); 


	LoadHighScore();
	loadAssets();

	std::cout << "Đang tạo Entities..." << std::endl;

	CreateMainMenu();
	currentState = GameState::MainMenu;
	std::cout << "[INIT] Attempting to play menu theme..." << std::endl;
	AudioManager::PlayMusic("menu_theme", -1);
	isRunning = true;
	lastFrameTime = SDL_GetTicks();
	std::cout << "Game Initialized Successfully." << std::endl;

}

void Game::handleEvents() {
	Entity* playerPtr = nullptr;
	auto& entities = manager.getEntities(); 
	for (auto& ent : entities) {
		if (ent->hasComponent<KeyboardController>()) {
			playerPtr = ent.get();
			break;
		}
	}

	KeyboardController* playerKeyboardController = nullptr;
	if (playerPtr && playerPtr->isActive()) {
		playerKeyboardController = &playerPtr->getComponent<KeyboardController>();
	}
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) { 
			isRunning = false;
		}

		if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				HandleMouseClick(event.button.x, event.button.y);
			}
		}

		switch (currentState) {
		case GameState::Playing:
		case GameState::Paused:
		{
			Entity* playerPtr = nullptr;
			for (auto& ent : manager.getEntities()) {
				if (ent->hasComponent<KeyboardController>()) {
					playerPtr = ent.get(); break;
				}
			}
			KeyboardController* playerKeyboardController = nullptr;
			if (playerPtr && playerPtr->isActive()) {
				playerKeyboardController = &playerPtr->getComponent<KeyboardController>();
			}

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					if (currentState == GameState::Playing) {
						PauseGame();
					}
					else if (currentState == GameState::Paused) {
						ResumeGame();
					}
					break;
				case SDLK_SPACE:
					if (currentState == GameState::Playing && playerKeyboardController) {
						playerKeyboardController->TryJump();
					}
					break;
				case SDLK_p:
					if (currentState == GameState::Playing) {
						PauseGame();
					}
					else if (currentState == GameState::Paused) {
						ResumeGame();
					}
					break;
				}
			}
		}
		break;
		case GameState::GameOver:
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_RETURN) {
					StartNewGame();
				}
				else if (event.key.keysym.sym == SDLK_m) {
					GoToMainMenu();
				}
			}
			break;
		case GameState::MainMenu:
			break;
		default:
			break;

		}
	}
}


void Game::update() {
	if (!isRunning) return;

	Uint32 currentFrameTime = SDL_GetTicks();
	deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
	lastFrameTime = currentFrameTime;

	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}
	switch (currentState) {
	case GameState::MainMenu:
		manager.refresh();
		manager.update(deltaTime);
		break;

	case GameState::Paused:
		manager.refresh();
		if (pauseMenuBackground && pauseMenuBackground->isActive()) {
			pauseMenuBackground->update(deltaTime);
		}
		if (pauseContinueButton && pauseContinueButton->isActive()) {
			pauseContinueButton->update(deltaTime);
		}
		if (pauseNewGameButton && pauseNewGameButton->isActive()) {
			pauseNewGameButton->update(deltaTime);
		}
		if (pauseExitButton && pauseExitButton->isActive()) {
			pauseExitButton->update(deltaTime);
		}
		break;

	case GameState::Playing:
		manager.refresh();
		manager.update(deltaTime);

		fireballSpawnTimer += deltaTime;
		if (fireballSpawnTimer >= fireballSpawnInterval) {
			SpawnFireball();
			fireballSpawnTimer = 0.0f;
		}

		starSpawnTimer += deltaTime;
		if (starSpawnTimer >= starSpawnInterval) {
			SpawnStar();
			starSpawnTimer = 0.0f;
		}

		if (player && player->isActive() && player->hasComponent<ColliderComponent>()) {
			auto& pCollider = player->getComponent<ColliderComponent>();
			auto& pTransform = player->getComponent<TransformComponent>();

			for (auto& entity : manager.getEntities()) {
				if (entity->isActive() && entity->hasComponent<ColliderComponent>() && entity->getComponent<ColliderComponent>().tag == "fireball") {
					auto& fireballCollider = entity->getComponent<ColliderComponent>();

					if (Collision::AABB(pCollider, fireballCollider)) {
						std::cout << "Player hit by fireball!" << std::endl;
						currentState = GameState::GameOver;

						pTransform.velocity.Zero();
						pTransform.isGrounded = true;
						if (player->hasComponent<SpriteComponent>()) {
							auto& playerSprite = player->getComponent<SpriteComponent>();
							if (playerSprite.ChangeTexture("asset/Player_idle2.png", Game::renderer, true)) {
								Animation hitAnim = Animation(0, 7, 150, false);
								playerSprite.animation.clear(); 
								playerSprite.animation.emplace("Hit", hitAnim);
								playerSprite.Play("Hit");
								std::cout << "Changed to Hit animation." << std::endl;
							}
							else {
								std::cerr << "Failed to change player texture to Player_idle2.png" << std::endl;
							}
						}
						if (player->hasComponent<KeyboardController>()) {
							player->getComponent<KeyboardController>().enabled = false;
						}

						AudioManager::PlayChunk("hit");
						entity->destroy();
						DestroyPlayingUI();
						CreateGameOverMenu(); 

						if (currentScore > highScore) {
							highScore = currentScore;
							SaveHighScore();
							UpdateHighScoreDisplay();
						}
						goto end_collision_check_playing;
					}
				}
				else if (entity->hasComponent<ColliderComponent>() && entity->getComponent<ColliderComponent>().tag == "star") {
					auto& starCollider = entity->getComponent<ColliderComponent>();
					if (Collision::AABB(pCollider, starCollider)) {
						std::cout << "Player picked up star!" << std::endl;
						currentScore += 10; 
						UpdateScoreDisplay();
						AudioManager::PlayChunk("pickup");
						entity->destroy();
					}
				}

				if (entity->hasComponent<TransformComponent>() && entity->hasComponent<ColliderComponent>()) {
					const std::string& tag = entity->getComponent<ColliderComponent>().tag;
					if (tag == "fireball" || tag == "star") {
						if (entity->getComponent<TransformComponent>().position.y > SCREEN_HEIGHT + 50 ||
							entity->getComponent<TransformComponent>().position.x < -50 ||
							entity->getComponent<TransformComponent>().position.x > SCREEN_WIDTH + 50)
						{
							entity->destroy();
						}
					}
				}
			}
		}
		if (player && player->isActive() && player->hasComponent<TransformComponent>() && player->hasComponent<ColliderComponent>()) {
			auto& playerTransform = player->getComponent<TransformComponent>();
			auto& pCollider = player->getComponent<ColliderComponent>();
			playerTransform.isGrounded = false;

			for (auto& entity : manager.getEntities()) {
				if (entity.get() != player && entity->isActive() && entity->hasComponent<ColliderComponent>()) {
					auto& otherCollider = entity->getComponent<ColliderComponent>();
					if (otherCollider.tag == "ground" || otherCollider.tag == "wall") {
						if (Collision::AABB(pCollider, otherCollider)) {
							SDL_Rect playerRect = pCollider.collider;
							SDL_Rect otherRect = otherCollider.collider;
							auto& otherTransform = otherCollider.entity->getComponent<TransformComponent>();

							float playerCenterX = playerTransform.position.x + pCollider.offsetX + playerRect.w / 2.0f;
							float playerCenterY = playerTransform.position.y + pCollider.offsetY + playerRect.h / 2.0f;
							float otherCenterX = otherTransform.position.x + otherCollider.offsetX + otherRect.w / 2.0f;
							float otherCenterY = otherTransform.position.y + otherCollider.offsetY + otherRect.h / 2.0f;

							float diffX = playerCenterX - otherCenterX;
							float diffY = playerCenterY - otherCenterY;
							float combinedHalfWidths = playerRect.w / 2.0f + otherRect.w / 2.0f;
							float combinedHalfHeights = playerRect.h / 2.0f + otherRect.h / 2.0f;
							float overlapX = combinedHalfWidths - std::abs(diffX);
							float overlapY = combinedHalfHeights - std::abs(diffY);

							if (overlapX > 0 && overlapY > 0) {
								if (overlapY < overlapX) {
									if (diffY < 0 && playerTransform.velocity.y >= 0) { 
										playerTransform.isGrounded = true;
										playerTransform.velocity.y = 0;
										playerTransform.setY(otherTransform.position.y + otherCollider.offsetY - playerRect.h);
									}
									else if (diffY > 0 && playerTransform.velocity.y < 0) {
										playerTransform.velocity.y = 0;
										playerTransform.setY(otherTransform.position.y + otherCollider.offsetY + otherRect.h);
									}
									pCollider.collider.y = static_cast<int>(playerTransform.position.y + pCollider.offsetY);
								}
								else {
									if (diffX < 0) {
										playerTransform.velocity.x = 0;
										playerTransform.setX(otherTransform.position.x + otherCollider.offsetX - playerRect.w);
									}
									else {
										playerTransform.velocity.x = 0;
										playerTransform.setX(otherTransform.position.x + otherCollider.offsetX + otherRect.w);
									}
									pCollider.collider.x = static_cast<int>(playerTransform.position.x + pCollider.offsetX);
								}
							}
						}
					}
				}
			}
			float playerWidth = pCollider.collider.w;
			float playerLeftEdge = playerTransform.position.x + pCollider.offsetX;
			float playerRightEdge = playerLeftEdge + playerWidth;
			if (playerLeftEdge < 0) {
				playerTransform.setX(0 - pCollider.offsetX);
				if (playerTransform.velocity.x < 0) playerTransform.velocity.x = 0;
				pCollider.collider.x = 0;
			}
			else if (playerRightEdge > SCREEN_WIDTH) {
				playerTransform.setX(SCREEN_WIDTH - playerWidth - pCollider.offsetX);
				if (playerTransform.velocity.x > 0) playerTransform.velocity.x = 0;
				pCollider.collider.x = static_cast<int>(playerTransform.position.x + pCollider.offsetX);
			}

			if (player->hasComponent<SpriteComponent>()) {
				player->getComponent<SpriteComponent>().UpdateAnimationState();
			}
		}
		end_collision_check_playing:;
		break;

	case GameState::GameOver:
		manager.refresh();
		if (player && player->isActive()) player->update(deltaTime);
		if (gameOverText && gameOverText->isActive()) gameOverText->update(deltaTime);
		if (gameOverRestartButton && gameOverRestartButton->isActive()) gameOverRestartButton->update(deltaTime);
		if (gameOverMenuButton && gameOverMenuButton->isActive()) gameOverMenuButton->update(deltaTime);
		break;

	case GameState::Exiting:
		isRunning = false;
		break;
	}
}



void Game::render() {
	if (!renderer) return; 

	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	SDL_RenderClear(renderer);

	switch (currentState) {
	case GameState::MainMenu:
		manager.draw();
		break;

	case GameState::Playing:
		if (gameBackground && gameBackground->isActive()) {
			gameBackground->draw();
		}
		for (auto& entity : manager.getEntities()) {
			if (entity.get() != gameBackground && entity->isActive()) {
				bool isPauseUI = (entity.get() == pauseButton); 
				if (!isPauseUI) {
					entity->draw();
				}
			}
		}
		if (pauseButton && pauseButton->isActive()) {
			pauseButton->draw();
		}
		break;

	case GameState::Paused:

		if (gameBackground && gameBackground->isActive()) {
			gameBackground->draw();
		}

		for (auto& entity : manager.getEntities()) {
			if (entity.get() != gameBackground && entity->isActive()) {
				bool isPauseMenuUI = (entity.get() == pauseMenuBackground ||
										 entity.get() == pauseContinueButton ||
										 entity.get() == pauseNewGameButton ||
										 entity.get() == pauseExitButton ||
										 entity.get() == pauseButton);
				if (!isPauseMenuUI) {
					entity->draw();
				}
			}
		}

		if (pauseMenuBackground && pauseMenuBackground->isActive()) pauseMenuBackground->draw();
		if (pauseContinueButton && pauseContinueButton->isActive()) pauseContinueButton->draw();
		if (pauseNewGameButton && pauseNewGameButton->isActive()) pauseNewGameButton->draw();
		if (pauseExitButton && pauseExitButton->isActive()) pauseExitButton->draw();
		break;



		for (auto& entity : manager.getEntities()) {
			bool isPauseUI = (entity.get() == pauseMenuBackground ||
									 entity.get() == pauseContinueButton ||
									 entity.get() == pauseNewGameButton ||
									 entity.get() == pauseExitButton);

			if (!isPauseUI && entity->isActive()) {
				entity->draw();
			}
		}

		if (pauseMenuBackground && pauseMenuBackground->isActive()) pauseMenuBackground->draw();
		if (pauseContinueButton && pauseContinueButton->isActive()) pauseContinueButton->draw();
		if (pauseNewGameButton && pauseNewGameButton->isActive()) pauseNewGameButton->draw();
		if (pauseExitButton && pauseExitButton->isActive()) pauseExitButton->draw();
		break;

	case GameState::GameOver:

		if (gameBackground && gameBackground->isActive()) {
			gameBackground->draw();
		}
		for (auto& entity : manager.getEntities()) {
			if (entity.get() != gameBackground && entity->isActive()) {
				bool isGameOverUI = (entity.get() == gameOverText ||
									 entity.get() == gameOverRestartButton ||
									 entity.get() == gameOverMenuButton);
				bool isFireball = (entity->hasComponent<ColliderComponent>() && entity->getComponent<ColliderComponent>().tag == "fireball");

				if (!isGameOverUI && !isFireball) {
					entity->draw();
				}
			}
		}

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
		SDL_Rect overlayRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_RenderFillRect(renderer, &overlayRect);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);



		if (gameOverText && gameOverText->isActive()) {
			gameOverText->draw();
		}
		if (gameOverRestartButton && gameOverRestartButton->isActive()) {
			gameOverRestartButton->draw();
		}
		if (gameOverMenuButton && gameOverMenuButton->isActive()) {
			gameOverMenuButton->draw();
		}
		break;
	}

	bool drawDebugColliders = false;
	if (drawDebugColliders) {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
		for (auto& entity : manager.getEntities()) {
			if (entity->isActive() && entity->hasComponent<ColliderComponent>()) {
				SDL_RenderDrawRect(renderer, &entity->getComponent<ColliderComponent>().collider);
			}
		}
	}

	SDL_RenderPresent(renderer);
}


void Game::HandleMouseClick(int mouseX, int mouseY) {
	SDL_Point mousePoint = { mouseX, mouseY };
	std::vector<Entity*> currentButtons;
	if (currentState == GameState::MainMenu) {
		if (mainMenuNewGameButton) currentButtons.push_back(mainMenuNewGameButton);
		if (mainMenuExitButton) currentButtons.push_back(mainMenuExitButton);
	}
	else if (currentState == GameState::Paused) {
		if (pauseContinueButton) currentButtons.push_back(pauseContinueButton);
		if (pauseNewGameButton) currentButtons.push_back(pauseNewGameButton);
		if (pauseExitButton) currentButtons.push_back(pauseExitButton);
	}
	else if (currentState == GameState::Playing) {
		if (pauseButton) currentButtons.push_back(pauseButton); 
	}
	else if (currentState == GameState::GameOver) {
		if (gameOverRestartButton) currentButtons.push_back(gameOverRestartButton);
		if (gameOverMenuButton) currentButtons.push_back(gameOverMenuButton);
	}
	for (Entity* buttonEntity : currentButtons) {
		if (buttonEntity && buttonEntity->isActive() && buttonEntity->hasComponent<ButtonComponent>() && buttonEntity->hasComponent<TransformComponent>()) {
			auto& button = buttonEntity->getComponent<ButtonComponent>();
			auto& transform = buttonEntity->getComponent<TransformComponent>();
			SDL_Rect buttonRect = {
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				transform.width * transform.scale,
				transform.height * transform.scale
			};

			if (Collision::PointInRect(mousePoint, buttonRect)) {
				button.TriggerClick();
				break;
			}
		}
	}
}

void Game::CreateMainMenu() {
	std::cout << "Creating Main Menu..." << std::endl;
	auto& bg = manager.addEntity();
	
	bg.addComponent<TransformComponent>(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
	
	bg.addComponent<SpriteComponent>("asset/Menu_background.jpg", false, Game::renderer);
	mainMenuBackground = &bg;
	bg.getComponent<TransformComponent>().isGrounded = true;

	
	auto& ngButton = manager.addEntity();
	
	int ngButtonW = 142; 
	int ngButtonH = 59; 
	
	float ngButtonX = (SCREEN_WIDTH / 2.0f) - (ngButtonW / 2.0f);
	float ngButtonY = 250; 
	ngButton.addComponent<TransformComponent>(ngButtonX, ngButtonY, ngButtonW, ngButtonH, 1);
	ngButton.addComponent<SpriteComponent>("asset/NewGame_button.png", false, Game::renderer);
	
	ngButton.addComponent<ButtonComponent>([this]() { this->StartNewGame(); });
	
	mainMenuNewGameButton = &ngButton;
	ngButton.getComponent<TransformComponent>().isGrounded = true;

	
	auto& exButton = manager.addEntity();
	int exButtonW = 142; 
	int exButtonH = 59; 
	float exButtonX = (SCREEN_WIDTH / 2.0f) - (exButtonW / 2.0f);
	float exButtonY = 320; 
	exButton.addComponent<TransformComponent>(exButtonX, exButtonY, exButtonW, exButtonH, 1);
	exButton.addComponent<SpriteComponent>("asset/Exit_button.png", false, Game::renderer);
	exButton.addComponent<ButtonComponent>([this]() { this->ExitGame(); });
	exButton.getComponent<TransformComponent>().isGrounded = true;
	mainMenuExitButton = &exButton;


	if (gameFont) {
		auto& hsTextEntity = manager.addEntity();
		float hsTextX = (SCREEN_WIDTH / 2.0f);
		float hsTextY = 390.0f;
		hsTextEntity.addComponent<TransformComponent>(hsTextX, hsTextY, 0, 0, 1);
		hsTextEntity.getComponent<TransformComponent>().isGrounded = true;
		std::stringstream ss;
		ss << "High Score: " << std::setw(5) << std::setfill('0') << highScore;
		auto& hsTextComp = hsTextEntity.addComponent<TextComponent>(gameFont, ss.str(), HSColorMenu); 

		hsTextComp.init();
		hsTextComp.update(0.0f);
		auto& hsTextTransform = hsTextEntity.getComponent<TransformComponent>();
		hsTextTransform.position.x = (SCREEN_WIDTH / 2.0f) - (hsTextTransform.width / 2.0f * hsTextTransform.scale);

		menuHighScoreTextEntity = &hsTextEntity;
	}

}



void Game::DestroyMainMenu() {
	std::cout << "Destroying Main Menu..." << std::endl;
	if (mainMenuBackground) mainMenuBackground->destroy();
	if (mainMenuNewGameButton) mainMenuNewGameButton->destroy();
	if (mainMenuExitButton) mainMenuExitButton->destroy();

	mainMenuBackground = nullptr;
	mainMenuNewGameButton = nullptr;
	mainMenuExitButton = nullptr;
	if (menuHighScoreTextEntity) menuHighScoreTextEntity->destroy();
	menuHighScoreTextEntity = nullptr;

	manager.refresh();
}

void Game::CreateGameOverMenu() {
	std::cout << "Creating Game Over Menu..." << std::endl;

	if (gameFont) { 
		auto& textEntity = manager.addEntity();
		float textX = (SCREEN_WIDTH / 2.0f);
		float textY = 100.0f;
		textEntity.addComponent<TransformComponent>(textX, textY, 0, 0, 1);
		textEntity.getComponent<TransformComponent>().isGrounded = true;
		auto& textComp = textEntity.addComponent<TextComponent>(gameFont, "Game Over", gameOverColor); 
		textEntity.getComponent<TextComponent>().init();
		textEntity.getComponent<TextComponent>().update(0.0f);
		
		auto& textTransform = textEntity.getComponent<TransformComponent>();
		textTransform.position.x = (SCREEN_WIDTH / 2.0f) - (textTransform.width / 2.0f * textTransform.scale); 

		gameOverText = &textEntity; 
		std::cout << "Game Over text entity created." << std::endl;
	}
	else {
		std::cerr << "Cannot create Game Over text because font failed to load." << std::endl;
	}

	auto& restartButton = manager.addEntity();
	int restartButtonW = 142, restartButtonH = 59;
	float restartButtonX = (SCREEN_WIDTH / 2.0f) - (restartButtonW / 2.0f);
	float restartButtonY = 200;
	restartButton.addComponent<TransformComponent>(restartButtonX, restartButtonY, restartButtonW, restartButtonH, 1);
	restartButton.addComponent<SpriteComponent>("asset/Restart_button.png", false, Game::renderer);
	restartButton.addComponent<ButtonComponent>([this]() { this->StartNewGame(); });
	restartButton.getComponent<TransformComponent>().isGrounded = true;
	gameOverRestartButton = &restartButton;

	auto& menuButton = manager.addEntity();
	int menuButtonW = 142, menuButtonH = 53;
	float menuButtonX = (SCREEN_WIDTH / 2.0f) - (menuButtonW / 2.0f);
	float menuButtonY = 270;
	menuButton.addComponent<TransformComponent>(menuButtonX, menuButtonY, menuButtonW, menuButtonH, 1);
	menuButton.addComponent<SpriteComponent>("asset/Menu_button.png", false, Game::renderer);
	menuButton.addComponent<ButtonComponent>([this]() { this->GoToMainMenu(); });
	menuButton.getComponent<TransformComponent>().isGrounded = true;
	gameOverMenuButton = &menuButton;
	
}

void Game::DestroyGameOverMenu() {
	std::cout << "Destroying Game Over Menu..." << std::endl;
	if (gameOverRestartButton) gameOverRestartButton->destroy();
	if (gameOverMenuButton) gameOverMenuButton->destroy();
	if (gameOverText) gameOverText->destroy();
	gameOverRestartButton = nullptr;
	gameOverMenuButton = nullptr;
	gameOverText = nullptr;
	manager.refresh();
}

void Game::SpawnFireball() {
	std::cout << "Spawning fireball..." << std::endl;
	auto& fireball = manager.addEntity();
	int fireballW = 41; 
	int fireballH = 114;
	float startX = static_cast<float>(fireballPosDist(rng));
	float startY = -static_cast<float>(fireballH);
	float speedY = 150.0f + (rand() % 100);

	fireball.addComponent<TransformComponent>(startX, startY, fireballW, fireballH, 1);
	fireball.addComponent<SpriteComponent>("asset/Hit2.png", false, Game::renderer);
	auto& fbCollider = fireball.addComponent<ColliderComponent>("fireball");
	
	fireball.getComponent<TransformComponent>().velocity.y = speedY;
}

void Game::LoadHighScore() {
	std::ifstream file(highScoreFile);
	if (file.is_open()) {
		if (!(file >> highScore)) {
			std::cerr << "Warning: Could not read integer from " << highScoreFile << ". Setting high score to 0." << std::endl;
			highScore = 0;
		}
		file.close();
		std::cout << "High score loaded: " << highScore << std::endl;
	}
	else {
		std::cout << "High score file not found. Setting high score to 0." << std::endl;
		highScore = 0;
	}
}

void Game::UpdateScoreDisplay() {
	if (scoreTextEntity && scoreTextEntity->isActive() && scoreTextEntity->hasComponent<TextComponent>()) {
		std::stringstream ss;
		ss << "Score: " << std::setw(5) << std::setfill('0') << currentScore; 
		scoreTextEntity->getComponent<TextComponent>().SetText(ss.str());
		scoreTextEntity->getComponent<TransformComponent>().isGrounded = true;
	}
	else {
		std::cerr << "Warning: Score text entity is invalid or missing TextComponent." << std::endl;
	}
}

void Game::UpdateHighScoreDisplay() {
	std::stringstream ss;
	ss << "High: " << std::setw(5) << std::setfill('0') << highScore;
	std::string hsText = ss.str();

	if (highScoreTextEntity && highScoreTextEntity->isActive() && highScoreTextEntity->hasComponent<TextComponent>()) {
		highScoreTextEntity->getComponent<TextComponent>().SetText(hsText);
		highScoreTextEntity->getComponent<TransformComponent>().isGrounded = true;
	}

	if (menuHighScoreTextEntity && menuHighScoreTextEntity->isActive() && menuHighScoreTextEntity->hasComponent<TextComponent>()) {
		menuHighScoreTextEntity->getComponent<TextComponent>().SetText(hsText);
		menuHighScoreTextEntity->getComponent<TransformComponent>().isGrounded = true;
	}
}
void Game::SpawnStar() {
	auto& star = manager.addEntity();
	int starW = 56;
	int starH = 58;
	float startX = static_cast<float>(starPosDist(rng));
	float startY = -static_cast<float>(starH); 
	float speedY = 100.0f + (rand() % 80); 
	float speedX = (rand() % 100) - 50;
	star.addComponent<TransformComponent>(startX, startY, starW, starH, 1); 
	star.addComponent<SpriteComponent>("asset/Star.png", false, Game::renderer);
	star.addComponent<ColliderComponent>("star");

	auto& starTransform = star.getComponent<TransformComponent>();
	starTransform.velocity.y = speedY;
	starTransform.velocity.x = speedX; 
}

void Game::SaveHighScore() {
	std::ofstream file(highScoreFile);
	if (file.is_open()) {
		file << highScore;
		file.close();
		std::cout << "High score saved: " << highScore << std::endl;
	}
	else {
		std::cerr << "Error: Could not open " << highScoreFile << " for saving high score." << std::endl;
	}
}

void Game::CreatePlayingUI() {
	std::cout << "Creating Playing UI (Pause Button)..." << std::endl;
	auto& pButton = manager.addEntity();
	int pButtonW = 142;
	int pButtonH = 59;
	float pButtonX = SCREEN_WIDTH - pButtonW - 15;
	float pButtonY = 20;
	pButton.addComponent<TransformComponent>(pButtonX, pButtonY, pButtonW, pButtonH, 1);
	pButton.addComponent<SpriteComponent>("asset/pause_button.png", false, Game::renderer);
	pButton.getComponent<TransformComponent>().isGrounded = true;
	pButton.addComponent<ButtonComponent>([this]() { this->PauseGame(); });
	pauseButton = &pButton;
}

void Game::DestroyPlayingUI() {
	std::cout << "Destroying Playing UI..." << std::endl;
	if (pauseButton) pauseButton->destroy();
	if (scoreTextEntity) scoreTextEntity->destroy();
	if (highScoreTextEntity) highScoreTextEntity->destroy(); 
	pauseButton = nullptr;
	scoreTextEntity = nullptr;
	highScoreTextEntity = nullptr; 
	manager.refresh();
}

void Game::CreatePauseMenu() {
	std::cout << "Creating Pause Menu..." << std::endl;

	auto& contButton = manager.addEntity();
	int contButtonW = 142, contButtonH = 59;
	float contButtonX = (SCREEN_WIDTH / 2.0f) - (contButtonW / 2.0f);
	float contButtonY = 200;
	contButton.addComponent<TransformComponent>(contButtonX, contButtonY, contButtonW, contButtonH, 1);
	contButton.addComponent<SpriteComponent>("asset/Resume_button.png", false, Game::renderer); 
	contButton.addComponent<ButtonComponent>([this]() { this->ResumeGame(); });
	contButton.getComponent<TransformComponent>().isGrounded = true;
	pauseContinueButton = &contButton;

	auto& pngButton = manager.addEntity();
	int pngButtonW = 142, pngButtonH = 59;
	float pngButtonX = (SCREEN_WIDTH / 2.0f) - (pngButtonW / 2.0f);
	float pngButtonY = 270;
	pngButton.addComponent<TransformComponent>(pngButtonX, pngButtonY, pngButtonW, pngButtonH, 1);
	pngButton.addComponent<SpriteComponent>("asset/NewGame_button.png", false, Game::renderer);
	pngButton.addComponent<ButtonComponent>([this]() { this->StartNewGame(); });
	pngButton.getComponent<TransformComponent>().isGrounded = true;
	pauseNewGameButton = &pngButton;

	auto& pexButton = manager.addEntity();
	int pexButtonW = 142, pexButtonH = 59;
	float pexButtonX = (SCREEN_WIDTH / 2.0f) - (pexButtonW / 2.0f);
	float pexButtonY = 340;
	pexButton.addComponent<TransformComponent>(pexButtonX, pexButtonY, pexButtonW, pexButtonH, 1);
	pexButton.addComponent<SpriteComponent>("asset/Exit_button.png", false, Game::renderer);
	pexButton.addComponent<ButtonComponent>([this]() { this->ExitGame(); });
	pexButton.getComponent<TransformComponent>().isGrounded = true;
	pauseExitButton = &pexButton;

	auto& pbg = manager.addEntity();
	pbg.addComponent<TransformComponent>(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
	pbg.addComponent<SpriteComponent>("asset/pause_menu_bg.jpg", false, Game::renderer);
	pauseMenuBackground = &pbg;
	pbg.getComponent<TransformComponent>().isGrounded = true;

}

void Game::DestroyPauseMenu() {
	std::cout << "Destroying Pause Menu..." << std::endl;
	if (pauseMenuBackground) pauseMenuBackground->destroy();
	if (pauseContinueButton) pauseContinueButton->destroy();
	if (pauseNewGameButton) pauseNewGameButton->destroy();
	if (pauseExitButton) pauseExitButton->destroy();
	pauseMenuBackground = nullptr;
	pauseContinueButton = nullptr;
	pauseNewGameButton = nullptr;
	pauseExitButton = nullptr;
	manager.refresh();
	CreatePlayingUI();
}

void Game::ResetGame() {
	std::cout << "Resetting Game..." << std::endl;
	currentScore = 0;
	starSpawnTimer = 0.0f;
	fireballSpawnTimer = 0.0f;

	auto& entities = manager.getEntities();
	for (int i = entities.size() - 1; i >= 0; --i) {
		if (entities[i].get() != gameBackground) {
			entities[i]->destroy();
		}
	}
	manager.refresh();

	if (!gameBackground) {
		auto& bg = manager.addEntity();
		bg.addComponent<TransformComponent>(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
		bg.addComponent<SpriteComponent>("asset/bg2.png", false, Game::renderer);
		bg.getComponent<TransformComponent>().isGrounded = true;
		gameBackground = &bg;
	}
	else {
		gameBackground->setActive(true);
	}

	auto& playerEntity = manager.addEntity();
	int playerW = 100;
	int playerH = 113; 
	playerEntity.addComponent<TransformComponent>(100.0f, 100.0f, playerW, playerH, 1);
	auto& playerSprite = playerEntity.addComponent<SpriteComponent>("asset/player_idle1.png", true, renderer);
	playerEntity.addComponent<KeyboardController>();
	playerEntity.getComponent<KeyboardController>().enabled = true;
	auto& playerCollider = playerEntity.addComponent<ColliderComponent>("player");

	if (!playerSprite.animation.count("Idle")) {
		Animation idle = Animation(0, 2, 200);
		Animation walk = Animation(1, 4, 150);
		Animation jump = Animation(2, 2, 150);
		Animation fall = Animation(3, 2, 150);
		
		playerSprite.animation.emplace("Idle", idle);
		playerSprite.animation.emplace("Walk", walk);
		playerSprite.animation.emplace("Jump", jump);
		playerSprite.animation.emplace("Fall", fall);
		playerSprite.Play("Idle");
		std::cout << "ResetGame: Re-added standard animations to player." << std::endl;
	}
	else {
		playerSprite.Play("Idle");
	}

	this->player = &playerEntity;

	auto& ground = manager.addEntity();
	int groundY = 500, groundHeight = 50;
	ground.addComponent<TransformComponent>(0.0f, static_cast<float>(groundY), SCREEN_WIDTH, groundHeight, 1);
	auto& groundCollider = ground.addComponent<ColliderComponent>("ground");
	groundCollider.transform->isGrounded = true;

	auto& wall = manager.addEntity();
	float wallX = 700.0f, wallY = 400.0f; int wallWidth = 30, wallHeight = 100;
	wall.addComponent<TransformComponent>(wallX, wallY, wallWidth, wallHeight, 1);
	wall.addComponent<SpriteComponent>("asset/dirt.png", false, Game::renderer);
	wall.addComponent<ColliderComponent>("wall");
	wall.getComponent<TransformComponent>().isGrounded = true;

	auto& tree = manager.addEntity();
	float treeX = 400.0f, treeY = 400.0f; int treeWidth = 30, treeHeight = 100;
	tree.addComponent<TransformComponent>(treeX, treeY, treeWidth, treeHeight, 1);
	tree.addComponent<SpriteComponent>("asset/grass.png", false, Game::renderer);
	tree.addComponent<ColliderComponent>("wall");
	tree.getComponent<TransformComponent>().isGrounded = true;

	if (gameFont) {
		auto& scoreEntity = manager.addEntity();
		float scoreX = 15.0f;
		float scoreY = 10.0f;
		scoreEntity.addComponent<TransformComponent>(scoreX, scoreY, 0, 0, 1);
		auto& scoreComp = scoreEntity.addComponent<TextComponent>(gameFont, "Score: 00000", textColor);
		scoreTextEntity = &scoreEntity;
		UpdateScoreDisplay();

		auto& highScoreEntity = manager.addEntity();
		float highScoreY = scoreY + 35; 
		highScoreEntity.addComponent<TransformComponent>(scoreX, highScoreY, 0, 0, 1);

		auto& highScoreComp = highScoreEntity.addComponent<TextComponent>(gameFont, "High: 00000", highScoreColor);
		highScoreTextEntity = &highScoreEntity;
		UpdateHighScoreDisplay(); 
	}
	CreatePlayingUI();
}


void Game::StartNewGame() {
	std::cout << "Starting New Game..." << std::endl;
	AudioManager::StopMusic();
	if (currentState == GameState::MainMenu) {
		DestroyMainMenu();
	}
	else if (currentState == GameState::Paused) {
		DestroyPauseMenu();
	}
	else if (currentState == GameState::GameOver) { 
		DestroyGameOverMenu();
	}
	ResetGame();
	currentState = GameState::Playing;
	AudioManager::PlayMusic("theme", -1);
}

void Game::GoToMainMenu() {
	std::cout << "Going to Main Menu..." << std::endl;
	if (currentState == GameState::Paused) {
		DestroyPauseMenu();
		DestroyPlayingUI();
	}
	else if (currentState == GameState::GameOver) {
		DestroyGameOverMenu();
	}
	else if (currentState == GameState::Playing) {
		DestroyPlayingUI();
	}
	auto& entities = manager.getEntities();
	for (int i = entities.size() - 1; i >= 0; --i) {
		bool isMainMenuUI = (entities[i].get() == mainMenuBackground ||
							 entities[i].get() == mainMenuNewGameButton ||
							 entities[i].get() == mainMenuExitButton ||
							 entities[i].get() == gameBackground);
		if (!isMainMenuUI) {
			entities[i]->destroy();
		}
	}
	manager.refresh();
	player = nullptr;

	CreateMainMenu();
	currentState = GameState::MainMenu;
	AudioManager::PlayMusic("menu_theme", -1);
}



void Game::PauseGame() {
	if (currentState == GameState::Playing) {
		std::cout << "Game Paused." << std::endl;
		currentState = GameState::Paused;
		if (pauseButton) pauseButton->setActive(false);
			CreatePauseMenu();
	
	}
}

void Game::ResumeGame() {
	if (currentState == GameState::Paused) {
		std::cout << "Resuming Game..." << std::endl;
		DestroyPauseMenu();
		if (pauseButton) pauseButton->setActive(true);
		currentState = GameState::Playing;
	}
}

void Game::ExitGame() {
	std::cout << "Exiting Game..." << std::endl;
	AudioManager::StopMusic();
	currentState = GameState::Exiting;
}


void Game::clean() {
	std::cout << "Cleaning game....." << std::endl;
	SaveHighScore();
	if (gameFont) {
		TTF_CloseFont(gameFont);
		gameFont = nullptr;
		std::cout << "Font has been cleaned" << std::endl;
	}
	manager.refresh();
	auto& entities = manager.getEntities();
	for (int i = entities.size() - 1; i >= 0; --i) {
		entities[i]->destroy();
	}
	manager.refresh();

	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
		std::cout << "Renderer has been destroyed." << std::endl;
	}
	if (window) {
		SDL_DestroyWindow(window);
		window = nullptr; 
		std::cout << "Window has been destroyed." << std::endl;
	}

	IMG_Quit(); 
	SDL_Quit();
	std::cout << "Complete cleaning game!" << std::endl;
}
