#include "FlappyBird.h"
#include "KamataEngine.h"

using namespace KamataEngine;

// デストラクタ
FlappyBirdGame::~FlappyBirdGame()
{
    // Chickモデルの解放
    delete chickModel_;

    // Dokannモデルの解放
    delete dokannModel_;
}

// Chickの回転・移動行列を更新
void FlappyBirdGame::UpdateChickMatrix()
{
    // Chickの回転行列を作成
    Matrix4x4 rotateMatrix =
        MathUtility::MakeRotateYMatrix(chickWorldTransform_.rotation_.y);

    // Chickの移動行列を作成
    Matrix4x4 translateMatrix =
        MathUtility::MakeTranslateMatrix(chickWorldTransform_.translation_);

    // 回転と移動を合成する
    chickWorldTransform_.matWorld_ =
        MathUtility::operator*(rotateMatrix, translateMatrix);

    // 描画用の行列を転送
    chickWorldTransform_.TransferMatrix();
}

// 地面の上面の高さを設定
void FlappyBirdGame::SetGroundHeight(float height)
{
    groundHeight_ = height;
}

// 初期化
void FlappyBirdGame::Initialize()
{
    // ヒヨコモデル
    chickModel_ = Model::CreateFromOBJ("Chick");
    chickWorldTransform_.Initialize();

    // Chickを右向きに回転させる
    chickWorldTransform_.rotation_.y = -3.14159265f / 2.0f;

    // Chickの行列を更新
    UpdateChickMatrix();

    // Dokannモデルの読み込み
    dokannModel_ = Model::CreateFromOBJ("Dokann");

    // 下側のDokann
    dokannWorldTransform_.Initialize();
    dokannWorldTransform_.translation_ = { 10.0f, -12.0f, 0.0f };

    dokannWorldTransform_.matWorld_ =
        MathUtility::MakeTranslateMatrix(dokannWorldTransform_.translation_);

    dokannWorldTransform_.TransferMatrix();

    // 上側のDokann
    dokannUpperWorldTransform_.Initialize();
    dokannUpperWorldTransform_.translation_ = { 10.0f, 14.0f, 0.0f };

    // 上下反転用のスケール
    dokannUpperWorldTransform_.scale_ = { 1.0f, -1.0f, 1.0f };

    // 平行移動行列を作成
    dokannUpperWorldTransform_.matWorld_ =
        MathUtility::MakeTranslateMatrix(dokannUpperWorldTransform_.translation_);

    // Y方向を反転
    dokannUpperWorldTransform_.matWorld_.m[1][1] = -1.0f;

    dokannUpperWorldTransform_.TransferMatrix();

    // Chickの速度と衝突状態を初期化
    chickVelocityY_ = 0.0f;
    isChickHit_ = false;
}


void FlappyBirdGame::Update()
{
    Input* input = Input::GetInstance();

    // 衝突済みなら更新を停止する
    if (isChickHit_)
    {
        // Rキーでリスタート
        if (input->TriggerKey(DIK_R))
        {
            // Chickの位置と速度を初期化
            chickWorldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
            chickVelocityY_ = 0.0f;

            // Chickの行列を更新
            UpdateChickMatrix();

            // Dokannの位置を初期化
            dokannWorldTransform_.translation_ = { 10.0f, -12.0f, 0.0f };
            dokannUpperWorldTransform_.translation_ = { 10.0f, 14.0f, 0.0f };

            // 下側のDokannの行列を更新
            dokannWorldTransform_.matWorld_ =
                MathUtility::MakeTranslateMatrix(dokannWorldTransform_.translation_);
            dokannWorldTransform_.TransferMatrix();

            // 上側のDokannの行列を更新
            dokannUpperWorldTransform_.matWorld_ =
                MathUtility::MakeTranslateMatrix(dokannUpperWorldTransform_.translation_);

            dokannUpperWorldTransform_.matWorld_.m[1][1] = -1.0f;
            dokannUpperWorldTransform_.TransferMatrix();

            // 衝突状態を解除
            isChickHit_ = false;
        }

        return;
    }

    // Spaceキーを押した瞬間に上向きの速度を与える
    if (input->TriggerKey(DIK_SPACE))
    {
        chickVelocityY_ = kChickJumpPower;
    }

    // 重力によって落下速度を増加させる
    chickVelocityY_ -= kChickGravity;

    // 上下方向の位置を更新
    chickWorldTransform_.translation_.y += chickVelocityY_;

    // Chickの当たり判定の中心Y座標
    float chickCenterY = chickWorldTransform_.translation_.y + 4.3f;

    // Chickの足元のY座標
    float chickBottomY = chickCenterY - kChickHalfHeight;

    // 地面に接触したか判定
    if (chickBottomY <= groundHeight_)
    {
        // 地面より下にめり込まないよう位置を補正
        chickWorldTransform_.translation_.y =
            groundHeight_ + kChickHalfHeight - 4.3f;

        // 落下速度を停止
        chickVelocityY_ = 0.0f;

        // ゲームオーバー状態にする
        isChickHit_ = true;

        // デバッグ出力
        DebugText::GetInstance()->ConsolePrintf(
            "Chick Hit Ground!\n");

        // Chickの行列を更新
        UpdateChickMatrix();

        return;
    }

    // Chickの行列を更新
    UpdateChickMatrix();

    // 下側のDokannを左へ移動
    dokannWorldTransform_.translation_.x -= kDokannMoveSpeed;

    // 上側のDokannも同じX座標にする
    dokannUpperWorldTransform_.translation_.x =
        dokannWorldTransform_.translation_.x;

    // 左端まで移動したら右側へ戻す
    if (dokannWorldTransform_.translation_.x < kDokannLeftLimit)
    {
        dokannWorldTransform_.translation_.x = kDokannStartX;
        dokannUpperWorldTransform_.translation_.x = kDokannStartX;
    }

    // 下側のDokannの行列を更新
    dokannWorldTransform_.matWorld_ =
        MathUtility::MakeTranslateMatrix(dokannWorldTransform_.translation_);

    dokannWorldTransform_.TransferMatrix();

    // 上側のDokannの行列を更新
    dokannUpperWorldTransform_.matWorld_ =
        MathUtility::MakeTranslateMatrix(dokannUpperWorldTransform_.translation_);

    // 上下反転を維持する
    dokannUpperWorldTransform_.matWorld_.m[1][1] = -1.0f;

    dokannUpperWorldTransform_.TransferMatrix();

    // Chickの位置
    float chickX = chickWorldTransform_.translation_.x;
    float chickY = chickWorldTransform_.translation_.y + 4.3f;

    // DokannのX座標
    float dokannX = dokannWorldTransform_.translation_.x;

    // X方向の当たり判定
    bool hitX =
        (chickX + kChickHalfWidth > dokannX - kDokannHalfWidth) &&
        (chickX - kChickHalfWidth < dokannX + kDokannHalfWidth);

    // 下側のDokannの上端
    float lowerDokannTop =
        dokannWorldTransform_.translation_.y + 8.29f;

    // 上側のDokannの下端
    float upperDokannBottom =
        dokannUpperWorldTransform_.translation_.y - 8.29f;

    // Chickと下側のDokannの当たり判定
    bool hitLower =
        hitX &&
        (chickY - kChickHalfHeight < lowerDokannTop);

    // Chickと上側のDokannの当たり判定
    bool hitUpper =
        hitX &&
        (chickY + kChickHalfHeight > upperDokannBottom);

    // 衝突判定
    bool isHit = hitLower || hitUpper;

    // 衝突した瞬間だけデバッグ出力
    if (isHit && !isChickHit_)
    {
        DebugText::GetInstance()->ConsolePrintf(
            "Chick Hit Dokann! Lower:%d Upper:%d\n",
            hitLower, hitUpper);

        DebugText::GetInstance()->ConsolePrintf(
            "ChickY:%.2f LowerTop:%.2f UpperBottom:%.2f\n",
            chickY, lowerDokannTop, upperDokannBottom);
    }

    isChickHit_ = isHit;
}

// 描画
void FlappyBirdGame::Draw(const Camera& camera)
{
    Model::PreDraw();

    // Chick
    chickModel_->Draw(
        chickWorldTransform_, camera);

    // 下側のDokann
    dokannModel_->Draw(
        dokannWorldTransform_, camera);

    // 上側のDokann
    dokannModel_->Draw(
        dokannUpperWorldTransform_, camera);

    Model::PostDraw();
}