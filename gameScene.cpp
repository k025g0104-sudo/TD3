
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
}


void gameScene::Intialize() {
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

}

void gameScene::Draw() {
	switch (scene_)
	{
	case gameScene::Title:
		break;
	case gameScene::Game:
		switch (ganeScene_)
		{
		case gameScene::basebool:
			baseBall_->Draw(gameCamera_->GetCamera());
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
	case gameScene::Clear:
		break;
	default:
		break;
	}

}
