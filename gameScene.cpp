
#include "gameScene.h"
#include "FlappyBird.h"
#include "GameBackground.h"
#include "KamataEngine.h"
#include "baseBall.h"
#include "ganbattle.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace KamataEngine;

static const char* SceneName(int s) {
	switch (s) {
	case 0:
		return "basebool";
	case 1:
		return "FlappyBird";
	case 2:
		return "Western";
	default:
		return "Unknown";
	}
}

// デストラクタ
gameScene::~gameScene() {
	delete baseBall_;
	delete gameCamera_;
	delete flappyBirdGame_;
	delete gameBackground_;
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

	// Western(早撃ちゲーム)
	ganbattle_ = new ganbattle();
	ganbattle_->Initialize();
}

void gameScene::NextMiniGame() {
	miniGameTimer_ = 0;

	switch (ganeScene_) {
	case gameScene::basebool:
		ganeScene_ = FlappyBird;
		flappyBirdGame_->Reset();
		break;
	case gameScene::FlappyBird:
		ganeScene_ = Western;
		ganbattle_->Start();
		break;
	case gameScene::Western:
		ganeScene_ = basebool;
		loopCount_++;
		baseBall_->Reset(loopCount_);
		break;
	default:
		break;
	}
}

void gameScene::Update() {
	Input* input = Input::GetInstance();
	const float groundY = -10.0f;
	gameCamera_->Update();

	switch (scene_) {
	case Title:
		if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE)) {
			scene_ = Game;
			ganeScene_ = basebool;
			loopCount_ = 0;

			// 残機を初期値の3に戻す
			lives_ = kInitialLives;

			baseBall_->Reset();
			miniGameTimer_ = 0;
			totalTimer_ = 0;
		}
		break;

	case Game:
		if (++totalTimer_ >= kTotalFrames) {
			scene_ = Clear;
			break;
		}

		// 5秒経過したら次のミニゲームへ
		if (ganeScene_ != Western && ++miniGameTimer_ >= kMiniGameFrames) {
			NextMiniGame();
		}

		// ミニゲームごとに地面の位置を設定
		switch (ganeScene_) {
		case basebool:
		{
			// 更新前のミス回数を取得
			int previousMissCount = baseBall_->GetMissCount();

			// 野球ゲームを更新
			baseBall_->Update();

			// 更新後のミス回数を取得
			int currentMissCount = baseBall_->GetMissCount();

			// 新しいミスが発生した場合
			if (currentMissCount > previousMissCount)
			{
				// 残機を1減らす
				--lives_;

				// 現在の残機をデバッグ出力
				DebugText::GetInstance()->ConsolePrintf(
					"Remaining Lives: %d\n", lives_);

				// 残機が0になった場合
				if (lives_ <= 0)
				{
					scene_ = Clear;
				}
				else
				{
					// 残機が残っていれば次のミニゲームへ
					NextMiniGame();
				}
			}
			break;
		}

		case FlappyBird:
			// 地面の高さを設定
			gameBackground_->SetYukaPosition({ 0.0f, groundY, 0.0f });
			flappyBirdGame_->SetGroundHeight(groundY);

			// FlappyBirdの更新
			flappyBirdGame_->Update();

			// 失敗した場合
			if (flappyBirdGame_->IsFailed())
			{
				// 残機を1減らす
				--lives_;

				// 現在の残機をデバッグ出力
				DebugText::GetInstance()->ConsolePrintf(
					"Remaining Lives: %d\n", lives_);

				// 残機が0になったらクリアシーンへ
				if (lives_ <= 0)
				{
					scene_ = Clear;
				}
				else
				{
					// 残機が残っていれば次のミニゲームへ
					NextMiniGame();
				}
			}
			break;

		case Western:
			// Westernの更新
			ganbattle_->Update();

			// 地面の位置を設定
			gameBackground_->SetYukaPosition({ 0.0f, 0.0f, 0.0f });

			// 結果演出が終了した場合
			if (ganbattle_->IsFinished())
			{
				// 敗北またはお手つきの場合
				if (ganbattle_->IsGameOver())
				{
					// 残機を1減らす
					--lives_;

					// 現在の残機をデバッグ出力
					DebugText::GetInstance()->ConsolePrintf(
						"Remaining Lives: %d\n", lives_);

					// 残機が0になったらClearシーンへ
					if (lives_ <= 0)
					{
						scene_ = Clear;
						break;
					}
				}

				// 勝利した場合、または残機が残っている場合
				NextMiniGame();
			}
			break;

		default:
			break;
		}		

#ifdef _DEBUG
		// デバッグ用:LSHIFTで即座に次へ
		if (input->TriggerKey(DIK_LSHIFT)) {
			NextMiniGame();
		}
#endif
		break;

	case Clear:
		if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE)) {
			scene_ = Title;
			ganeScene_ = basebool;
			miniGameTimer_ = 0;
			totalTimer_ = 0;
		}
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
			ganbattle_->Draw();
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
