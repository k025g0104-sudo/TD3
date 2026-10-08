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

    // Ganモデルを読み込む
    ganModel_ = Model::CreateFromOBJ("Gan");

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
            // Chickモデルを描画
            Model::PreDraw();

            chickModel_->Draw(
                chickWorldTransform_, gameCamera_->GetCamera());

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