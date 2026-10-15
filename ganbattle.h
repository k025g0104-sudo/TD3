#pragma once
#include "KamataEngine.h"

// 西部劇の早打ちゲーム
//  プレイヤー : 銃モデル(ganSAA)のみ表示
//  相手(CPU)  : ガンマンモデル(ganMan)
class ganbattle {
public:
	~ganbattle();

	// モデル読み込み(1回だけ)
	void Initialize();

	// 対戦を最初からやり直す(ミニゲームに入る時に呼ぶ)
	void Start();

	void Update();

	void Draw(const KamataEngine::Camera& camera);

	// ---- 結果取得 ----
	bool IsFinished() const { return finished_; } // 結果表示が終わった
	bool IsWin() const { return finished_ && result_ == Result::Win; }
	bool IsGameOver() const { return finished_ && (result_ == Result::Lose || result_ == Result::Foul); }

private:
	enum class Phase {
		Countdown, // 合図待ち
		Signal,    // 合図が出た(撃て!)
		Result,    // 結果演出
	};

	enum class Result {
		None,
		Win,  // 先に撃った
		Lose, // 相手が先に撃った
		Foul, // お手つき
	};

	void ShowResult(Result r);
	void UpdateTransforms();

	// モデル
	KamataEngine::Model* ganModel_ = nullptr;    // プレイヤーの銃
	KamataEngine::Model* ganManModel_ = nullptr; // 相手ガンマン
	KamataEngine::WorldTransform ganWorldTransform_;
	KamataEngine::WorldTransform ganManWorldTransform_;

	// 進行
	Phase phase_ = Phase::Countdown;
	Result result_ = Result::None;
	bool finished_ = false;

	float timer_ = 0.0f;         // フェーズ内の経過時間(秒)
	float countdownTime_ = 3.0f; // 合図までの時間
	float cpuReaction_ = 0.5f;   // CPUが撃つまでの反応時間
	float resultTimer_ = 0.0f;
	int lastCountPrint_ = -1;
};