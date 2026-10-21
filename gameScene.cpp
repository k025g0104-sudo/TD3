
#include "gameScene.h"
#include "FlappyBird.h"
#include "GameBackground.h"
#include "KamataEngine.h"
#include "baseBall.h"
#include "ganbattle.h"

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
	default: return "Unknown";
	}
}

// ミニゲーム名
static const char* SceneName(int s) {
    switch (s) {
    case 0: return "basebool";
    case 1: return "FlappyBird";
    case 2: return "Western";   
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

	// カメラの初期化
	gameCamera_ = new GameCamera();
	gameCamera_->Initialize();
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

void gameScene::NextMiniGame() {
	miniGameTimer_ = 0;

	switch (ganeScene_)
	{
	case gameScene::basebool:
		ganeScene_ = FlappyBird;
		break;
	case gameScene::FlappyBird:
		ganeScene_ = Western;
		break;
	case gameScene::Western:
		ganeScene_ = basebool;
		break;
	default:
		break;
	}
}

void gameScene::Update()
{
	Input* input = Input::GetInstance();

	gameCamera_->Update();

	switch (scene_)
	{
	case Title:
		if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE))
		{
			scene_ = Game;
			ganeScene_ = basebool;
			miniGameTimer_ = 0; // タイマーリセット
			totalTimer_ = 0;
			DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", TitleScene(scene_));
		}
		break;

	case Game:
		// RETURNで強制的にクリアへ(従来の挙動を維持)
		if (++totalTimer_>=kTotalFrames)
		{
			scene_ = Clear;
			DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", TitleScene(scene_));
			break;
		}

		// 5秒経過したら次のミニゲームへ
		if (++miniGameTimer_ >= kMiniGameFrames)
		{
			NextMiniGame();			
		}

		// ミニゲームごとに地面の位置を設定
		switch (ganeScene_)
		{
		case basebool:
		case Western:
			gameBackground_->SetYukaPosition({ 0.0f, 0.0f, 0.0f });
			break;
    case Game:
        // ミニゲームごとの地面位置
        switch (ganeScene_) {
        case basebool:
            gameBackground_->SetYukaPosition({ 0.0f, 0.0f, 0.0f });
            break;

		case FlappyBird:
		{
			const float groundY = -10.0f;
			gameBackground_->SetYukaPosition({ 0.0f, groundY, 0.0f });
			flappyBirdGame_->SetGroundHeight(groundY);
			break;
		}
		default: break;
		}

		// ミニゲームの更新
		switch (ganeScene_)
		{
		case basebool:   baseBall_->Update();       break;
		case FlappyBird: flappyBirdGame_->Update(); break;
		default: break;
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
        // 全ミニゲーム共通の背景を描画
        gameBackground_->Draw(gameCamera_->GetCamera());

        // 各ミニゲームの描画
        switch (ganeScene_)
        {
        case gameScene::basebool:
           

            // 3Dモデル描画
            Model::PreDraw();
            // チームメンバーの野球ゲーム描画
            baseBall_->Draw(gameCamera_->GetCamera());

            /*baseballModel_->Draw(
            ganbattle_->Draw(gameCamera_->GetCamera());
            Model::PostDraw();
			break;

		case gameScene::FlappyBird:
			// FlappyBirdの描画
			flappyBirdGame_->Draw(gameCamera_->GetCamera());
            flappyBirdGame_->Draw(gameCamera_->GetCamera());
            break;

			// Ganモデルを描画
			Model::PreDraw();

			ganModel_->Draw(
				ganWorldTransform_, gameCamera_->GetCamera());

			Model::PostDraw();
			break;
		default:
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