#include "gameScene.h"
#include "KamataEngine.h"
#include "baseBall.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace KamataEngine;

static const char* SceneName(int s)
{
    switch (s)
    {
    case 0: return "basebool";
    case 1: return "FlappyBird";
    case 2: return "Western";
    case 3: return "RockBreaker";
    case 4: return "RunningFromThtPolice";
    default: return "Unknown";
    }
}

gameScene::~gameScene()
{
    delete baseBall_;
    delete gameCamera_;

    delete baseballModel_;
    delete batModel_;
    delete chickModel_;
    delete ganModel_;
    delete dokannModel_;
}

void gameScene::Intialize()
{
    // 野球ゲームの初期化
    baseBall_ = new baseBall();
    baseBall_->Initialize();

    // カメラの初期化
    gameCamera_ = new GameCamera();
    gameCamera_->Initialize();

    // 野球ボールモデル
    baseballModel_ = Model::CreateFromOBJ("baseboll");
    baseballWorldTransform_.Initialize();

    // バットモデル
    batModel_ = Model::CreateFromOBJ("bat");
    batWorldTransform_.Initialize();

    // ヒヨコモデル
    chickModel_ = Model::CreateFromOBJ("Chick");
    chickWorldTransform_.Initialize();

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

    // 上下を反転するため、Y方向のスケールをマイナスにする
    dokannUpperWorldTransform_.scale_ = { 1.0f, -1.0f, 1.0f };

    // 平行移動行列を作成
    dokannUpperWorldTransform_.matWorld_ =
        MathUtility::MakeTranslateMatrix(dokannUpperWorldTransform_.translation_);

    // Y方向を反転
    dokannUpperWorldTransform_.matWorld_.m[1][1] = -1.0f;

    dokannUpperWorldTransform_.TransferMatrix();

    // Ganモデルを読み込む
    ganModel_ = Model::CreateFromOBJ("ganSAA");

    // Ganのワールドトランスフォームを初期化
    ganWorldTransform_.Initialize();

}

void gameScene::Update()
{
    Input* input = Input::GetInstance();

    // カメラ更新
    gameCamera_->Update();

    switch (scene_)
    {
    case Title:
        // タイトルからゲームへ
        if (input->TriggerKey(DIK_SPACE))
        {
            scene_ = Game;
        }

        break;

    case Game:
        // ミニゲームの切り替え
        switch (ganeScene_)
        {
        case gameScene::basebool:
            baseBall_->Update();

#ifdef _DEBUG
            if (input->TriggerKey(DIK_LSHIFT))
            {
                ganeScene_ = FlappyBird;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
#endif
            break;

        case gameScene::FlappyBird:
        {
            // 衝突済みなら更新を停止する
            if (isChickHit_)
            {
                // Rキーでリスタート
                if (input->TriggerKey(DIK_R))
                {
                    // Chickの位置と速度を初期化
                    chickWorldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
                    chickVelocityY_ = 0.0f;

                    chickWorldTransform_.matWorld_ =
                        MathUtility::MakeTranslateMatrix(chickWorldTransform_.translation_);
                    chickWorldTransform_.TransferMatrix();

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

                break;
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

            // 座標から平行移動行列を作成
            chickWorldTransform_.matWorld_ =
                MathUtility::MakeTranslateMatrix(chickWorldTransform_.translation_);

            // ワールド行列を転送
            chickWorldTransform_.TransferMatrix();

            // 下側のDokannを左へ移動
            dokannWorldTransform_.translation_.x -= kDokannMoveSpeed;

            // 上側のDokannも同じX座標にする
            dokannUpperWorldTransform_.translation_.x =
                dokannWorldTransform_.translation_.x;

            // 左端まで移動したら右側へ戻す
            if (dokannWorldTransform_.translation_.x < kDokannLeftLimit) {
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
            // OBJモデルのY座標は約 -0.57 ～ 8.29
            float lowerDokannTop =
                dokannWorldTransform_.translation_.y + 8.29f;

            // 上側のDokannの下端
            // 上下反転しているため、元の上端が下端になる
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

#ifdef _DEBUG
            // デバッグ用シーン切り替え
            if (input->TriggerKey(DIK_LSHIFT))
            {
                ganeScene_ = Western;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
#endif
            break;
        }
        case gameScene::Western:
#ifdef _DEBUG
            if (input->TriggerKey(DIK_LSHIFT))
            {
                ganeScene_ = RockBreaker;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
#endif
            break;

        case gameScene::RockBreaker:
#ifdef _DEBUG
            if (input->TriggerKey(DIK_LSHIFT))
            {
                ganeScene_ = RunningFromThtPolice;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
#endif
            break;

        case gameScene::RunningFromThtPolice:
#ifdef _DEBUG
            if (input->TriggerKey(DIK_LSHIFT))
            {
                scene_ = Clear;
            }
#endif
            break;

        default:
            break;
        }
        break;

    case Clear:
        // クリアからタイトルへ
        if (input->TriggerKey(DIK_SPACE))
        {
            scene_ = Title;
            ganeScene_ = basebool;
        }
        break;

    default:
        break;
    }
}

void gameScene::Draw()
{
    switch (scene_)
    {
    case Title:
        // タイトル画面（今後実装）
        break;

    case Game:
        switch (ganeScene_)
        {
        case gameScene::basebool:
            // チームメンバーの野球ゲーム描画
            baseBall_->Draw(gameCamera_->GetCamera());

            // 3Dモデル描画
            Model::PreDraw();

            baseballModel_->Draw(
                baseballWorldTransform_, gameCamera_->GetCamera());

            batModel_->Draw(
                batWorldTransform_, gameCamera_->GetCamera());

            Model::PostDraw();
            break;

        case gameScene::FlappyBird:
            Model::PreDraw();

            // Chick
            chickModel_->Draw(
                chickWorldTransform_, gameCamera_->GetCamera());

            // 下側のDokann
            dokannModel_->Draw(
                dokannWorldTransform_, gameCamera_->GetCamera());

            // 上側のDokann
            dokannModel_->Draw(
                dokannUpperWorldTransform_, gameCamera_->GetCamera());

            Model::PostDraw();
            break;

        case gameScene::Western:
            // Ganモデルを描画
            Model::PreDraw();

            ganModel_->Draw(
                ganWorldTransform_, gameCamera_->GetCamera());

            Model::PostDraw();
            break;

        case gameScene::RockBreaker:
            break;

        case gameScene::RunningFromThtPolice:
            break;

        default:
            break;
        }
        break;

    case Clear:
        // クリア画面（今後実装）
        break;

    default:
        break;
    }
}