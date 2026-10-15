
#include "gameScene.h"
#include "FlappyBird.h"
#include "GameBackground.h"
#include "KamataEngine.h"
#include "baseBall.h"
#include "ganbattle.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace KamataEngine;

// ミニゲーム名
static const char* SceneName(int s) {
    switch (s) {
    case 0: return "basebool";
    case 1: return "FlappyBird";
    case 2: return "Western";
    case 3: return "RockBreaker";
    case 4: return "RunningFromThtPolice";
    default: return "Unknown";
    }
}

// デストラクタ
gameScene::~gameScene() {
    delete baseBall_;
    delete flappyBirdGame_;
    delete gameBackground_;
    delete gameCamera_;

    delete baseballModel_;
    delete batModel_;
    delete ganbattle_;
}

// 初期化
void gameScene::Intialize() {
    // 野球ゲーム
    baseBall_ = new baseBall();
    baseBall_->Initialize();

    // FlappyBird
    flappyBirdGame_ = new FlappyBirdGame();
    flappyBirdGame_->Initialize();

    // カメラ
    gameCamera_ = new GameCamera();
    gameCamera_->Initialize();

    // 共通背景
    gameBackground_ = new GameBackground();
    gameBackground_->Initialize();

    // 野球ボールモデル
    baseballModel_ = Model::CreateFromOBJ("baseboll");
    baseballWorldTransform_.Initialize();

    // バットモデル
    batModel_ = Model::CreateFromOBJ("bat");
    batWorldTransform_.Initialize();

    // Western（早撃ちゲーム）
    ganbattle_ = new ganbattle();
    ganbattle_->Initialize();
}

// 更新
void gameScene::Update() {
    Input* input = Input::GetInstance();

    // カメラ更新
    gameCamera_->Update();

    switch (scene_) {
    case Title:
        // タイトルからゲームへ
        if (input->TriggerKey(DIK_SPACE)) {
            scene_ = Game;
        }
        break;

    case Game:
        // ミニゲームごとの地面位置
        switch (ganeScene_) {
        case basebool:
            gameBackground_->SetYukaPosition({ 0.0f, 0.0f, 0.0f });
            break;

        case FlappyBird: {
            const float groundY = -10.0f;
            gameBackground_->SetYukaPosition({ 0.0f, groundY, 0.0f });
            flappyBirdGame_->SetGroundHeight(groundY);
            break;
        }

        case Western:
        case RockBreaker:
        case RunningFromThtPolice:
            gameBackground_->SetYukaPosition({ 0.0f, 0.0f, 0.0f });
            break;

        default:
            break;
        }

        // 各ミニゲームの更新
        switch (ganeScene_) {
        case basebool:
            baseBall_->Update();

#ifdef _DEBUG
            if (input->TriggerKey(DIK_LSHIFT)) {
                ganeScene_ = FlappyBird;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
#endif
            break;

        case FlappyBird:
            flappyBirdGame_->Update();

#ifdef _DEBUG
            if (input->TriggerKey(DIK_LSHIFT)) {
                ganeScene_ = Western;
                ganbattle_->Start();

                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
#endif
            break;

        case Western:
            ganbattle_->Update();

            if (ganbattle_->IsGameOver()) {
                scene_ = Title;
                ganeScene_ = basebool;
            }
            else if (ganbattle_->IsWin()) {
                ganeScene_ = RockBreaker;
            }
            break;

        case RockBreaker:
#ifdef _DEBUG
            if (input->TriggerKey(DIK_LSHIFT)) {
                ganeScene_ = RunningFromThtPolice;
                DebugText::GetInstance()->ConsolePrintf(
                    "Scene->%s\n", SceneName(ganeScene_));
            }
#endif
            break;

        case RunningFromThtPolice:
#ifdef _DEBUG
            if (input->TriggerKey(DIK_LSHIFT)) {
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
        if (input->TriggerKey(DIK_SPACE)) {
            scene_ = Title;
            ganeScene_ = basebool;
        }
        break;

    default:
        break;
    }
}

// 描画
void gameScene::Draw() {
    switch (scene_) {
    case Title:
        // タイトル画面
        break;

    case Game:
        // 共通背景
        gameBackground_->Draw(gameCamera_->GetCamera());

        // 各ミニゲームの描画
        switch (ganeScene_) {
        case basebool:
            Model::PreDraw();

            baseBall_->Draw(gameCamera_->GetCamera());

            Model::PostDraw();
            break;

        case FlappyBird:
            flappyBirdGame_->Draw(gameCamera_->GetCamera());
            break;

        case Western:
            ganbattle_->Draw(gameCamera_->GetCamera());
            break;

        case RockBreaker:
            break;

        case RunningFromThtPolice:
            break;

        default:
            break;
        }
        break;

    case Clear:
        // クリア画面
        break;

    default:
        break;
    }
}
