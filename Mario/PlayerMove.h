#pragma once
#include "MoveComponent.h"
#include "Math.h"



class PlayerMove : public MoveComponent
{
public:
	PlayerMove(class Player* owner);

	// Update the move component
	void Update(float deltaTime) override;
	void ProcessInput(const Uint8 *keyState) override;

	// Getters/setters
	float GetAngularSpeed() const { return mAngularSpeed; }
	float GetForwardSpeed() const { return mForwardSpeed; }
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }

	Player* mPlayer;
	Vector2 mPlayerVel;

private:

	void UpdateAnim();

	// Angular speed (in radians/second)
	float mAngularSpeed;
	// Forward speed (in pixels/second)
	float mForwardSpeed;

	float mYSpeed;
	bool mSpacePressed;
	bool mInAir;
	const Uint8 *lastFrame;

	class Game* mGame;
};
