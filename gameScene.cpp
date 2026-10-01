#include "gameScene.h"
#include "KamataEngine.h"
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


using namespace KamataEngine;

void gameScene::Intialize()
{
	
}
void gameScene::Update()
{
	Input* input = Input::GetInstance();

	switch (scene_)
	{
	case Title:
		// タイトルでSPACEを押したらゲームへ
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
		if (input->TriggerKey(DIK_SPACE)) 
		{
			ganeScene_ = FlappyBird;
		}
		break;
	case gameScene::FlappyBird:
		if (input->TriggerKey(DIK_SPACE))
		{
			ganeScene_ = Western;
		}
		break;
	case gameScene::Western:
		if (input->TriggerKey(DIK_SPACE)) 
		{
			ganeScene_ = RockBreaker;
		}
		break;
	case gameScene::RockBreaker:
		if (input->TriggerKey(DIK_SPACE)) 
		{
			ganeScene_ = RunningFromThtPolice;
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
		// SPACEを押したらタイトルへ戻る
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
	
}
