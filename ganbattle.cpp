#include "ganbattle.h"
#include <cmath>
#include <random>

using namespace KamataEngine;

namespace {
// ===== 調整用の定数(見え方が合わなければここだけ触る) =====
constexpr float kDt = 1.0f / 60.0f; // 1フレームの時間
constexpr float kPi = 3.14159265f;

// カウント
constexpr float kCountBase = 3.0f;   // 最低待ち時間
constexpr float kCountRandom = 1.5f; // さらにランダムで足す最大秒数

// CPUの反応時間(秒) 短いほど強い
constexpr float kCpuReactMin = 0.35f;
constexpr float kCpuReactMax = 0.65f;

// 結果表示の長さ
constexpr float kResultTime = 1.8f;

// 相手ガンマン(モデルは高さ約7)
const Vector3 kManPos = {0.0f, 0.0f, 30.0f};
constexpr float kManRotY = kPi; // プレイヤー側を向かせる(逆なら 0.0f)
constexpr float kManScale = 1.0f;

// プレイヤーの銃(モデルは全長約16なので縮小)
const Vector3 kGunPos = {1.5f, 1.0f, 12.0f};
constexpr float kGunRotY = kPi; // 銃口を相手(+Z)へ向ける(逆なら 0.0f)
constexpr float kGunScale = 0.3f;
} // namespace

ganbattle::~ganbattle() {
	delete ganModel_;
	delete ganManModel_;
}
// WorldTransformの行列を更新して転送する(拡縮 → X回転 → Y回転 → 移動)
static void UpdateWorldMatrix(WorldTransform& wt) {
	// 拡縮行列
	Matrix4x4 s = MathUtility::MakeTranslateMatrix({0.0f, 0.0f, 0.0f}); // 単位行列として使う
	s.m[0][0] = wt.scale_.x;
	s.m[1][1] = wt.scale_.y;
	s.m[2][2] = wt.scale_.z;

	// X軸回転行列
	Matrix4x4 rx = MathUtility::MakeTranslateMatrix({0.0f, 0.0f, 0.0f});
	float c = std::cos(wt.rotation_.x);
	float sn = std::sin(wt.rotation_.x);
	rx.m[1][1] = c;
	rx.m[1][2] = sn;
	rx.m[2][1] = -sn;
	rx.m[2][2] = c;

	// Y軸回転行列・移動行列
	Matrix4x4 ry = MathUtility::MakeRotateYMatrix(wt.rotation_.y);
	Matrix4x4 t = MathUtility::MakeTranslateMatrix(wt.translation_);

	// 合成して転送
	wt.matWorld_ = MathUtility::operator*(MathUtility::operator*(MathUtility::operator*(s, rx), ry), t);
	wt.TransferMatrix();
}

void ganbattle::Initialize() {
	// Resources/ganSAA/ganSAA.obj , Resources/ganMan/ganMan.obj
	ganModel_ = Model::CreateFromOBJ("ganSAA");
	ganManModel_ = Model::CreateFromOBJ("ganMan");

	ganWorldTransform_.Initialize();
	ganManWorldTransform_.Initialize();

	Start();
}

void ganbattle::Start() {
	static std::mt19937 rng{std::random_device{}()};
	std::uniform_real_distribution<float> countDist(0.0f, kCountRandom);
	std::uniform_real_distribution<float> reactDist(kCpuReactMin, kCpuReactMax);

	phase_ = Phase::Countdown;
	result_ = Result::None;
	finished_ = false;
	timer_ = 0.0f;
	resultTimer_ = 0.0f;
	lastCountPrint_ = -1;

	countdownTime_ = kCountBase + countDist(rng); // 合図までの時間
	cpuReaction_ = reactDist(rng);                // CPUの反応時間

	UpdateTransforms();
}

void ganbattle::ShowResult(Result r) {
	result_ = r;
	phase_ = Phase::Result;
	resultTimer_ = 0.0f;

	DebugText* dt = DebugText::GetInstance();
	switch (r) {
	case Result::Win:
		dt->ConsolePrintf("YOU WIN! (%.3f sec)\n", timer_);
		break;
	case Result::Lose:
		dt->ConsolePrintf("GAME OVER : 相手が先に撃った\n");
		break;
	case Result::Foul:
		dt->ConsolePrintf("GAME OVER : お手つき\n");
		break;
	default:
		break;
	}
}

void ganbattle::Update() {
	Input* input = Input::GetInstance();
	const bool pressed = input->TriggerKey(DIK_SPACE);
	switch (phase_) {
	case Phase::Countdown: {
		timer_ += kDt;

		// 残り秒数を1秒ごとに表示
		int remain = static_cast<int>(std::ceil(countdownTime_ - timer_));
		if (remain != lastCountPrint_ && remain > 0 && remain <= 3) {
			lastCountPrint_ = remain;
			DebugText::GetInstance()->ConsolePrintf("%d...\n", remain);
		}

		// お手つき
		if (pressed) {
			ShowResult(Result::Foul);
			break;
		}

		// 合図
		if (timer_ >= countdownTime_) {
			phase_ = Phase::Signal;
			timer_ = 0.0f;
			DebugText::GetInstance()->ConsolePrintf("DRAW!!\n");
		}
		break;
	}

	UpdateTransforms();
}

void ganbattle::UpdateTransforms() {
	// ---------- 相手ガンマン ----------
	Vector3 manPos = kManPos;
	Vector3 manRot = {0.0f, kManRotY, 0.0f};
	float manScale = kManScale;

	if (phase_ == Phase::Signal) {
		// 合図: ピョンと跳ねて知らせる
		manPos.y += 1.5f * std::sin(std::fmin(timer_ / 0.15f, 1.0f) * kPi);
	} else if (phase_ == Phase::Result) {
		float t = std::fmin(resultTimer_ / 0.6f, 1.0f);
		if (result_ == Result::Win) {
			// 撃たれて後ろに倒れる
			manRot.x = -kPi * 0.5f * t;
		} else if (result_ == Result::Lose) {
			// 勝ち誇ってわずかに前へ
			manPos.z -= 2.0f * t;
		}
	}

	ganManWorldTransform_.translation_ = manPos;
	ganManWorldTransform_.rotation_ = manRot;
	ganManWorldTransform_.scale_ = {manScale, manScale, manScale};

	// ---------- プレイヤーの銃 ----------
	Vector3 gunPos = kGunPos;
	Vector3 gunRot = {0.0f, kGunRotY, 0.0f};

	// 待機中のゆらぎ(緊張感)
	if (phase_ == Phase::Countdown) {
		gunPos.y += 0.05f * std::sin(timer_ * 4.0f);
	}

	// 発砲の反動(勝ち or お手つき時にプレイヤーが撃った扱い)
	if (phase_ == Phase::Result && (result_ == Result::Win || result_ == Result::Foul)) {
		float t = std::fmin(resultTimer_ / 0.2f, 1.0f);
		gunRot.x = -0.5f * std::sin(t * kPi);
	}
	// やられた時は銃が下がる
	if (phase_ == Phase::Result && result_ == Result::Lose) {
		gunPos.y -= 1.5f * std::fmin(resultTimer_ / 0.5f, 1.0f);
	}

	ganWorldTransform_.translation_ = gunPos;
	ganWorldTransform_.rotation_ = gunRot;
	ganWorldTransform_.scale_ = {kGunScale, kGunScale, kGunScale};

	UpdateWorldMatrix(ganManWorldTransform_);
	UpdateWorldMatrix(ganWorldTransform_);
}

void ganbattle::Draw(const Camera& camera) {
	Model::PreDraw();

	ganManModel_->Draw(ganManWorldTransform_, camera);
	ganModel_->Draw(ganWorldTransform_, camera);

	Model::PostDraw();
}