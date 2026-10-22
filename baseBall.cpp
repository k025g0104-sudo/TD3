#include "baseBall.h"
#include <algorithm>
#include <cmath>
using namespace KamataEngine;

static Matrix4x4 MakeRotScale(const WorldTransform& wt) {
	const Vector3& s = wt.scale_;
	const Vector3& r = wt.rotation_;

	float sx = std::sin(r.x), cx = std::cos(r.x);
	float sy = std::sin(r.y), cy = std::cos(r.y);
	float sz = std::sin(r.z), cz = std::cos(r.z);

	Matrix4x4 m = {};
	m.m[0][0] = s.x * (cy * cz);
	m.m[0][1] = s.x * (cy * sz);
	m.m[0][2] = s.x * (-sy);

	m.m[1][0] = s.y * (sx * sy * cz - cx * sz);
	m.m[1][1] = s.y * (sx * sy * sz + cx * cz);
	m.m[1][2] = s.y * (sx * cy);

	m.m[2][0] = s.z * (cx * sy * cz + sx * sz);
	m.m[2][1] = s.z * (cx * sy * sz - sx * cz);
	m.m[2][2] = s.z * (cx * cy);

	m.m[3][3] = 1.0f;
	return m;
}

static void UpdateWorld(WorldTransform& wt) {
	Matrix4x4 m = MakeRotScale(wt);
	m.m[3][0] = wt.translation_.x;
	m.m[3][1] = wt.translation_.y;
	m.m[3][2] = wt.translation_.z;
	wt.matWorld_ = m;
	wt.TransferMatrix();
}

static void UpdateBatWorld(WorldTransform& wt, float swingAngle) {
	Matrix4x4 base = MakeRotScale(wt);

	float s = std::sin(swingAngle), c = std::cos(swingAngle);
	Matrix4x4 ry = {};
	ry.m[0][0] = c;
	ry.m[0][2] = -s;
	ry.m[1][1] = 1.0f;
	ry.m[2][0] = s;
	ry.m[2][2] = c;
	ry.m[3][3] = 1.0f;

	Matrix4x4 m = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				m.m[i][j] += base.m[i][k] * ry.m[k][j];
			}
		}
	}

	m.m[3][0] = wt.translation_.x;
	m.m[3][1] = wt.translation_.y;
	m.m[3][2] = wt.translation_.z;
	wt.matWorld_ = m;
	wt.TransferMatrix();
}

baseBall::baseBall() {}

baseBall::~baseBall() {
	delete model_;
	delete baseballModel_;
	delete batModel_;
	delete ringModel_;
}

void baseBall::Initialize() {
	// ※モデルの読み込みとWorldTransformの生成は最初に一度だけ行う。
	//   ゲーム状態の初期化は Reset() に任せる。

	// バッターbox
	model_ = Model::CreateFromOBJ("BatterBox", true);
	worldTransform_.Initialize();
	worldTransform_.rotation_.y = -89.5f;
	worldTransform_.translation_.x = -1.5f;
	worldTransform_.translation_.z = 50.0f;

	// 野球ボール
	baseballModel_ = Model::CreateFromOBJ("baseboll", true);
	for (Ball& ball : balls_) {
		ball.worldTrnasform.Initialize();
	}

	// バット
	batModel_ = Model::CreateFromOBJ("bat", true);
	batWorldTransform_.Initialize();

	// リング用3Dモデル("ring.obj"がModelsフォルダにある前提)
	ringModel_ = Model::CreateFromOBJ("ring", true);
	targetRingTransform_.Initialize();
	for (int i = 0; i < kMaxBalls; ++i) {
		approachRingTransforms_[i].Initialize();
	}
	flashRingTransform_.Initialize();

	// 乱数のシード
	rng_.seed(std::random_device{}());

	// ゲーム状態を初期値にする
	Reset();
}

// レベルに応じた難易度の設定(数値を変えれば調整できる)
void baseBall::ApplyDifficulty(int level) {
	level_ = std::clamp(level, 0, kMaxLevel);

	// 球速:1レベルごとに +20%
	pitchSpeed_ = kPitchSpeed * (1.0f + kSpeedUpPerLevel * level_);

	// 1セットの最小球数
	minPitchCount_ = (std::min)(kMinPitchCount + level_ / 2, kMaxPitchCount);

	// 判定の許容フレームを狭くする(下限あり)
	goodRange_ = (std::max)(4, kGoodRange - level_);
	greatRange_ = (std::max)(2, kGreatRange - level_ / 2);
	perfectRange_ = (level_ >= 3) ? 1 : kPerfectRange;
}

// ミニゲームが切り替わるたびに呼ぶ:ゲームの状態だけを初期値に戻す
void baseBall::Reset(int level) {
	ApplyDifficulty(level);

	// ボール
	for (Ball& ball : balls_) {
		ball.active = false;
		ball.judged = false;
		ball.timer = 0;
		ball.state = BallState::Pitched;
		ball.velocity = { 0.0f, 0.0f, 0.0f };
	}
	spawnTimer_ = kPitchInterval - 1;
	pitchRemaining_ = 0;
	pitchGapTimer_ = 0;

	// バット
	batWorldTransform_.translation_ = { -7.0f, 1.0f, 0.0f };
	batWorldTransform_.rotation_.y = kBatStartAngle;
	batWorldTransform_.rotation_.z = -1.5f;

	isSwinging_ = false;
	swingAngle_ = 0.0f;
	swingTimer_ = 0;
	swingCooldowonTimer_ = 0;

	// 判定・スコア
	lastJudge_ = Judge::None;
	combo_ = 0;
	score_ = 0;

	// リング
	const Vector3 hitPosition = { kTargetWorldX, kTargetWorldY, batWorldTransform_.translation_.z };
	const Vector3 ringRotation = { 0.0f, 1.5f, 0.0f };

	// ターゲットリング(打点位置に配置)
	targetRingTransform_.translation_ = hitPosition;
	targetRingTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
	targetRingTransform_.rotation_ = ringRotation;

	// 縮んでくるリング
	for (int i = 0; i < kMaxBalls; ++i) {
		approachRingTransforms_[i].translation_ = hitPosition;
		approachRingTransforms_[i].scale_ = { kApproachStartScale, kApproachStartScale, kApproachStartScale };
		approachRingTransforms_[i].rotation_ = ringRotation;
		approachRingActive_[i] = false;
	}

	// 判定用演出リング
	flashRingTransform_.translation_ = hitPosition;
	flashRingTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
	flashRingTransform_.rotation_ = ringRotation;
	flashRingActive_ = false;
	flashTimer_ = 0;
}

void baseBall::SpawnBall() {
	for (Ball& ball : balls_) {
		if (ball.active) {
			continue;
		}

		// 球種を決める
		float baseFrames = kPitchDistance / pitchSpeed_;
		float mul = 1.0f;
		ball.type = PitchType::Straight;

		ball.curveAngle = kCurveAngleDeg * 3.14159265f / 180.0f;

		std::uniform_int_distribution<int> d(0, 99);
		int r = d(rng_);
		if (level_ >= 1 && r < 15 + level_ * 6) {
			ball.type = PitchType::ChangeUp;
			mul = 1.3f;
		} else if (level_ >= 2 && r < 35 + level_ * 6) {
			ball.type = PitchType::Curve;
			mul = 1.1f;

			std::uniform_int_distribution<int> side(0, 1);
			ball.curveDir = side(rng_) ? 1.0f : -1.0f;
		}
		ball.totalFrames = (std::max)(1, static_cast<int>(baseFrames * mul));

		// 初期化
		ball.active = true;
		ball.state = BallState::Pitched;
		ball.timer = 0;
		ball.judged = false;
		ball.velocity = {0.0f, 0.0f, 0.0f};
		ball.worldTrnasform.translation_ = {worldTransform_.translation_.x, kTargetWorldY, 50.0f};
		return;
	}
}

void baseBall::UpdateBat() {
	Input* input = Input::GetInstance();

	if (swingCooldowonTimer_ > 0) {
		swingCooldowonTimer_--;
	}

	if (!isSwinging_ && swingCooldowonTimer_ <= 0 && input->TriggerKey(DIK_SPACE)) {
		isSwinging_ = true;
		swingTimer_ = 0;
	}

	if (isSwinging_) {
		swingTimer_++;
		float t = static_cast<float>(swingTimer_) / kSwingFrame;
		t = (std::min)(t, 1.0f);
		swingAngle_ = (kBatendAngle - kBatStartAngle) * t;

		if (swingTimer_ >= kSwingFrame) {
			isSwinging_ = false;
			swingAngle_ = 0.0f;
			swingCooldowonTimer_ = kSwingCoolDownFrame;
		}
	}
}

baseBall::Judge baseBall::JudgeTiming() {
	if (!isSwinging_) {
		return Judge::Miss;
	}
	int diff = std::abs(swingTimer_ - kContactFrame);
	if (diff <= perfectRange_) return Judge::Perfect;
	if (diff <= greatRange_)   return Judge::Great;
	if (diff <= goodRange_)    return Judge::Good;
	return Judge::Miss;
}

void baseBall::ApplyJudge(Judge judge) {
	lastJudge_ = judge;
	flashTimer_ = kFlashFrame;
	flashRingActive_ = true;

	if (judge == Judge::Miss) {
		combo_ = 0;
		return;
	}

	combo_++;
	int base = 0;
	switch (judge) {
	case Judge::Perfect: base = 300; break;
	case Judge::Great:   base = 200; break;
	case Judge::Good:    base = 100; break;
	default: break;
	}
	score_ += base + combo_ * 10;
}

void baseBall::UpdateBall() {
	// 一定間隔で「投球セット」を開始(1~3球をランダムに決める)
	if (++spawnTimer_ >= kPitchInterval) {
		spawnTimer_ = 0;
		std::uniform_int_distribution<int> dist(minPitchCount_, kMaxPitchCount);
		pitchRemaining_ = dist(rng_);
		pitchGapTimer_ = 1; // 1球目はすぐ投げる
	}

	// セット中は kPitchGapFrames(1秒)ごとに1球ずつ投げる
	if (pitchRemaining_ > 0 && --pitchGapTimer_ <= 0) {
		SpawnBall();
		pitchRemaining_--;

		int minGap = (std::max)(30, kPitchGapFrames - level_ * 6);
		std::uniform_int_distribution<int> gap(minGap, kPitchGapFrames);
		pitchGapTimer_ = gap(rng_);
	}

	for (Ball& ball : balls_) {
		if (!ball.active) {
			continue;
		}

		Vector3& pos = ball.worldTrnasform.translation_;

		switch (ball.state) {
		case BallState::Pitched: {
			ball.timer++;
			const float startZ = 50.0f;
			const float batZ = batWorldTransform_.translation_.z;
			float u = static_cast<float>(ball.timer) / ball.totalFrames;

			float e = u; // 進行度の変形
			float offX = 0.0f;
			float offY = 0.0f;

			if (u < 1.0f) {
				if (ball.type == PitchType::ChangeUp) {
					// 前半は速く、手前で急に失速
					e = 0.4f * u + 0.6f * (1.0f - (1.0f - u) * (1.0f - u));
				} else if (ball.type == PitchType::Curve) {
					float k = std::sin(u * 3.14159265f) * (1.0f - u);
					float amp = kCurveAmount * k;
					offX = amp * std::cos(ball.curveAngle) * ball.curveDir;
					offY = amp * std::sin(ball.curveAngle);
				}
			}

			// ボックスの位置から打点へ、横方向にも移動する
			const float releaseX = worldTransform_.translation_.x;
			const float lateral = (std::min)(u, 1.0f);

			pos.x = releaseX + (kTargetWorldX - releaseX) * lateral + offX;
			pos.y = kTargetWorldY + offY;
			pos.z = startZ + (batZ - startZ) * e;

			if (!ball.judged && ball.timer >= ball.totalFrames) {
				ball.judged = true;

				Judge judge = JudgeTiming();
				ApplyJudge(judge);

				if (judge != Judge::Miss) {
					ball.state = BallState::Hit;
					ball.timer = 0;
					switch (judge) {
					case Judge::Perfect:
						ball.velocity = {0.05f, 0.60f, 2.0f};
						break;
					case Judge::Great:
						ball.velocity = {0.05f, 0.45f, 1.5f};
						break;
					case Judge::Good:
						ball.velocity = {0.05f, 0.20f, 0.8f};
						break;
					default:
						break;
					}
				}
			} else if (pos.z < -5.0f) {
				ball.active = false;
			}
			break;
		}

		case BallState::Hit:
			ball.velocity.y -= 0.01f;
			pos.x += ball.velocity.x;
			pos.y += ball.velocity.y;
			pos.z += ball.velocity.z;

			if (pos.y < 0.0f || ++ball.timer > 600) {
				ball.active = false;
			}
			break;
		}
	}
}

// 縮小リング・判定演出の更新
void baseBall::UpdateTimingUI() {
	for (int i = 0; i < kMaxBalls; i++) {
		const Ball& ball = balls_[i];
		WorldTransform& ringTransform = approachRingTransforms_[i];

		// 投球中のストレートだけリングを出す(変化球はボールを見て打つ)
		if (!ball.active || ball.state != BallState::Pitched || ball.type != PitchType::Straight) {
			approachRingActive_[i] = false;
			continue;
		}

		float framesToArrive = static_cast<float>(ball.totalFrames - ball.timer);
		float framesUntilPress = framesToArrive - static_cast<float>(kContactFrame);

		if (framesUntilPress > kApproachFrame) {
			approachRingActive_[i] = false;
			continue;
		}

		approachRingActive_[i] = true;

		// スケールの計算 (1.0倍を基準にアプローチ割合に応じて縮小)
		float scale = 1.0f + (framesUntilPress / kApproachFrame) * (kApproachStartScale - 1.0f);
		scale = (std::max)(scale, 0.5f);

		ringTransform.scale_ = {scale, scale, scale};
		UpdateWorld(ringTransform);
	}

	// 判定演出（広がって消えるリング）
	if (flashTimer_ > 0) {
		flashTimer_--;
		float a = static_cast<float>(flashTimer_) / kFlashFrame;
		float scale = 1.0f + 0.8f * (1.0f - a);

		flashRingTransform_.scale_ = { scale, scale, scale };
		UpdateWorld(flashRingTransform_);
	}
	else {
		flashRingActive_ = false;
	}

	UpdateWorld(targetRingTransform_);
}

void baseBall::Update() {
	UpdateBat();
	UpdateBall();

	UpdateWorld(worldTransform_);
	UpdateBatWorld(batWorldTransform_, swingAngle_);

	for (Ball& ball : balls_) {
		if (ball.active) {
			UpdateWorld(ball.worldTrnasform);
		}
	}

	UpdateTimingUI();
}

void baseBall::Draw(const Camera& camera) {
	Model::PreDraw();

	model_->Draw(worldTransform_, camera);
	batModel_->Draw(batWorldTransform_, camera);
	for (Ball& ball : balls_) {
		if (ball.active) {
			baseballModel_->Draw(ball.worldTrnasform, camera);
		}
	}

	// --- 3Dリングモデルの描画 ---
	// ターゲットリング（常に描画）
	ringModel_->Draw(targetRingTransform_, camera);

	// 近づくリング
	for (int i = 0; i < kMaxBalls; ++i) {
		if (approachRingActive_[i]) {
			ringModel_->Draw(approachRingTransforms_[i], camera);
		}
	}

	// フラッシュリング
	if (flashRingActive_) {
		ringModel_->Draw(flashRingTransform_, camera);
	}

	Model::PostDraw();
}