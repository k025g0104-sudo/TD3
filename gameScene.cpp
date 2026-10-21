#include "gameScene.h"
#include "KamataEngine.h"
#include "baseBall.h"
#include "FlappyBird.h"
#include "GameBackground.h"

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

static const char* TitleScene(int scene) {
	switch (scene) {
	case 0: return "Title";
	case 1: return "Game";
	case 2: return "Clear";
	default:return "Unknown";
	}
}


gameScene::~gameScene() {
	delete baseBall_;
	delete gameCamera_;
	delete flappyBirdGame_;
	delete gameBackground_;
	delete baseballModel_;
	delete batModel_;
	delete ganModel_;
}


void gameScene::Intialize() {
	baseBall_ = new baseBall();
	baseBall_->Initialize();

	// FlappyBirdの初期化
	flappyBirdGame_ = new FlappyBirdGame();
	flappyBirdGame_->Initialize();

	// カメラの初期化
	gameCamera_ = new GameCamera();
	gameCamera_->Initialize();

	// 共通背景の初期化
	gameBackground_ = new GameBackground();
	gameBackground_->Initialize();

	// 野球ボールモデル
	baseballModel_ = Model::CreateFromOBJ("baseboll");
	baseballWorldTransform_.Initialize();

	// バットモデル
	batModel_ = Model::CreateFromOBJ("bat");
	batWorldTransform_.Initialize();

	// Ganモデルを読み込む
	ganModel_ = Model::CreateFromOBJ("ganSAA");

	// Ganのワールドトランスフォームを初期化
	ganWorldTransform_.Initialize();

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

#ifdef _DEBUG
		// デバッグ用:LSHIFTで即座に次へ
		if (input->TriggerKey(DIK_LSHIFT))
		{
			NextMiniGame();
		}
#endif
		break;

	case Clear:
		if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE))
		{
			scene_ = Title;
			ganeScene_ = basebool;
			miniGameTimer_ = 0;
			totalTimer_ = 0;
			DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", TitleScene(scene_));
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
				baseballWorldTransform_, gameCamera_->GetCamera());          */
			Model::PostDraw();
			break;

		case gameScene::FlappyBird:
			// FlappyBirdの描画
			flappyBirdGame_->Draw(gameCamera_->GetCamera());
			break;

		case gameScene::Western:
			// Ganモデルを描画
			Model::PreDraw();

			ganModel_->Draw(
				ganWorldTransform_, gameCamera_->GetCamera());

			Model::PostDraw();
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