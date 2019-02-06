#include "Player.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "SDL/SDL.h"
#include "Random.h"
#include "PlayerMove.h"


Player::Player(Game* game)
	:Actor(game)
{
	mGame = game;
	cc = new CollisionComponent(this);
	cc->SetSize(32.0f, 32.0f);

	pm = new PlayerMove(this);

	as = new AnimatedSprite(this, 150);
	std::vector<SDL_Texture*> idleAnim{
		mGame->GetTexture("Assets/Mario/idle.png")
	};
	std::vector<SDL_Texture*> deadAnim{
		mGame->GetTexture("Assets/Mario/Dead.png")
	};
	std::vector<SDL_Texture*> jumpLeft{
		mGame->GetTexture("Assets/Mario/JumpLeft.png")
	};
	std::vector<SDL_Texture*> jumpRight{
		mGame->GetTexture("Assets/Mario/JumpRight.png")
	};
	std::vector<SDL_Texture*> runLeft{
		mGame->GetTexture("Assets/Mario/RunLeft0.png"),
		mGame->GetTexture("Assets/Mario/RunLeft1.png"),
		mGame->GetTexture("Assets/Mario/RunLeft2.png")
	};
	std::vector<SDL_Texture*> runRight{
		mGame->GetTexture("Assets/Mario/RunRight0.png"),
		mGame->GetTexture("Assets/Mario/RunRight1.png"),
		mGame->GetTexture("Assets/Mario/RunRight2.png")
	};
	// Assumes "asc" is the animated sprite component
	as->AddAnimation("idle", idleAnim);
	as->AddAnimation("dead", deadAnim);
	as->AddAnimation("jumpLeft", jumpLeft);
	as->AddAnimation("jumpRight", jumpRight);
	as->AddAnimation("runLeft", runLeft);
	as->AddAnimation("runRight", runRight);
	as->SetAnimation("idle");
}

void Player::ChangeTexture(char* texturePath) {
	SDL_Texture* texture = mGame->GetTexture(texturePath);
	as->SetTexture(texture);
}