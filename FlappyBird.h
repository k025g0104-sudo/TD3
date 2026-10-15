#pragma once

#include "KamataEngine.h"

// FlappyBirdの処理を管理するクラス
class FlappyBirdGame
{
public:
    // デストラクタ
    ~FlappyBirdGame();

    // 初期化
    void Initialize();

    // 更新
    void Update();

    // 描画
    void Draw(const KamataEngine::Camera& camera);

private:
    // Chickの回転・移動行列を更新
    void UpdateChickMatrix();

    // ヒヨコ
    KamataEngine::Model* chickModel_ = nullptr;
    KamataEngine::WorldTransform chickWorldTransform_;

    // Dokannモデル
    KamataEngine::Model* dokannModel_ = nullptr;

    // 下側のDokann
    KamataEngine::WorldTransform dokannWorldTransform_;

    // 上側のDokann
    KamataEngine::WorldTransform dokannUpperWorldTransform_;

    // Dokannの移動速度
    static constexpr float kDokannMoveSpeed = 0.1f;

    // Dokannが左端に到達したと判定するX座標
    static constexpr float kDokannLeftLimit = -10.0f;

    // Dokannを右側に戻すX座標
    static constexpr float kDokannStartX = 10.0f;

    // Chickの上下方向の速度
    float chickVelocityY_ = 0.0f;

    // ジャンプ時の上昇速度
    static constexpr float kChickJumpPower = 0.3f;

    // 重力
    static constexpr float kChickGravity = 0.01f;

    // Chickの当たり判定用サイズ
    static constexpr float kChickHalfWidth = 1.05f;
    static constexpr float kChickHalfHeight = 1.3f;

    // Dokannの当たり判定用サイズ
    static constexpr float kDokannHalfWidth = 1.75f;
    static constexpr float kDokannHeight = 8.86f;

    // ChickがDokannに衝突したか
    bool isChickHit_ = false;
};