#pragma once

#include "ECS.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "Components.h" 
#include "TextureManager.h" 
#include "Game.hpp"	

class TextComponent : public Component {
private:
	TransformComponent* transform = nullptr;
	SDL_Texture* texture = nullptr;
	TTF_Font* font = nullptr;
	std::string text;
	SDL_Color color;
	SDL_Rect srcRect, destRect;
	bool needsUpdate = true; 

	void CreateTextTexture(SDL_Renderer* renderer) {
		if (texture) {
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}
		if (!font || text.empty()) return;

		SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color); 
		if (!surface) {
			std::cerr << "TTF_RenderText_Blended Error: " << TTF_GetError() << std::endl;
			return;
		}

		texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (!texture) {
			std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		}
		else {
			int texW, texH;
			SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
			srcRect = { 0, 0, texW, texH };
			if (transform) {
				if (transform->width == 0 || transform->height == 0) {
					transform->width = texW;
					transform->height = texH;
				}
				destRect.w = transform->width * transform->scale;
				destRect.h = transform->height * transform->scale;
			}
			else {
				destRect.w = texW;
				destRect.h = texH;
			}
			std::cout << "Text texture created: '" << text << "' (" << texW << "x" << texH << ")" << std::endl;
		}
		SDL_FreeSurface(surface);
		needsUpdate = false;
	}


public:
	TextComponent(TTF_Font* f, std::string t, SDL_Color c)
		: font(f), text(t), color(c), texture(nullptr)
	{
		if (!font) {
			std::cerr << "TextComponent Warning: Font is NULL!" << std::endl;
		}
	}

	~TextComponent() {
		if (texture) {
			SDL_DestroyTexture(texture);
		}
	}

	void init() override {
		if (!entity->hasComponent<TransformComponent>()) {
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->getComponent<TransformComponent>();
		needsUpdate = true;
	}

	void update(float dt) override {
		if (!transform) return;
		if (needsUpdate && Game::renderer) {
			CreateTextTexture(Game::renderer);
		}
		if (transform) {
			destRect.x = static_cast<int>(transform->position.x);
			destRect.y = static_cast<int>(transform->position.y);
			destRect.w = transform->width * transform->scale;
			destRect.h = transform->height * transform->scale;
		}
	}

	void draw() override {
		if (texture && transform && Game::renderer) {
			destRect.x = static_cast<int>(transform->position.x);
			destRect.y = static_cast<int>(transform->position.y);
			destRect.w = transform->width * transform->scale;
			destRect.h = transform->height * transform->scale;
			TextureManager::Draw(texture, srcRect, destRect, Game::renderer, SDL_FLIP_NONE);
		}
		else if (needsUpdate && Game::renderer) {
			CreateTextTexture(Game::renderer);
			if (texture) {
				TextureManager::Draw(texture, srcRect, destRect, Game::renderer, SDL_FLIP_NONE);
			}
		}
	}

	void SetText(const std::string& newText, const SDL_Color& newColor) {
		if (text != newText || color.r != newColor.r || color.g != newColor.g || color.b != newColor.b || color.a != newColor.a) {
			text = newText;
			color = newColor;
			needsUpdate = true;
		}
	}
	void SetText(const std::string& newText) {
		if (text != newText) {
			text = newText;
			needsUpdate = true;
		}
	}
};