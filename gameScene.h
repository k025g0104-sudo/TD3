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

    // 野球ゲーム
    baseBall* baseBall_ = nullptr;

    // カメラ
    GameCamera* gameCamera_ = nullptr;

    // 野球ボール
    KamataEngine::Model* baseballModel_ = nullptr;
    KamataEngine::WorldTransform baseballWorldTransform_;

    // バット
    KamataEngine::Model* batModel_ = nullptr;
    KamataEngine::WorldTransform batWorldTransform_;

    // ヒヨコ
    KamataEngine::Model* chickModel_ = nullptr;
    KamataEngine::WorldTransform chickWorldTransform_;

    // FlappyBird用のDokannモデル
    KamataEngine::Model* dokannModel_ = nullptr;
    KamataEngine::WorldTransform dokannWorldTransform_;

    // 上側のDokann用ワールドトランスフォーム
    KamataEngine::WorldTransform dokannUpperWorldTransform_;

    // Dokannの移動速度
    static constexpr float kDokannMoveSpeed = 0.1f;

    // Dokannが左側に消えたと判定するX座標
    static constexpr float kDokannLeftLimit = -10.0f;

    // Dokannを右側に戻すX座標
    static constexpr float kDokannStartX = 10.0f;

    // Western用のGanモデル
    KamataEngine::Model* ganModel_ = nullptr;
    KamataEngine::WorldTransform ganWorldTransform_;

    // ヒヨコのジャンプ処理
    float chickVelocityY_ = 0.0f;

    // ジャンプ時の上昇速度
    static constexpr float kChickJumpPower = 0.3f;

    // 重力
    static constexpr float kChickGravity = 0.01f;

    // Chickの当たり判定用サイズ（半幅・半高さ）
    static constexpr float kChickHalfWidth = 1.05f;
    static constexpr float kChickHalfHeight = 1.3f;

    // Dokannの当たり判定用サイズ（半幅）
    static constexpr float kDokannHalfWidth = 1.75f;

    // Dokannの当たり判定用サイズ（高さ）
    static constexpr float kDokannHeight = 8.86f;

    // ChickがDokannに衝突したか
    bool isChickHit_ = false;

public:
    ~gameScene();

    void Intialize();
    void Update();
    void Draw();
};