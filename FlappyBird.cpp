#include "FlappyBird.h"
#include "KamataEngine.h"
#include <random>

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

    // 3組のDokannを初期化
    for (size_t i = 0; i < kDokannCount; ++i)
    {
        // 土管のX座標を一定間隔で配置
        float x = kDokannStartX + static_cast<float>(i) * kDokannSpacing;

        // 下側のDokann
        dokannWorldTransforms_[i].Initialize();
        dokannWorldTransforms_[i].translation_ = { x, -12.0f, 0.0f };

        dokannWorldTransforms_[i].matWorld_ =
            MathUtility::MakeTranslateMatrix(
                dokannWorldTransforms_[i].translation_);

        dokannWorldTransforms_[i].TransferMatrix();

        // 上側のDokann
        dokannUpperWorldTransforms_[i].Initialize();
        dokannUpperWorldTransforms_[i].translation_ = { x, 14.0f, 0.0f };

        // 上下反転
        dokannUpperWorldTransforms_[i].scale_ = { 1.0f, -1.0f, 1.0f };

        dokannUpperWorldTransforms_[i].matWorld_ =
            MathUtility::MakeTranslateMatrix(
                dokannUpperWorldTransforms_[i].translation_);

        dokannUpperWorldTransforms_[i].matWorld_.m[1][1] = -1.0f;

        dokannUpperWorldTransforms_[i].TransferMatrix();
    }

    // ゲーム状態を初期化
    Reset();
}

// FlappyBirdのゲーム状態を初期値に戻す
void FlappyBirdGame::Reset()
{
    // Chickの位置と速度を初期化
    chickWorldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
    chickVelocityY_ = 0.0f;

    // Chickの行列を更新
    UpdateChickMatrix();

    // 乱数生成器を用意（初回のみ初期化）
    static std::mt19937 randomEngine(std::random_device{}());

    // 土管の高さを変更する乱数
    static std::uniform_real_distribution<float> randomOffset(-3.0f, 3.0f);

    // 3組のDokannの位置を初期化
    for (size_t i = 0; i < kDokannCount; ++i)
    {
        // 初期配置のX座標
        float x = kDokannStartX + static_cast<float>(i) * kDokannSpacing;

        // 土管の高さをランダムに変更
        float offsetY = randomOffset(randomEngine);

        // 下側のDokann
        dokannWorldTransforms_[i].translation_ = { x, -12.0f + offsetY, 0.0f };

        dokannWorldTransforms_[i].matWorld_ =
            MathUtility::MakeTranslateMatrix(
                dokannWorldTransforms_[i].translation_);

        dokannWorldTransforms_[i].TransferMatrix();

        // 上側のDokann
        dokannUpperWorldTransforms_[i].translation_ = { x, 14.0f + offsetY, 0.0f };

        dokannUpperWorldTransforms_[i].matWorld_ =
            MathUtility::MakeTranslateMatrix(
                dokannUpperWorldTransforms_[i].translation_);

        // 上下反転を維持
        dokannUpperWorldTransforms_[i].matWorld_.m[1][1] = -1.0f;

        dokannUpperWorldTransforms_[i].TransferMatrix();
    }

    // 衝突状態を解除
    isChickHit_ = false;
}

void FlappyBirdGame::Update()
{
    Input* input = Input::GetInstance();

    // 衝突済みなら更新を停止する
    if (isChickHit_)
    {
        if (input->TriggerKey(DIK_R))
        {
            Reset();
        }

        return;
    }

    // SpaceキーまたはZキーを押した瞬間に上向きの速度を与える
    if (input->TriggerKey(DIK_SPACE) ||
        input->TriggerKey(DIK_Z))
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

    // 乱数生成器を用意（初回のみ初期化）
    static std::mt19937 randomEngine(std::random_device{}());

    // 土管の高さを変更する乱数
    static std::uniform_real_distribution<float> randomOffset(-3.0f, 3.0f);

    // 3組のDokannを移動・更新
    for (size_t i = 0; i < kDokannCount; ++i)
    {
        // 下側のDokannを左へ移動
        dokannWorldTransforms_[i].translation_.x -= kDokannMoveSpeed;

        // 上側のDokannも同じX座標にする
        dokannUpperWorldTransforms_[i].translation_.x =
            dokannWorldTransforms_[i].translation_.x;

        // 左端まで移動したら最後尾に配置する
        if (dokannWorldTransforms_[i].translation_.x < kDokannLeftLimit)
        {
            // 他の土管の中で最も右にあるX座標を探す
            float rightmostX = dokannWorldTransforms_[i].translation_.x;

            for (size_t j = 0; j < kDokannCount; ++j)
            {
                if (dokannWorldTransforms_[j].translation_.x > rightmostX)
                {
                    rightmostX = dokannWorldTransforms_[j].translation_.x;
                }
            }

            // 最後尾のさらに右側へ配置
            float newX = rightmostX + kDokannSpacing;

            dokannWorldTransforms_[i].translation_.x = newX;
            dokannUpperWorldTransforms_[i].translation_.x = newX;

            // 上下の土管の間隔を維持したまま高さをランダムに変更
            float offsetY = randomOffset(randomEngine);

            dokannWorldTransforms_[i].translation_.y = -12.0f + offsetY;
            dokannUpperWorldTransforms_[i].translation_.y = 14.0f + offsetY;
        }

        // 下側のDokannの行列を更新
        dokannWorldTransforms_[i].matWorld_ =
            MathUtility::MakeTranslateMatrix(
                dokannWorldTransforms_[i].translation_);

        dokannWorldTransforms_[i].TransferMatrix();

        // 上側のDokannの行列を更新
        dokannUpperWorldTransforms_[i].matWorld_ =
            MathUtility::MakeTranslateMatrix(
                dokannUpperWorldTransforms_[i].translation_);

        // 上下反転を維持
        dokannUpperWorldTransforms_[i].matWorld_.m[1][1] = -1.0f;

        dokannUpperWorldTransforms_[i].TransferMatrix();
    }

    // Chickの位置
    float chickX = chickWorldTransform_.translation_.x;
    float chickY = chickWorldTransform_.translation_.y + 4.3f;

    // 3組のDokannとの衝突判定
    for (size_t i = 0; i < kDokannCount; ++i)
    {
        // DokannのX座標
        float dokannX = dokannWorldTransforms_[i].translation_.x;

        // X方向の当たり判定
        bool hitX =
            (chickX + kChickHalfWidth > dokannX - kDokannHalfWidth) &&
            (chickX - kChickHalfWidth < dokannX + kDokannHalfWidth);

        // 下側のDokannの上端
        float lowerDokannTop =
            dokannWorldTransforms_[i].translation_.y + 8.29f;

        // 上側のDokannの下端
        float upperDokannBottom =
            dokannUpperWorldTransforms_[i].translation_.y - 8.29f;

        // Chickと下側のDokannの当たり判定
        bool hitLower =
            hitX &&
            (chickY - kChickHalfHeight < lowerDokannTop);

        // Chickと上側のDokannの当たり判定
        bool hitUpper =
            hitX &&
            (chickY + kChickHalfHeight > upperDokannBottom);

        // どちらかの土管に衝突した場合
        if (hitLower || hitUpper)
        {
            // デバッグ出力
            DebugText::GetInstance()->ConsolePrintf(
                "Chick Hit Dokann! Index:%d Lower:%d Upper:%d\n",
                static_cast<int>(i), hitLower, hitUpper);

            DebugText::GetInstance()->ConsolePrintf(
                "ChickY:%.2f LowerTop:%.2f UpperBottom:%.2f\n",
                chickY, lowerDokannTop, upperDokannBottom);

            // 衝突状態にする
            isChickHit_ = true;

            // 衝突したので判定を終了
            break;
        }
    }
}

// 描画
void FlappyBirdGame::Draw(const Camera& camera)
{
    Model::PreDraw();

    // Chick
    chickModel_->Draw(
        chickWorldTransform_, camera);

    // 3組のDokannを描画
    for (size_t i = 0; i < kDokannCount; ++i)
    {
        // 下側のDokann
        dokannModel_->Draw(
            dokannWorldTransforms_[i], camera);

        // 上側のDokann
        dokannModel_->Draw(
            dokannUpperWorldTransforms_[i], camera);
    }

    Model::PostDraw();
}