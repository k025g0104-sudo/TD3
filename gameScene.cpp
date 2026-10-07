#include "gameScene.h"
#include "KamataEngine.h"
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


using namespace KamataEngine;

void gameScene::Intialize()
{
	// 野球ボールモデルを読み込む
	baseballModel_ = Model::CreateFromOBJ("baseboll");

	// 野球ボールのワールドトランスフォームを初期化
	baseballWorldTransform_.Initialize();

	// バットモデルを読み込む
	batModel_ = Model::CreateFromOBJ("bat");

	// バットのワールドトランスフォームを初期化
	batWorldTransform_.Initialize();

	// ヒヨコモデルを読み込む
	chickModel_ = Model::CreateFromOBJ("Chick");

	// ヒヨコのワールドトランスフォームを初期化
	chickWorldTransform_.Initialize();

	// 描画確認用カメラを初期化
	camera_.Initialize();

	// カメラを手前に移動
	camera_.translation_ = { 0.0f, 0.0f, -10.0f };

	// カメラ行列を更新
	camera_.UpdateMatrix();

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
	// 3Dモデル描画前処理
	Model::PreDraw();

	// 野球ボールを描画
	baseballModel_->Draw(baseballWorldTransform_, camera_);

	// バットを描画
	batModel_->Draw(batWorldTransform_, camera_);

	// ヒヨコを描画
	chickModel_->Draw(chickWorldTransform_, camera_);

	// 3Dモデル描画後処理
	Model::PostDraw();
}