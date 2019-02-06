#pragma once
#include "MoveComponent.h"
#include "Math.h"

class GoombaMove : public MoveComponent
{
public:
	GoombaMove(class Goomba* owner);
	Goomba* mGoomba;
	Vector2 mPlayerVel;

	void Update(float deltaTime) override;

private:
	// Angular speed (in radians/second)
	float mAngularSpeed;
	// Forward speed (in pixels/second)
	float mForwardSpeed;

	float mYSpeed;
	bool mInAir;
	float lifetime;
};
