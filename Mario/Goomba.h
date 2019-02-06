#pragma once
#include "Actor.h"

class Goomba : public Actor
{
public:
	Goomba(class Game* game);
	~Goomba();
	void ChangeTexture(char* texturePath);

	class CollisionComponent* cc;
	class AnimatedSprite* as;

	bool stomped;

private:
	class GoombaMove* gm;
};