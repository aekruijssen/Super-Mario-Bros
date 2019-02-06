#pragma once
#include "Actor.h"

class Block : public Actor
{
public:
	Block(class Game* game);
	~Block();
	void ChangeTexture(const char* texturePath);

	class CollisionComponent* cc;

private:
	class MoveComponent* mc;
	class SpriteComponent* sc;
};