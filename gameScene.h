#pragma once
class gameScene
{
	// 大きなシーン
	enum Scene
	{
		Title,
		Game,
		Clear,
	};

	// ミニゲーム
	enum GameScene
	{ 

		basebool,
		FlappyBird,
		Western,
		RockBreaker,
		RunningFromThtPolice,
	};

	// 現在のシーン
	Scene scene_ = Title;

	// 現在のミニゲーム
	GameScene ganeScene_ = basebool;

public:
	void Intialize();
	void Update();
	void Draw();
};

