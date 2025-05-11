#pragma once
#include "ECS.h"
#include "Components.h"
#include <SDL_keyboard.h>
#include "AudioManager.h"
#include <iostream>
#include "Collision.h"
class KeyboardController : public Component {
public:
	TransformComponent* transform = nullptr;
	SpriteComponent* sprite = nullptr;
	bool enabled = true; 

	void init() override {
		if (entity->hasComponent<TransformComponent>()) {
			transform = &entity->getComponent<TransformComponent>();
		}
		else {
			std::cerr << "KC Error: Missing TransformComponent!\n";
		}
		if (entity->hasComponent<SpriteComponent>()) {
			sprite = &entity->getComponent<SpriteComponent>();
			if (sprite && !sprite->animation.empty() && sprite->animation.count("Idle")) {
				sprite->Play("Idle");
			}
		}
		else {
			std::cerr << "KC Warning: Missing SpriteComponent!\n";
		}
		enabled = true;
	}

	void update(float dt) override {
		if (!enabled || !transform || !sprite || sprite->animation.empty()) return;
		const Uint8* keystate = SDL_GetKeyboardState(NULL);
		if (!keystate) return;

		bool currentGroundedState = transform->isGrounded;

		float targetVelocityX = 0.0f;
		if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) {
			targetVelocityX = -transform->speed;
			if (sprite) sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
		}
		else if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) {
			targetVelocityX = transform->speed;
			if (sprite) sprite->spriteFlip = SDL_FLIP_NONE;
		}
		if (!transform->isGrounded) {
			targetVelocityX *= 0.8f;
		}
		transform->velocity.x = targetVelocityX;

		const float nearZeroVelocityY = 0.1f;
		bool considerAsGrounded = currentGroundedState || (std::abs(transform->velocity.y) < nearZeroVelocityY);
	} 


	void TryJump() {
		if (!enabled) return;
		if (transform && transform->isGrounded) {
			transform->velocity.y = -transform->jumpForce;
			transform->isGrounded = false;
			if (sprite && sprite->animation.count("Jump")) {
				sprite->Play("Jump");
			}
			AudioManager::PlayChunk("jump");
		}
	}
};
