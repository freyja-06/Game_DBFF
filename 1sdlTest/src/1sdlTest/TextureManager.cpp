#include "TextureManager.h"
#include <iostream>    

SDL_Texture* TextureManager::LoadTexture(const char* fileName, SDL_Renderer* ren) {
	if (!ren) { 
		std::cerr << "TextureManager Error: Renderer passed to LoadTexture is NULL!" << std::endl;
		return nullptr;
	}
	SDL_Surface* tempSurface = IMG_Load(fileName);
	if (!tempSurface) {
		std::cerr << "IMG_Load Error loading [" << fileName << "]: " << IMG_GetError() << std::endl;
		return nullptr;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, tempSurface);
	SDL_FreeSurface(tempSurface);
	if (!tex) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}
	return tex;
}

void TextureManager::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_Renderer* ren, SDL_RendererFlip flip) {
	if (!ren) {
		return;
	}
	if (!tex) {
		return;
	}
	SDL_RenderCopyEx(ren, tex, &src, &dest, 0.0, NULL, flip);
}