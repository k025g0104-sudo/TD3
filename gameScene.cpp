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
	case 3:
		return "RockBreaker";
	case 4:
		return "RunningFromThtPolice";
	default:
		return "Unknown";
	}
}

gameScene::~gameScene() {
	delete baseBall_;
	delete flappyBirdGame_;
	delete gameBackground_;
	delete gameCamera_;

	delete baseballModel_;
	delete batModel_;
	delete ganbattle_;
}

void gameScene::Intialize() {
	// 野球ゲームの初期化
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

	   // Western(早打ちゲーム)の初期化
	ganbattle_ = new ganbattle();
	ganbattle_->Initialize();
}

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
		// ミニゲームごとに地面の位置を設定
		switch (ganeScene_) {
		case gameScene::basebool:
			gameBackground_->SetYukaPosition({0.0f, 0.0f, 0.0f});
			break;

		case gameScene::FlappyBird: {
			// FlappyBird用の地面の高さ
			const float groundY = -10.0f;

			// 地面の位置を設定
			gameBackground_->SetYukaPosition({0.0f, groundY, 0.0f});

			// Chickの接触判定に地面の高さを渡す
			flappyBirdGame_->SetGroundHeight(groundY);
			break;
		}

		case gameScene::Western:
			gameBackground_->SetYukaPosition({0.0f, 0.0f, 0.0f});
			break;

		case gameScene::RockBreaker:
			gameBackground_->SetYukaPosition({0.0f, 0.0f, 0.0f});
			break;

		case gameScene::RunningFromThtPolice:
			gameBackground_->SetYukaPosition({0.0f, 0.0f, 0.0f});
			break;

		default:
			break;
		}

		// ミニゲームの切り替え
		switch (ganeScene_) {
		case gameScene::basebool:
			baseBall_->Update();
        
void gameScene::Update()
{
	Input* input = Input::GetInstance();

	gameCamera_->Update();
	switch (scene_)
	{
	case gameScene::Title:
		if (input->TriggerKey(DIK_RETURN)) {
			scene_ = Game;
			DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", TitleScene(scene_));
		}
		break;
	case gameScene::Game:
		if (input->TriggerKey(DIK_RETURN)) {
			scene_ = Clear;
			DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", TitleScene(scene_));
		}
		switch (ganeScene_)
		{
		case gameScene::basebool:
			 baseBall_->Update();
			if (input->TriggerKey(DIK_SPACE)) {
				ganeScene_ = FlappyBird;
				DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
			}
			break;
		case gameScene::FlappyBird:
			if (input->TriggerKey(DIK_SPACE))
			{
				ganeScene_ = Western;
				DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
			}
			break;
		case gameScene::Western:
			if (input->TriggerKey(DIK_SPACE))
			{
				ganeScene_ = RockBreaker;
				DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
			}
			break;
		case gameScene::RockBreaker:
			if (input->TriggerKey(DIK_SPACE))
			{
				ganeScene_ = RunningFromThtPolice;
				DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
			}
			break;
		case gameScene::RunningFromThtPolice:
			if (input->TriggerKey(DIK_SPACE)) {
				ganeScene_ = basebool;
				DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
			}
			break;
		default:
			break;
		}
		break;
	case gameScene::Clear:
		if (input->TriggerKey(DIK_RETURN)) {
			scene_ = Title;
			DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", TitleScene(scene_));
		}
		break;
	default:
		break;
	}

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
        // ミニゲームごとに地面の位置を設定
        switch (ganeScene_)
        {
        case gameScene::basebool:
            gameBackground_->SetYukaPosition({ 0.0f, 0.0f, 0.0f });
            break;

        case gameScene::FlappyBird:
        {
            // FlappyBird用の地面の高さ
            const float groundY = -10.0f;

            // 地面の位置を設定
            gameBackground_->SetYukaPosition({ 0.0f, groundY, 0.0f });

            // Chickの接触判定に地面の高さを渡す
            flappyBirdGame_->SetGroundHeight(groundY);
            break;
        }

        case gameScene::Western:
            gameBackground_->SetYukaPosition({ 0.0f, 0.0f, 0.0f });
            break;

        case gameScene::RockBreaker:
            gameBackground_->SetYukaPosition({ 0.0f, 0.0f, 0.0f });
            break;

        case gameScene::RunningFromThtPolice:
            gameBackground_->SetYukaPosition({ 0.0f, 0.0f, 0.0f });
            break;

        default:
            break;
        }

        // ミニゲームの切り替え
        switch (ganeScene_)
        {
        case gameScene::basebool:
            baseBall_->Update();


#ifdef _DEBUG
			if (input->TriggerKey(DIK_LSHIFT)) {
				ganeScene_ = FlappyBird;
				DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
			}
#endif
			break;

		case gameScene::FlappyBird: {
			// FlappyBirdの更新
			flappyBirdGame_->Update();

#ifdef _DEBUG
			// デバッグ用シーン切り替え
			if (input->TriggerKey(DIK_LSHIFT)) {
				ganeScene_ = Western;
				ganbattle_->Start();
				DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
			}
#endif

			break;
		}
		case gameScene::Western:
#ifdef _DEBUG
			ganbattle_->Update();

			if (ganbattle_->IsGameOver()) {
				scene_ = Title;
				ganeScene_ = basebool;
			} else if (ganbattle_->IsWin()) {
				ganeScene_ = RockBreaker;
			}
#endif
			break;

		case gameScene::RockBreaker:
#ifdef _DEBUG
			if (input->TriggerKey(DIK_LSHIFT)) {
				ganeScene_ = RunningFromThtPolice;
				DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
			}
#endif
			break;

		case gameScene::RunningFromThtPolice:
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


void gameScene::Draw() {
	switch (scene_) {
	case Title:
		// タイトル画面（今後実装）
		break;

	case Game:
		// 全ミニゲーム共通の背景を描画
		gameBackground_->Draw(gameCamera_->GetCamera());

		// 各ミニゲームの描画
		switch (ganeScene_) {
		case gameScene::basebool:
			// チームメンバーの野球ゲーム描画
			baseBall_->Draw(gameCamera_->GetCamera());

			// 3Dモデル描画
			Model::PreDraw();

			baseballModel_->Draw(baseballWorldTransform_, gameCamera_->GetCamera());

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

			batModel_->Draw(batWorldTransform_, gameCamera_->GetCamera());

			Model::PostDraw();
			break;

		case gameScene::FlappyBird:
			// FlappyBirdの描画
			flappyBirdGame_->Draw(gameCamera_->GetCamera());
			break;

		case gameScene::Western:
			// Ganモデルを描画
			ganbattle_->Draw(gameCamera_->GetCamera());
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