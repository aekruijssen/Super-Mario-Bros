#pragma once
#include "Actor.h"

class Player : public Actor
{
public:
	Player(class Game* game);

	void ChangeTexture(char* texturePath);

	class CollisionComponent* cc;
	class AnimatedSprite* as;

private:
	class MoveComponent* mc;
	class PlayerMove* pm;
};