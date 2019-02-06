#include "Block.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "SDL/SDL.h"
#include "Random.h"


Block::Block(Game* game)
	:Actor(game)
{
	cc = new CollisionComponent(this);
	cc->SetSize(32.0f, 32.0f);

	sc = new SpriteComponent(this);

	//add Block actor to game
	mGame->AddBlock(this);
}


Block::~Block() {
	mGame->RemoveBlock(this);
}

void Block::ChangeTexture(const char* texturePath) {
	SDL_Texture* texture = mGame->GetTexture(texturePath);
	sc->SetTexture(texture);
}