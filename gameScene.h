#pragma once
class gameScene{
	enum GameScene
	{
		basebool,
		FlappyBird,
		Western,
		RockBreaker,
		RunningFromThtPolice,
	};

	GameScene ganeScene_ = basebool;
public:
	void Intialize();
	void Update();
	void Draw();
};

