#include "gameScene.h"
#include "KamataEngine.h"
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


using namespace KamataEngine;

void gameScene::Intialize() {}
void gameScene::Update() {
	Input* input = Input::GetInstance();
	switch (ganeScene_)
	{
	case gameScene::basebool:
		if (input->TriggerKey(DIK_SPACE)) {
			ganeScene_ = FlappyBird;
		}
		break;
	case gameScene::FlappyBird:
		if (input->TriggerKey(DIK_SPACE)) {
			ganeScene_ = Western;
		}
		break;
	case gameScene::Western:
		if (input->TriggerKey(DIK_SPACE)) {
			ganeScene_ = RockBreaker;
		}
		break;
	case gameScene::RockBreaker:
		if (input->TriggerKey(DIK_SPACE)) {
			ganeScene_ = RunningFromThtPolice;
		}
		break;
	case gameScene::RunningFromThtPolice:
		if (input->TriggerKey(DIK_SPACE)) {
			ganeScene_ = basebool;
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
		printf("basebool\n");
		break;
	case gameScene::FlappyBird:
		printf("FlappyBird\n");
		break;
	case gameScene::Western:
		printf("Western\n");
		break;
	case gameScene::RockBreaker:
		printf("RockBreaker\n");	
		break;
	case gameScene::RunningFromThtPolice:
		printf("RunningFromThtPolice\n");
		break;
	default:
		break;
	}
}
