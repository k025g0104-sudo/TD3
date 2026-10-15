#include "GameBackground.h"
#include "KamataEngine.h"

using namespace KamataEngine;

// デストラクタ
GameBackground::~GameBackground()
{
    // 天球モデルの解放
    delete skyModel_;

    // 地面モデルの解放
    delete yukaModel_;
}

// 初期化
void GameBackground::Initialize()
{
    // 天球モデルの読み込み
    skyModel_ = Model::CreateFromOBJ("Sky");
    skyWorldTransform_.Initialize();

    // 天球を拡大
    skyWorldTransform_.scale_ = { 100.0f, 100.0f, 100.0f };

    // 地面モデルの読み込み
    yukaModel_ = Model::CreateFromOBJ("Yuka");
    yukaWorldTransform_.Initialize();

    // 地面を拡大
    yukaWorldTransform_.scale_ = { 10.0f, 1.0f, 10.0f };

    // 天球のワールド行列を更新
    skyWorldTransform_.matWorld_ =
        MathUtility::MakeScaleMatrix(skyWorldTransform_.scale_);

    skyWorldTransform_.TransferMatrix();

    // 地面のワールド行列を更新
    yukaWorldTransform_.matWorld_ =
        MathUtility::MakeScaleMatrix(yukaWorldTransform_.scale_);

    yukaWorldTransform_.TransferMatrix();
}

// 描画
void GameBackground::Draw(const KamataEngine::Camera& camera)
{
    // 3Dモデルの描画開始
    KamataEngine::Model::PreDraw();

    // 天球の描画
    skyModel_->Draw(skyWorldTransform_, camera);

    // 地面の描画
    yukaModel_->Draw(yukaWorldTransform_, camera);

    // 3Dモデルの描画終了
    KamataEngine::Model::PostDraw();
}

// 地面の位置を変更
void GameBackground::SetYukaPosition(const KamataEngine::Vector3& position)
{
    // 地面の位置を設定
    yukaWorldTransform_.translation_ = position;

    // 拡大縮小行列を作成
    KamataEngine::Matrix4x4 scaleMatrix =
        KamataEngine::MathUtility::MakeScaleMatrix(
            yukaWorldTransform_.scale_);

    // 平行移動行列を作成
    KamataEngine::Matrix4x4 translateMatrix =
        KamataEngine::MathUtility::MakeTranslateMatrix(
            yukaWorldTransform_.translation_);

    // ワールド行列を更新
    yukaWorldTransform_.matWorld_ =
        KamataEngine::MathUtility::operator*(
            scaleMatrix, translateMatrix);

    // GPUに転送
    yukaWorldTransform_.TransferMatrix();
}