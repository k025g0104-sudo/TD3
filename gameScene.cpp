
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

            if (input->TriggerKey(DIK_SPACE))
            {
                ganeScene_ = FlappyBird;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
            break;

        case gameScene::FlappyBird:
            if (input->TriggerKey(DIK_SPACE))
            {
                ganeScene_ = Western;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
            break;

        case gameScene::Western:
            if (input->TriggerKey(DIK_SPACE))
            {
                ganeScene_ = RockBreaker;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
            break;

        case gameScene::RockBreaker:
            if (input->TriggerKey(DIK_SPACE))
            {
                ganeScene_ = RunningFromThtPolice;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
            break;

        case gameScene::RunningFromThtPolice:
            if (input->TriggerKey(DIK_SPACE))
            {
                scene_ = Clear;
            }
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

            chickModel_->Draw(
                chickWorldTransform_, gameCamera_->GetCamera());

            Model::PostDraw();
            break;

        case gameScene::FlappyBird:
            break;

        case gameScene::Western:
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
