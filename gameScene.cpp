#include "gameScene.h"
#include "KamataEngine.h"
#include "baseBall.h"
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


using namespace KamataEngine;

static const char* SceneName(int s) {
	switch (s) {
	case 0: return "basebool";
	case 1: return "Flapybird";
	case 2: return "Western";
	case 3: return "RockBreaker";
	case 4: return "RunningFromThtPolice";
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

	gameCamera_ = new GameCamera();
	gameCamera_->Initialize();
}

void gameScene::Update() {
	Input* input = Input::GetInstance();

	gameCamera_->Update();

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
		if (input->TriggerKey(DIK_SPACE)) {
			ganeScene_ = Western;
			DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
		}
		break;
	case gameScene::Western:
		if (input->TriggerKey(DIK_SPACE)) {
			ganeScene_ = RockBreaker;
			DebugText::GetInstance()->ConsolePrintf("Scene->%s\n", SceneName(ganeScene_));
		}
		break;
	case gameScene::RockBreaker:
		if (input->TriggerKey(DIK_SPACE)) {
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
}

void gameScene::Draw() {
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
}
