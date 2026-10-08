#pragma once
#include "KamataEngine.h"
#include "Camera.h"

using namespace KamataEngine;
class baseBall;
class gameScene{
	enum GameScene
	{ 
		basebool,
		FlappyBird,
		Western,
		RockBreaker,
		RunningFromThtPolice,
	};
	
	GameScene ganeScene_ = basebool;
	baseBall* baseBall_ = nullptr;
	GameCamera* gameCamera_ = nullptr;
public:
	~gameScene();
	void Intialize();
	void Update();
	void Draw();
};

