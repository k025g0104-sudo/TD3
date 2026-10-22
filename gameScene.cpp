
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
		break;
	case gameScene::Western:
		ganeScene_ = basebool;
		baseBall_->Reset();
		break;
	default:
		break;
	}
}

void gameScene::Update() {
	Input* input = Input::GetInstance();

	gameCamera_->Update();

	switch (scene_) {
	case Title:
		if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE)) {
			scene_ = Game;
			ganeScene_ = basebool;
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
		case Western:
			gameBackground_->SetYukaPosition({0.0f, 0.0f, 0.0f});
			break;

		case FlappyBird: {
			const float groundY = -10.0f;
			gameBackground_->SetYukaPosition({0.0f, groundY, 0.0f});
			flappyBirdGame_->SetGroundHeight(groundY);
			break;
		}
		default:
			break;
		}

		// ミニゲームの更新
		switch (ganeScene_) {
		case basebool:
			baseBall_->Update();
			break;
		case FlappyBird:
			flappyBirdGame_->Update();
			break;
		case Western:
			ganbattle_->Update();
			if (ganbattle_->IsFinished()) {
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

// 2D描画(Sprite::PreDraw ~ PostDraw の間で呼ばれる)
void gameScene::DrawUI() {
	if (scene_ == Game && ganeScene_ == Western) {
		ganbattle_->DrawUI();
	}
}
