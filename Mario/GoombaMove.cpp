#include "GoombaMove.h"
#include "Actor.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Block.h"
#include "Math.h"
#include "Player.h"
#include "AnimatedSprite.h"
#include "MoveComponent.h"
#include "SDL/SDL.h"
#include "Random.h"
#include "Goomba.h"


GoombaMove::GoombaMove(class Goomba* owner)
	:MoveComponent(owner)
	, mAngularSpeed(0.0f)
	, mForwardSpeed(-100.0f)
{
	mGoomba = owner;
	mPlayerVel.x = -100;
	mPlayerVel.y = 0;
	mYSpeed = 0.0f;
	lifetime = 0;

	SetForwardSpeed(-100);

	mInAir = false;
}

void GoombaMove::Update(float deltaTime) {
	if (mGoomba->stomped == false) {
		Vector2 pos = mGoomba->GetPosition();
		pos.x += mForwardSpeed * deltaTime;
		pos.y += mYSpeed * deltaTime;
		mGoomba->SetPosition(pos);

		if (mGoomba->GetPosition().y > 448.0f) {
			Vector2 resetPos = mGoomba->GetPosition();
			resetPos.y = 448.0f;
			mGoomba->SetPosition(resetPos);
		}
		for (auto enemy : (mGoomba->GetGame())->mGoombas) {
			if (enemy != mGoomba) {
				float goombaX = mGoomba->GetPosition().x;
				float goombaY = mGoomba->GetPosition().y;
				Vector2 goombaPos = Vector2(goombaX, goombaY);

				CollSide cs = (mGoomba->cc)->GetMinOverlap(enemy->cc, goombaPos);
				Vector2 curPos = mGoomba->GetPosition();
				if (cs != CollSide::None) {
					if (cs == CollSide::Top && mYSpeed > 0.0f) {
						mYSpeed = 0.0f;
						mInAir = false;
					}
					else if (cs == CollSide::Bottom && mYSpeed < 0) {
						mYSpeed = 0.0f;
					}
					curPos.y += goombaPos.y;
					mGoomba->SetPosition(curPos);
				}
			}
		}

		//If while moving left, they run into a block or another Goomba, they should start moving to the right
		for (auto enemy : (mGoomba->GetGame())->mGoombas) {
			if (enemy != mGoomba) {
				float goombaX = mGoomba->GetPosition().x;
				float goombaY = mGoomba->GetPosition().y;
				Vector2 goombaPos = Vector2(goombaX, goombaY);

				CollSide cs = (mGoomba->cc)->GetMinOverlap(enemy->cc, goombaPos);
				Vector2 curPos = mGoomba->GetPosition();
				if (cs != CollSide::None) {
					if (cs == CollSide::Right) {
						if (enemy->GetPosition().y <= curPos.y + 1 && enemy->GetPosition().y >= curPos.y - 1) {
							SetForwardSpeed(100.0f);
							mForwardSpeed = 100.0f;
							curPos.x += goombaPos.x + (mForwardSpeed*deltaTime);
						}
					}
					else if (cs == CollSide::Left) {
						if (enemy->GetPosition().y <= curPos.y + 1 && enemy->GetPosition().y >= curPos.y - 1) {
							SetForwardSpeed(-100.0f);
							mForwardSpeed = -100;
							curPos.x += goombaPos.x + (mForwardSpeed*deltaTime);
						}
					}
					curPos.y += goombaPos.y;
					mGoomba->SetPosition(curPos);
				}
			}
		}

		for (auto block : (mGoomba->GetGame())->mBlocks) {
			float goombaX = mGoomba->GetPosition().x;
			float goombaY = mGoomba->GetPosition().y;
			Vector2 goombaPos = Vector2(goombaX, goombaY);

			CollSide cs = (mGoomba->cc)->GetMinOverlap(block->cc, goombaPos);
			Vector2 curPos = mGoomba->GetPosition();
			if (cs != CollSide::None) {
				if (cs == CollSide::Top && mYSpeed > 0.0f) {
					mYSpeed = 0.0f;
					mInAir = false;
				}
				else if (cs == CollSide::Bottom && mYSpeed < 0) {
					mYSpeed = 0.0f;
				}
				curPos.y += goombaPos.y;
				mGoomba->SetPosition(curPos);
			}
		}
		mYSpeed += 2000.0f * deltaTime;

		for (auto block : (mGoomba->GetGame())->mBlocks) {
			float goombaX = mGoomba->GetPosition().x;
			float goombaY = mGoomba->GetPosition().y;
			Vector2 goombaPos = Vector2(goombaX, goombaY);

			CollSide cs = (mGoomba->cc)->GetMinOverlap(block->cc, goombaPos);
			Vector2 curPos = mGoomba->GetPosition();
			if (cs != CollSide::None) {
				if (cs == CollSide::Right) {
					if (block->GetPosition().y <= curPos.y + 1 && block->GetPosition().y >= curPos.y - 1) {
						SetForwardSpeed(100.0f);
						mForwardSpeed = 100.0f;
						curPos.x += (mForwardSpeed*deltaTime);
					}
				}
				else if (cs == CollSide::Left) {
					if (block->GetPosition().y <= curPos.y + 1 && block->GetPosition().y >= curPos.y - 1) {
						SetForwardSpeed(-100.0f);
						mForwardSpeed = -100.0f;
						curPos.x += (mForwardSpeed*deltaTime);
					}
				}
				curPos.y += goombaPos.y;
				mGoomba->SetPosition(curPos);
			}
		}
	}

	else {
		mGoomba->as->SetAnimation("dead");
		lifetime += deltaTime;
		if (lifetime >= 0.25f) {
			mGoomba->SetState(ActorState::Destroy);
			mGoomba->as->SetIsPaused(true);
		}
	}
}