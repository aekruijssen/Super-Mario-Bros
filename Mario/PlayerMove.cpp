#include "PlayerMove.h"
#include "Actor.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Block.h"
#include "Math.h"
#include "Player.h"
#include "Goomba.h"
#include "AnimatedSprite.h"
#include "SDL/SDL_mixer.h"

// MoveComponent constructor - initialize order to 50 and speeds to 0
PlayerMove::PlayerMove(class Player* owner)
	:MoveComponent(owner)
	, mAngularSpeed(0.0f)
	, mForwardSpeed(0.0f)
{
	mPlayer = owner;
	mPlayerVel.x = 300;
	mPlayerVel.y = 0;
	mYSpeed = 0.0f;

	mSpacePressed = false;
	mInAir = false;

	mGame = mPlayer->GetGame();
}

// Use the left and right arrow keys to modify the forward speed.For now, just set it to
// 300.0f for right and -300.0f for left, and 0 if neither left nor right(use SetForwardSpeed)
void PlayerMove::ProcessInput(const Uint8 *keyState) {
	if (keyState[SDL_SCANCODE_RIGHT]) {
		SetForwardSpeed(300.0f);
		mForwardSpeed = 300.0f;
	}
	else if (keyState[SDL_SCANCODE_LEFT]) {
		SetForwardSpeed(-300.0f);
		mForwardSpeed = -300.0f;
	}
	else {
		SetForwardSpeed(0);
		mForwardSpeed = 0;
	}

	if(keyState[SDL_SCANCODE_SPACE] && !mSpacePressed && !mInAir){
		mYSpeed = -700.0f;
		mInAir = true;
		Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Jump.wav"), 0);
	}
	// At the end of ProcessInput, save the current keyState value in
	mSpacePressed = keyState[SDL_SCANCODE_SPACE];
}

// Update - updates position/rotation of Actor
void PlayerMove::Update(float deltaTime) {
	if (mPlayer->GetState() == ActorState::Active) {
		Vector2 pos = mOwner->GetPosition();
		pos.x += mForwardSpeed * deltaTime;
		pos.y += mYSpeed * deltaTime;
		mOwner->SetPosition(pos);

		if (mPlayer->GetPosition().x > 6368) {
			Mix_HaltChannel(mGame->mChannel);
			Mix_PlayChannel(-1, mGame->GetSound("Assets/Mario/StageCleared.wav"), 0);
			mPlayer->SetState(ActorState::Paused);
		}

		else if (mPlayer->GetPosition().y > 448.0f) {
			mPlayer->ChangeTexture("Assets/Mario/Dead.png");
			Mix_HaltChannel(mGame->mChannel);
			Mix_PlayChannel(-1, mGame->GetSound("Assets/Mario/Dead.wav"), 0);
			mPlayer->SetState(ActorState::Paused);
		}

		if (mPlayer->GetState() == ActorState::Active) {
			// Block Collision
			bool collide = false;
			for (auto block : (mPlayer->GetGame())->mBlocks) {
				float blockX = block->GetPosition().x;
				float blockY = block->GetPosition().y;
				Vector2* blockPos = new Vector2(blockX, blockY);
				float playerX = mPlayer->GetPosition().x;
				float playerY = mPlayer->GetPosition().y;
				Vector2 playerPos = Vector2(playerX, playerY);

				CollSide cs = (mPlayer->cc)->GetMinOverlap(block->cc, playerPos);
				// Didn’t collide with the block, so move on
				if (cs != CollSide::None) { 

					// Landing on top of block, so set mYSpeed = 0, which stops Mario from moving down
					if (cs == CollSide::Top && mYSpeed > 0.0f) {
						mYSpeed = 0.0f;
						mInAir = false;
					}
					else if (cs == CollSide::Bottom && mYSpeed < 0)
					{
						mYSpeed = 0.0f;
						Mix_PlayChannel(-1, mPlayer->GetGame()->GetSound("Assets/Sounds/Bump.wav"), 0);
					}

					// Change the player’s position by the offset vector you get from GetMinOverlap
					Vector2 curPos = mPlayer->GetPosition();
					curPos.x += playerPos.x;
					curPos.y += playerPos.y;
					mPlayer->SetPosition(curPos);
					collide = true;
				}
			}

			if (collide == false) {
				mInAir = true;
			}
			if (mPlayer->GetPosition().y > 488.0f) {
				mInAir = false;
			}

			// Goomba Stomped IF Mario hits the top of the Goomba OR Mario hits the left / right of the Goomba AND mInAir is true
			for (auto enemy : (mPlayer->GetGame())->mGoombas) {
				if (enemy->stomped == false) {
					float enemyX = enemy->GetPosition().x;
					float enemyY = enemy->GetPosition().y;
					Vector2* enemyPos = new Vector2(enemyX, enemyY);
					float playerX = mPlayer->GetPosition().x;
					float playerY = mPlayer->GetPosition().y;
					Vector2 playerPos = Vector2(playerX, playerY);

					CollSide cs = (mPlayer->cc)->GetMinOverlap(enemy->cc, playerPos);
					if (cs != CollSide::None) {
						if (cs == CollSide::Top && mYSpeed > 0.0f) {
							mYSpeed = -350.0f;
							mInAir = true;
							enemy->stomped = true;
							Mix_PlayChannel(-1, mPlayer->GetGame()->GetSound("Assets/Sounds/Stomp.wav"), 0);
						}
						else if (cs == CollSide::Left && mInAir == true)
						{
							mYSpeed = -350.0f;
							enemy->stomped = true;
						}
						else if (cs == CollSide::Right && mInAir == true)
						{
							mYSpeed = -350.0f;
							enemy->stomped = true;
						}
						else {
							mPlayer->ChangeTexture("Assets/Mario/Dead.png");
							mPlayer->SetState(ActorState::Paused);
						}
						Vector2 curPos = mPlayer->GetPosition();
						curPos.x += playerPos.x;
						curPos.y += playerPos.y;
						mPlayer->SetPosition(curPos);
						collide = true;
					}
				}
			}

			mYSpeed += 2000.0f * deltaTime;

			// Don’t change camera.x if you would decrease camera.x
			if ((mPlayer->GetPosition().x - 300) >= mPlayer->GetGame()->GetCameraPos().x) {
				mPlayer->GetGame()->SetCameraPos(mPlayer->GetPosition().x - 300);
			}
			// Don’t let the camera.x go lower than 0 (to fix the problem at the start)
			if (mPlayer->GetGame()->GetCameraPos().x <= 0) {
				mPlayer->GetGame()->SetCameraPos(0);
			}
			// Don’t let the player.x ever be less than the camera.x
			else if (mPlayer->GetPosition().x <= mPlayer->GetGame()->GetCameraPos().x) {
				Vector2 vec = mPlayer->GetPosition();
				vec.x = mPlayer->GetGame()->GetCameraPos().x;
				mPlayer->SetPosition(vec);
			}
		}
	}
	UpdateAnim();
}

void PlayerMove::UpdateAnim() {
	if (mPlayer->GetState() == ActorState::Paused && mPlayer->GetPosition().x < 6368) {
		mPlayer->as->SetAnimation("dead");
	}
	else if (mInAir == false) {
		if (mForwardSpeed > 0) {
			mPlayer->as->SetAnimation("runRight");
		}
		else if(mForwardSpeed < 0) {
			mPlayer->as->SetAnimation("runLeft");
		}
		else {
			mPlayer->as->SetAnimation("idle");
		}
	}
	else if (mForwardSpeed > 0) {
		mPlayer->as->SetAnimation("jumpRight");
	}
	else if (mForwardSpeed < 0) {
		mPlayer->as->SetAnimation("jumpLeft");
	}
	else if((mPlayer->as->GetAnimName()) == "runLeft" || (mPlayer->as->GetAnimName()) == "jumpLeft"){
		mPlayer->as->SetAnimation("jumpLeft");
	}
	else {
		mPlayer->as->SetAnimation("jumpRight");
	}
}