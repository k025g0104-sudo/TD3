#pragma once
#include "KamataEngine.h"
#include "Camera.h"
#include <array>

class baseBall;
class FlappyBirdGame;
class GameBackground;
class ganbattle;

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
	};

	// 現在のシーン
	Scene scene_ = Title;

	// 現在のミニゲーム
	GameScene ganeScene_ = basebool;

	// 野球ゲーム
	baseBall* baseBall_ = nullptr;

	// FlappyBird
	FlappyBirdGame* flappyBirdGame_ = nullptr;

	// 共通背景
	GameBackground* gameBackground_ = nullptr;

	// カメラ
	GameCamera* gameCamera_ = nullptr;

	// 野球ボール
	KamataEngine::Model* baseballModel_ = nullptr;
	KamataEngine::WorldTransform baseballWorldTransform_;

	// バット
	KamataEngine::Model* batModel_ = nullptr;
	KamataEngine::WorldTransform batWorldTransform_;

	// Western(早打ちゲーム)
	ganbattle* ganbattle_ = nullptr;

	int miniGameTimer_ = 0;
	static constexpr int kMiniGameFrames = 60 * 5;

	int loopCount_ = 0;

	// プレイヤーの初期残機
	static constexpr int kInitialLives = 3;

	// プレイヤーの現在の残機
	int lives_ = kInitialLives;

	// 残機表示用のハートモデル
	KamataEngine::Model* heartModel_ = nullptr;

	// 残機表示用ハートの最大数
	static constexpr int kHeartCount = 3;

	// ハート3個分のワールド座標
	std::array<KamataEngine::WorldTransform, kHeartCount> heartWorldTransforms_;

	// 残機表示専用カメラ
	KamataEngine::Camera heartCamera_;

	int totalTimer_ = 0;
	static constexpr int kTotalFrames = 60 * 90;

	void NextMiniGame();

public:
	~gameScene();

	void Intialize();
	void Update();
	void Draw();	
};