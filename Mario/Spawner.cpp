#include "Player.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "SDL/SDL.h"
#include "Random.h"
#include "PlayerMove.h"
#include "Spawner.h"
#include "Goomba.h"


Spawner::Spawner(Game* game)
	:Actor(game)
{
}
void Spawner::OnUpdate(float deltaTime){
	// Spawn Goomba if the x - distance between the spawner and the players less than 600
	if (abs(GetPosition().x - mGame->GetPlayer()->GetPosition().x) < 600) {
		Goomba* goomba = new Goomba(mGame);
		goomba->SetPosition(GetPosition());
		SetState(ActorState::Destroy);
	}
} 