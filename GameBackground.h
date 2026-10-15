#pragma once

#include "KamataEngine.h"

// 全ミニゲーム共通の背景を管理するクラス
class GameBackground
{
public:
    // デストラクタ
    ~GameBackground();

    // 初期化
    void Initialize();

    // 描画
    void Draw(const KamataEngine::Camera& camera);

    // 地面の位置を変更
    void SetYukaPosition(const KamataEngine::Vector3& position);

private:
    // 天球モデル
    KamataEngine::Model* skyModel_ = nullptr;
    KamataEngine::WorldTransform skyWorldTransform_;

    // 地面モデル
    KamataEngine::Model* yukaModel_ = nullptr;
    KamataEngine::WorldTransform yukaWorldTransform_;
};