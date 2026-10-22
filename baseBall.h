#pragma once
#include "KamataEngine.h"
#include <array>
#include <random>
class baseBall {
public:
	enum class Judge {
		None,
		Perfect,
		Great,
		Good,
		Miss,
	};

	baseBall();
	~baseBall();
	void Initialize();
	void Reset(int level=0); // ゲーム状態を初期値に戻す(ミニゲーム切り替え時に呼ぶ)
	void Update();
	void Draw(const KamataEngine::Camera& camera);


	Judge GetLastJudge() const { return lastJudge_; }
	int GetCombo() const { return combo_; }
	int GetScore() const { return score_; }
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
		bool judged = false;
	};

	void ApplyDifficulty(int level);
	void SpawnBall();
	void UpdateBat();
	void UpdateBall();
	Judge JudgeTiming();
	void ApplyJudge(Judge judge);
	void UpdateTimingUI(); // 縮小円・判定演出の更新
	void UpdateTargetPos(const KamataEngine::Camera& camera); 

	//バッターbox君
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;

	//野球ボール
	KamataEngine::Model* baseballModel_ = nullptr;
	static inline const int kMaxBalls = 16;
	std::array<Ball, kMaxBalls>balls_;
	int spawnTimer_ = 0; // 「投球セット」を開始するまでのタイマー

	// 1回の投球セットで投げる球数(ランダム)と投げる間隔
	static constexpr int kMinPitchCount = 1;
	static constexpr int kMaxPitchCount = 3;
	static constexpr int kPitchGapFrames = 60; // 球と球の間隔(1秒)
	int pitchRemaining_ = 0; // このセットで残りの球数
	int pitchGapTimer_ = 0;  // 次の球を投げるまでのタイマー
	std::mt19937 rng_;

	//バット
	KamataEngine::Model* batModel_ = nullptr;
	KamataEngine::WorldTransform batWorldTransform_;
	bool isSwinging_ = false;
	float swingAngle_ = 0.0f;
	int swingTimer_ = 0;

	Judge lastJudge_ = Judge::None;
	int combo_ = 0;
	int score_ = 0;

	static constexpr float  kBPM = 80.0f;
	static constexpr float kFramesPerBeat = 3600.0f / kBPM;
	static constexpr int kTravelBeats = 4;
	static constexpr float kPitchDistance = 50.0f;

	static constexpr int kPitchInterval = static_cast<int>(kFramesPerBeat * 6);
	static constexpr float kPitchSpeed = kPitchDistance / (kFramesPerBeat * kTravelBeats);

	static inline int kSwingFrame = 20;

	static inline const int kContactFrame = 9;

	static inline const int kPerfectRange = 2;
	static inline const int kGreatRange = 4;
	static inline const int kGoodRange = 7;

	static constexpr int kMaxLevel = 5;
	static constexpr float kSpeedUpPerLevel = 1.0;
	int level_ = 0;
	float pitchSpeed_ = kPitchSpeed;
	int minPitchCount_ = kMinPitchCount;
	int perfectRange_ = kPerfectRange;
	int greatRange_ = kGreatRange;
	int goodRange_ = kGoodRange;

	static inline const float kBatStartAngle = 1.0f;
	static inline const float kBatendAngle = -2.0f;


	static constexpr float kTargetWorldX = 0.0f;
	static constexpr float kTargetWorldY = 2.0f;
	static constexpr float kTargetSize = 120.0f;
	static constexpr float kApproachFrame = 60.0f;
	static constexpr float kApproachStartScale = 3.0f;
	static constexpr int kFlashFrame = 20;

	static inline const int kSwingCoolDownFrame = 30;
	int swingCooldowonTimer_ = 0;

	KamataEngine::Model* ringModel_ = nullptr;
	KamataEngine::WorldTransform targetRingTransform_;
	std::array<KamataEngine::WorldTransform, kMaxBalls>approachRingTransforms_;
	KamataEngine::WorldTransform flashRingTransform_;

	std::array<bool, kMaxBalls> approachRingActive_ = {};
	bool flashRingActive_ = false;

	int flashTimer_ = 0;
};