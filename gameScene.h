#pragma once
#include "KamataEngine.h"

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

	// 野球ボール
	KamataEngine::Model* baseballModel_ = nullptr;
	KamataEngine::WorldTransform baseballWorldTransform_;

	// バット
	KamataEngine::Model* batModel_ = nullptr;
	KamataEngine::WorldTransform batWorldTransform_;

	// ヒヨコ
	KamataEngine::Model* chickModel_ = nullptr;
	KamataEngine::WorldTransform chickWorldTransform_;

	// 描画確認用カメラ
	KamataEngine::Camera camera_;

public:
	void Intialize();
	void Update();
	void Draw();
};