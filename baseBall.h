#pragma once
#include "KamataEngine.h"
#include <array>

class baseBall {
public:
	baseBall();
	~baseBall();
	void Initialize();
	void Update();
	void Draw(const KamataEngine::Camera& camera);

private:
	//ボール
	enum class BallState {
		Pitched,//投げられた状態
		Hit,//打たれた状態
	};

	struct Ball {
		KamataEngine::WorldTransform worldTrnasform;
		KamataEngine::Vector3 velocity = { 0.0f,0.0f,0.0f };
		BallState state = BallState::Pitched;
		int timer = 0;
		bool active = false;
	};

	void SpawnBall();
	void UpdateBat();
	void UpdateBall();

	//バッターbox君
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;

	//野球ボール
	KamataEngine::Model* baseballModel_ = nullptr;
	static inline const int kMaxBalls = 16;
	std::array<Ball, kMaxBalls>balls_;
	int spawnTimer_ = 0;			

	//バット
	KamataEngine::Model* batModel_ = nullptr;
	KamataEngine::WorldTransform batWorldTransform_;
	bool isSwinging_ = false;
	float swingAngle_ = 0.0f;
	int swingTimer_ = 0;

	static inline const int kPitchInterval = 30;
	static inline const int kSwingFrame = 20;
	static inline const int kHitStartFrame = 5;
	static inline const int kHitEndFrame = 13;
	static inline const float kHitRange = 1.0f;
	static inline const float kPitchSpeed = 0.3f;

	static inline const float kBatStartAngle = 1.0f;
	static inline const float kBatendAngle = -2.0f;
};

