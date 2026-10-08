#pragma once
class gameScene
{
	// 大きなシーン
	enum Scene
	{
		Title,
		Game,
		Clear,
	};

	// ミニゲーム
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

	// 現在のシーン
	Scene scene_ = Title;

	// 現在のミニゲーム
	
	GameScene ganeScene_ = basebool;
	baseBall* baseBall_ = nullptr;
	GameCamera* gameCamera_ = nullptr;


	// 現在のシーン
	Scene scene_ = Title;

	// 現在のミニゲーム
	GameScene ganeScene_ = basebool;
	baseBall* baseBall_ = nullptr;
	GameCamera* gameCamera_ = nullptr;
public:
	~gameScene();
	void Intialize();
	void Update();
	void Draw();
};

