﻿#pragma once
#include "ECS.h"
#include "Vector2D.h"

class TransformComponent : public Component {
public:
	Vector2D position;
	Vector2D velocity; 
	int width = 32;    
	int height = 32;
	int scale = 1;

	bool isGrounded = false;
	float gravity = 400.0f;
	float jumpForce = 350.0f; 
	float speed = 180.0f; 

	
	TransformComponent() = default;

	TransformComponent(float x, float y, int w, int h, int s)
		: position(x, y), width(w), height(h), scale(s)
	{
		velocity.Zero();
	}
	void init() override {
		velocity.Zero();
	}
	void update(float dt) override {
		position.x += velocity.x * dt;
		if (!isGrounded) {
			velocity.y += gravity * dt;
		}
		if (std::abs(velocity.x) > 0.01f) {
		     std::cout << "TC Update: Applied vx=" << velocity.x << ", dt=" << dt << ". New pos.x=" << position.x << std::endl;
		 }
		position.x += velocity.x * dt;
		position.y += velocity.y * dt;

	}

	void setX(float x) { position.x = x; }
	void setY(float y) { position.y = y; }
};
