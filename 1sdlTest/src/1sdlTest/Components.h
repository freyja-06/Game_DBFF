#pragma once

#include "ECS.h"
#include "Vector2D.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "KeyboardController.h"
#include "ColliderComponent.h"
#include "TextComponent.h"


#include <functional> 
#include <string>

class TransformComponent;
class SpriteComponent;
class KeyboardController;
class ColliderComponent;

enum class ButtonState {
	Normal,
	Pressed
};

class ButtonComponent : public Component {
public:
	TransformComponent* transform = nullptr; 
	ButtonState currentState = ButtonState::Normal;
	std::function<void()> onClick;

	ButtonComponent(std::function<void()> action = nullptr) : onClick(action) {}

	void init() override {
		if (!entity->hasComponent<TransformComponent>()) {
			std::cerr << "ButtonComponent Error: Missing TransformComponent!\n";
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->getComponent<TransformComponent>();
	}

	void update(float dt) override {
	}
	void draw() override {
	}

	void TriggerClick() {
		if (onClick) {
			onClick(); 
		}
		else {
			std::cout << "Button clicked, but no action assigned." << std::endl;
		}
	}
};
