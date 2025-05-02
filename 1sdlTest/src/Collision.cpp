#include "Collision.h"
#include "ColliderComponent.h"

bool Collision::AABB(const SDL_Rect& rectA, const SDL_Rect& rectB) {
	if (rectA.x + rectA.w >= rectB.x &&  
		rectB.x + rectB.w >= rectA.x &&   
		rectA.y + rectA.h >= rectB.y &&   
		rectB.y + rectB.h >= rectA.y) {   
		return true;
	}
	return false;
}

bool Collision::AABB(const ColliderComponent& colA, const ColliderComponent& colB) {
	return AABB(colA.collider, colB.collider);
}

bool Collision::PointInRect(const SDL_Point& point, const SDL_Rect& rect) {
	return (point.x >= rect.x &&             
			point.x < (rect.x + rect.w) &&  
			point.y >= rect.y &&             
			point.y < (rect.y + rect.h));    
}

