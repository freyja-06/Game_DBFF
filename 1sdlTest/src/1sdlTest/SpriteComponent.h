#pragma once
#include "ECS.h"
#include "Animation.h"
#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <string> 
#include <iostream> 
#include "TextureManager.h"
class TransformComponent;

class SpriteComponent : public Component {
private:
	TransformComponent* transform = nullptr;
	SDL_Texture* texture = nullptr; 
	SDL_Rect srcRect = { 0, 0, 0, 0 };
	SDL_Rect destRect = { 0, 0, 0, 0 };

	bool animated = false;
	int frames = 0;
	int speed = 100;

	Uint32 animationStartTime = 0;
	bool currentAnimationLoops = true;
	bool animationFinished = false;


public:
	int animIndex = 0; 
	std::map<const char*, Animation> animation;
	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

	SpriteComponent() = default;

	SpriteComponent(const char* texturePath, SDL_Renderer* rendererRef = nullptr)
	{
		if (texturePath && texturePath[0] != '\0' && rendererRef) {
			if (!setTexture(texturePath, rendererRef)) {
			}
		}
	}


	SpriteComponent(const char* texturePath, bool isAnimated, SDL_Renderer* ren) 
		: animated(isAnimated)
	{
		if (!setTexture(texturePath, ren)) {
			std::cerr << "SpriteComponent Error: Failed to load texture in constructor." << std::endl;
			animated = false;
			return;
		}
		if (animated) {
			Animation idle = Animation(0, 2, 200, true); 
			Animation walk = Animation(1, 4, 150, true); 
			Animation jump = Animation(2, 2, 150, true);
			Animation fall = Animation(3, 2, 150, true);
			Animation hit  = Animation(0, 7, 150, false);
			animation.emplace("Idle", idle);
			animation.emplace("Walk", walk);
			animation.emplace("Jump", jump); 
			animation.emplace("Fall", fall); 
			animation.emplace("Hit", hit);
			Play("Idle");
			std::cout << "SpriteComponent: Initialized with animations. Starting with Idle." << std::endl;
		}
		else {
			std::cout << "SpriteComponent: Initialized as non-animated." << std::endl;
		}
	}

	~SpriteComponent() {
		if (texture) {
			SDL_DestroyTexture(texture);
		}
	}

	bool setTexture(const char* texturePath, SDL_Renderer* ren) {
		if (texture) SDL_DestroyTexture(texture);
		texture = nullptr; 

		if (!ren) {
			std::cerr << "SpriteComponent Error: Renderer is NULL in setTexture!\n";
			return false;
		}
		if (!texturePath || texturePath[0] == '\0') {
			std::cerr << "SpriteComponent Error: Invalid texture path.\n";
			return false;
		}

		SDL_Surface* tmpSurface = IMG_Load(texturePath);
		if (!tmpSurface) {
			std::cerr << "IMG_Load Error loading [" << texturePath << "]: " << IMG_GetError() << std::endl;
			return false;
		}
		texture = SDL_CreateTextureFromSurface(ren, tmpSurface);
		SDL_FreeSurface(tmpSurface);
		if (!texture) {
			std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
			return false;
		}
		std::cout << "SpriteComponent: Texture loaded [" << texturePath << "], frame size " << srcRect.w << "x" << srcRect.h << std::endl;

		if (entity && entity->hasComponent<TransformComponent>()) {
			auto* tempTransform = &entity->getComponent<TransformComponent>();
			if (tempTransform->width == 0 || tempTransform->height == 0) { 
				tempTransform->width = srcRect.w;
				tempTransform->height = srcRect.h;
				std::cout << "SpriteComponent: Updated Transform size from texture." << std::endl;
			}
		}

		return true;
	}

	void init() override {
		if (!entity->hasComponent<TransformComponent>()) {
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->getComponent<TransformComponent>();
		if (transform) {
			srcRect.w = transform->width;
			srcRect.h = transform->height;
			if (srcRect.w <= 0 || srcRect.h <= 0) {
				std::cerr << "SpriteComponent Warning: Invalid size from Transform in init(). Using 32x32.\n";
				srcRect.w = 32;
				srcRect.h = 32;
				transform->width = 32; 
				transform->height = 32;
			}
		}
		else {
			std::cerr << "SpriteComponent Error: Missing Transform in init()! Using 32x32.\n";
			srcRect.w = 32;
			srcRect.h = 32;
		}
		srcRect.x = 0;
		srcRect.y = 0;
		std::cout << "SpriteComponent Init: srcRect set to " << srcRect.w << "x" << srcRect.h << std::endl;
	}

	void UpdateAnimationState() {
		if (!transform || animation.empty()) return;

		bool currentGroundedState = transform->isGrounded;
		bool isMovingHorizontally = std::abs(transform->velocity.x) > 0.1f;

		int currentAnimIndex = animIndex;
		int idleIndex = animation.count("Idle") ? animation["Idle"].index : -1;
		int walkIndex = animation.count("Walk") ? animation["Walk"].index : -1;
		int jumpIndex = animation.count("Jump") ? animation["Jump"].index : -1;
		int fallIndex = animation.count("Fall") ? animation["Fall"].index : -1;

		if (currentGroundedState) {
			if (isMovingHorizontally) {
				if (walkIndex != -1 && currentAnimIndex != walkIndex) Play("Walk");
			}
			else {
				if (idleIndex != -1 && currentAnimIndex != idleIndex) Play("Idle");
			}
			if (currentAnimIndex == jumpIndex || currentAnimIndex == fallIndex) {
				if (isMovingHorizontally && walkIndex != -1) Play("Walk");
				else if (idleIndex != -1) Play("Idle");
			}

		}
		else {
			if (transform->velocity.y < -0.1f) {
				if (jumpIndex != -1 && currentAnimIndex != jumpIndex) Play("Jump");
			}
			else if (transform->velocity.y > 0.1f) {
				if (fallIndex != -1 && currentAnimIndex != fallIndex) Play("Fall");
				else if (fallIndex == -1 && jumpIndex != -1 && currentAnimIndex != jumpIndex) Play("Jump");
			}
		}
	}

	void update(float dt) override {
		if (!transform) return;

		int currentFrame = 0;
		if (animated && frames > 0 && speed > 0 && srcRect.w > 0) { 
			if (!currentAnimationLoops && animationFinished) {
				currentFrame = frames - 1;
			}
			else {
				Uint32 elapsedTime = SDL_GetTicks() - animationStartTime;
				currentFrame = (speed > 0) ? (elapsedTime / speed) : 0;

				if (currentAnimationLoops) {
					currentFrame %= frames;
				}
				else {
					if (currentFrame >= frames) {
						currentFrame = frames - 1;
						animationFinished = true;
					}
				}
			}
			srcRect.x = srcRect.w * currentFrame;

		}
		else {
			srcRect.x = 0;
		}
		if (srcRect.h > 0) {
			srcRect.y = animIndex * srcRect.h;
		}
		else {
			srcRect.y = 0;
		}
		destRect.x = static_cast<int>(transform->position.x);
		destRect.y = static_cast<int>(transform->position.y);
		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
}


	void draw() override {
		if (texture && transform && renderer && srcRect.w > 0 && srcRect.h > 0) {
			SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0.0, NULL, spriteFlip);
		}
	}

	void Play(const char* animName) {
		if (animation.count(animName)) {
			Animation newAnim = animation[animName];

			if (this->animIndex != newAnim.index || this->animationFinished) {
				this->animIndex = newAnim.index;
				this->frames = newAnim.frames;
				this->speed = newAnim.speed;

				this->currentAnimationLoops = newAnim.loop;
				this->animationStartTime = SDL_GetTicks();
				this->animationFinished = false;

				std::cout << "Sprite Playing: " << animName << " (Index: " << animIndex
					<< ", Frames: " << frames << ", Speed: " << speed
					<< ", Loop: " << currentAnimationLoops << ")" << std::endl << std::flush;

			}
			std::cout << "Sprite Playing: " << animName << " (Index: " << animIndex << ", Frames: " << frames << ", Speed: " << speed << ")" << std::endl << std::flush;
		}
		else {
			std::cerr << "Sprite Error: Animation '" << animName << "' not found!" << std::endl;
		}
	}
	// Biến tĩnh renderer (vẫn cần định nghĩa và gán giá trị trong main.cpp)
	static SDL_Renderer* renderer;

	bool ChangeTexture(const char* texturePath, SDL_Renderer* ren, bool isNowAnimated = false) {
		animated = isNowAnimated; // Cập nhật trạng thái animated
		animation.clear();       // Xóa animation cũ nếu có
		frames = 0;              // Reset frames
		animIndex = 0;           // Reset animIndex
		// Tải texture mới
		bool success = setTexture(texturePath, ren);
		if (success && entity && entity->hasComponent<TransformComponent>()) {
			// Cập nhật lại srcRect từ transform sau khi đổi texture
			transform = &entity->getComponent<TransformComponent>();
			srcRect.w = transform->width;
			srcRect.h = transform->height;
			srcRect.x = 0;
			srcRect.y = 0;
		}
		return success;
	}

};