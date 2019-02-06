#include "Goomba.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "SDL/SDL.h"
#include "Random.h"
#include "GoombaMove.h"


Goomba::Goomba(Game* game)
	:Actor(game)
{
	cc = new CollisionComponent(this);
	cc->SetSize(32.0f, 32.0f);

	as = new AnimatedSprite(this, 150);
	std::vector<SDL_Texture*> walkAnim{
		mGame->GetTexture("Assets/Goomba/Walk0.png"),
		mGame->GetTexture("Assets/Goomba/Walk1.png")
	};
	std::vector<SDL_Texture*> deadAnim{
		mGame->GetTexture("Assets/Goomba/Dead.png")
	};
	// This assumes "asc" is the animated sprite component
	as->AddAnimation("walk", walkAnim);
	as->AddAnimation("dead", deadAnim);
	as->SetAnimation("walk");

	gm = new GoombaMove(this);

	stomped = false;

	mGame->AddGoomba(this);
}


Goomba::~Goomba() {
	mGame->RemoveGoomba(this);
}

void Goomba::ChangeTexture(char* texturePath) {
	SDL_Texture* texture = mGame->GetTexture(texturePath);
	as->SetTexture(texture);
}