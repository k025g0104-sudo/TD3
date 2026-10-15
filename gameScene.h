#pragma once
#include "KamataEngine.h"
#include "Camera.h"

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
        RockBreaker,
        RunningFromThtPolice,
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

public:
    ~gameScene();

    void Intialize();
    void Update();
    void Draw();
};