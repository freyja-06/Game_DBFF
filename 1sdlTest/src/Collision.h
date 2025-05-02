#pragma once
#include <SDL_rect.h>

class ColliderComponent;

class Collision {
public:
	static bool AABB(const SDL_Rect& rectA, const SDL_Rect& rectB);
	static bool AABB(const ColliderComponent& colA, const ColliderComponent& colB);
	static bool PointInRect(const SDL_Point& point, const SDL_Rect& rect);
};