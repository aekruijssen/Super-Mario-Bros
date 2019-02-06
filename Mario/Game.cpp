#include "Game.h"
#include "Actor.h"
#include "Block.h"
#include "Player.h"
#include "SpriteComponent.h"
#include "Random.h"
#include "Goomba.h"
#include "Spawner.h"
#include "SDL/SDL_mixer.h"

#include <cmath>
#include <SDL/SDL_image.h>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <string>


// Game constructor - initialize renderer and window to null, running bool to true
Game::Game() {
	mRenderer = nullptr;
	mWindow = nullptr;
	mIsRunning = true;
}


// Initialize - SDL video/audio, create window/renderer, Random, SDL Image, ticks, and call LoadData()
bool Game::Initialize() {
	// Initialize sdl video and audio
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0) {
		SDL_Log("Unable to initialize SDL Video: %s", SDL_GetError());
		return false;
	}
	sdlResult = SDL_Init(SDL_INIT_AUDIO);
	if (sdlResult != 0) {
		SDL_Log("Unable to initialize SDL Audio: %s", SDL_GetError());
		return false;
	}

	// Create Window
	mWindow = SDL_CreateWindow(
		"Mario - Adina Kruijssen",
		100,
		100,
		600,
		448,
		0
	);

	if (!mWindow) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create Renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, 
		-1, 
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mRenderer) {
		SDL_Log("Unable to create renderer: %s", SDL_GetError());
		return false;
	}

	//Random::Init();

	//initialize SDL Image with IMG_Init
	IMG_Init(IMG_INIT_PNG);

	mTicksCount = 0;
	cameraPos.x = 0;
	cameraPos.y = 0;

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	Game::LoadData();

	return true;
}


// ShutDown  - quit SDL Image, unload data (delete Actors/Textures), destroy renderer/window, quit
void Game::ShutDown() {
	Mix_CloseAudio();
	IMG_Quit();
	UnloadData();
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

Mix_Chunk* Game::GetSound(const std::string& filename) {
	mChunk = Mix_LoadWAV(filename.c_str());
	return mChunk;
}


// Run Loop - basic game loop (process input -> update game -> generate output)
void Game::RunLoop() {
	while (mIsRunning) {
		processInput();
		updateGame();
		generateOutput();
	}
}


// Process Input - check if exiting/running, get keyboard state, Process Input for all Actors
void Game::processInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				mIsRunning = false; 
				break;
		}
	}
	// Get keyboard state
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}

	// Make a copy of the actor vector. 
	std::vector<Actor*> mActorsCopy = mActors;
	// Loop over the copy of all actors and call ProcessInput on each.
	for (auto actor : mActorsCopy) {
		actor->ProcessInput(state);
	}
}


// Generate Output - background, swap, draw sprites
void Game::generateOutput() {
	SDL_SetRenderDrawColor(
		mRenderer,
		0,
		0,
		255,
		255
	);
	SDL_RenderClear(mRenderer);
	
	SDL_SetRenderDrawColor(
		mRenderer,
		255,
		255,
		255,
		255
	);
	//draw sprites
	for (auto sprite : mSprites) {
		if (sprite->IsVisible()) {
			sprite->Draw(mRenderer);
		}
	}
	//present 
	SDL_RenderPresent(mRenderer);
}


//Update game - update ticks, deltaTime, and Actors
void Game::updateGame() {
	mTicksCount = SDL_GetTicks();
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.033f) {
		deltaTime = 0.033f;
	}

	//Make a copy(just a shallow copy using = ) of the actor vector
	std::vector<Actor*> mActorsCopy = mActors;
	//Loop over the copy and call Update on each actor
	for (auto actor : mActorsCopy) {
		actor->Update(deltaTime);
	}
	//Make a temporary Actor* vector for actors to destroy
	std::vector<Actor*> mActorsDestroy;
	//Loop over the actor vector, and add any actors in state ActorState::Destroy to temp vec
	for (auto actor : mActors) {
		if (actor->GetState() == ActorState::Destroy) {
			mActorsDestroy.emplace_back(actor);
		}
	}
	//Loop over the vector from step 3 and delete each actor in it
	for (auto actor : mActorsDestroy) {
		delete actor;
	}
}

void Game::LoadLevel() {
	
	int mChannel = Mix_PlayChannel(-1, GetSound("Assets/Sounds/Music.ogg"), -1);

	//The block in the top left corner is centered at (16, 16), Each block is 32x32

	char x;
	std::ifstream inFile;

	inFile.open("assets/Level1.txt");
	if (!inFile) {
		std::cout << "Unable to open file";
		exit(1); // terminate with error
	}
	int xPos = 16;
	int yPos = 16;
	int row = 0;

	std::string line = "";
	while (inFile >> line) {
		for (int i = 0; i < line.length(); i++) {
			char type = line[i];
			if (type == 'P') {
				mPlayer = new Player(this);
				Vector2 playerPos = Vector2(xPos + (i*32), yPos + (row*32));
				mPlayer->SetPosition(playerPos);
			}
			else if (type == 'Y') {
				Spawner* spawner = new Spawner(this);
				Vector2 spawnerPos = Vector2(xPos + (i * 32), yPos + (row * 32));
				spawner->SetPosition(spawnerPos);
			}
			else if (type == '.') {
			}
			else {
				std::string path = "Assets/Block";
				path += type;
				path += ".png";
				Block* block = new Block(this);
				Vector2 blockPos = Vector2(xPos + (i*32), yPos + (row*32));
				block->SetPosition(blockPos);
				block->ChangeTexture(path.c_str());
			}
		}
		row++;
	}
	inFile.close();
}

Player* Game::GetPlayer() {
	return mPlayer;
}

// Load data - create actors (Ship, stars background, 10 Asteroids
void Game::LoadData() {
	Actor* background = new Actor(this);
	const Vector2 bgPos = Vector2(3392, 224);
	background->SetPosition(bgPos);
	SpriteComponent* bgSprite = new SpriteComponent(background, 90);
	bgSprite->SetTexture(GetTexture("assets/background.png"));

	Game::LoadLevel();
}


// Unload data - delete Actors and Textures
void Game::UnloadData() {

	Mix_FreeChunk(mChunk);

	while (!mActors.empty()) {
		delete mActors.back();
	}
	mActors.clear();

	for (auto texture : mTextures) {
		SDL_DestroyTexture(std::get<1>(texture));
	}
	mTextures.clear();
}



// Add/Remove functions for Actors, Textures, and Sprite Components:

void Game::AddActor(Actor* actor) {
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor) {
	auto search = std::find(std::begin(mActors), std::end(mActors), actor);
	if (search != std::end(mActors)) {
		mActors.erase(search);
	}
}

void Game::AddBlock(Block* block) {
	mBlocks.emplace_back(block);
}

void Game::RemoveBlock(Block* block) {
	auto search = std::find(std::begin(mBlocks), std::end(mBlocks), block);
	if (search != std::end(mBlocks)) {
		mBlocks.erase(search);
	}
}

void Game::AddSprite(SpriteComponent* sprite) {
	mSprites.emplace_back(sprite);
	std::sort(mSprites.begin(), mSprites.end(),
		[](SpriteComponent* a, SpriteComponent* b) {
		return a->GetDrawOrder() < b->GetDrawOrder();
	});
}

void Game::RemoveSprite(SpriteComponent* sprite) {
	auto search = std::find(std::begin(mSprites), std::end(mSprites), sprite);
	if (search != std::end(mSprites)) {
		mSprites.erase(search);
	}
}

void Game::AddGoomba(Goomba* goomba) {
	mGoombas.emplace_back(goomba);
}

void Game::RemoveGoomba(Goomba* goomba) {
	auto search = std::find(std::begin(mGoombas), std::end(mGoombas), goomba);
	if (search != std::end(mGoombas)) {
		mGoombas.erase(search);
	}
}

// Returns texture from filename
SDL_Texture* Game::GetTexture(std::string filename) {
	auto search = mTextures.find(filename.c_str());
	if (search != mTextures.end()) {
		return mTextures[filename];
		//SDL_Log();
	}
	else {
		SDL_Surface *image;
		image = IMG_Load(filename.c_str());
		SDL_Texture* tex = SDL_CreateTextureFromSurface(mRenderer, image);
		SDL_FreeSurface(image);
		mTextures.insert({ filename, tex });
		return mTextures[filename.c_str()];
	}
	return nullptr;
}


vector2 Game::GetCameraPos() {
	return cameraPos;
}


void Game::SetCameraPos(float newX) {
	cameraPos.x = newX;
}