
#pragma once
#include "KamataEngine.h"
#include "Camera.h"

class baseBall;

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
	baseBall* baseBall_ = nullptr;
	GameCamera* gameCamera_ = nullptr;
	
public:
	~gameScene();

	void Intialize();
	void Update();
	void Draw();
};
