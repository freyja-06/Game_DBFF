#pragma once
#include "ECS.h"
#include "Components.h"
#include "Game.hpp"
#include <SDL_rect.h>
#include <string>
#include <vector> 
#include <iostream>

class ColliderComponent : public Component {
public:
	SDL_Rect collider; 
	std::string tag; 
	TransformComponent* transform = nullptr;

	int offsetX = 0;
	int offsetY = 0; 
	ColliderComponent(std::string t = "untagged", int offX = 0, int offY = 0)
		: tag(t), offsetX(offX), offsetY(offY)
	{
		collider = { 0, 0, 0, 0 };
	}

	~ColliderComponent() {
		
	}

	void init() override {
		if (!entity->hasComponent<TransformComponent>()) {
			std::cerr << "ColliderComponent Warning: Entity missing TransformComponent. Adding default.\n";
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->getComponent<TransformComponent>();

		if (transform) {
			collider.w = transform->width * transform->scale;
			collider.h = transform->height * transform->scale;
			collider.x = static_cast<int>(transform->position.x) + offsetX;
			collider.y = static_cast<int>(transform->position.y) + offsetY;
		}
	}

	void update(float dt) override {
		if (transform) {
			collider.x = static_cast<int>(transform->position.x);
			collider.y = static_cast<int>(transform->position.y);
			collider.w = transform->width * transform->scale;
			collider.h = transform->height * transform->scale;
		}
	}
};