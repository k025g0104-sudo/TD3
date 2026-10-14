#include "baseBall.h"
#include <algorithm>
#include <cmath>
using namespace KamataEngine;

// scale / rotation から行列を作る(平行移動なし)
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

// バット用:構えの姿勢のあとに、世界のY軸まわりでスイング角を掛ける
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

	// m = base * ry
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
baseBall::baseBall() {};

baseBall::~baseBall() {
	delete model_;
	delete baseballModel_;
	delete batModel_;
}

void baseBall::Initialize() {
	// バッターbox君
	model_ = Model::CreateFromOBJ("BatterBox", true);
	worldTransform_.Initialize();
	worldTransform_.rotation_.y = -89.5f;
	worldTransform_.translation_.x = -1.5f;
	worldTransform_.translation_.z = 50;

	// 野球ボール
	baseballModel_ = Model::CreateFromOBJ("baseboll", true);
	for (Ball& ball : balls_) {
		ball.worldTrnasform.Initialize();
		ball.active = false;
	}
	spawnTimer_ = 0;

	// バット
	batModel_ = Model::CreateFromOBJ("bat", true);
	batWorldTransform_.Initialize();
	batWorldTransform_.translation_ = { -30.0f, 1.0f, 0.0f };
	batWorldTransform_.rotation_.y = kBatStartAngle;
	batWorldTransform_.rotation_.z = -1.5f;
}

void baseBall::SpawnBall() {
	for (Ball& ball : balls_) {
		if (ball.active) {
			continue;
		}
		ball.active = true;
		ball.state = BallState::Pitched;
		ball.timer = 0;
		ball.velocity = { 0.0f,0.0f,-kPitchSpeed };
		ball.worldTrnasform.translation_ = { 0.0f,2.0f,50.0f };
		return;
	}
}

void baseBall::UpdateBat() {
	Input* input = Input::GetInstance();

	if (!isSwinging_ && input->TriggerKey(DIK_Z)) {
		isSwinging_ = true;
		swingTimer_ = 0;
	}

	if (isSwinging_) {
		swingTimer_++;
		float t = static_cast<float>(swingTimer_) / kSwingFrame;
		t = (std::min)(t, 1.0f);
		swingAngle_ = (kBatendAngle - kBatStartAngle) * t; // 0 → -3.0

		if (swingTimer_ >= kSwingFrame) {
			isSwinging_ = false;
			swingAngle_ = 0.0f; 
		}
	}
}

void baseBall::UpdateBall() {
	if (++spawnTimer_ >= kPitchInterval) {
		spawnTimer_ = 0;
		SpawnBall();
	}
	bool swingActive = isSwinging_ && swingTimer_ >= kHitStartFrame && swingTimer_ <= kHitEndFrame;

	for (Ball& ball : balls_) {
		if (!ball.active) {
			continue;
		}

		Vector3& pos = ball.worldTrnasform.translation_;

		switch (ball.state)
		{
		case BallState::Pitched: {
			pos.x += ball.velocity.x;
			pos.y += ball.velocity.y;
			pos.z += ball.velocity.z;

			float dz = pos.z - batWorldTransform_.translation_.z;
			if (swingActive && std::abs(dz) < kHitRange) {
				ball.state = BallState::Hit;
				ball.timer = 0;
				ball.velocity = { 0.05f,0.4f,1.5f };
			}
			else if (pos.z < -5.0f) {
				ball.active = false;
			}
			break;
		}
		case BallState::Hit:
			ball.velocity.y -= 0.01f;
			pos.x += ball.velocity.x;
			pos.y += ball.velocity.y;
			pos.z += ball.velocity.z;

			if (pos.y < 0.0f || ++ball.timer>600) {
				ball.active = false;
			}
			break;
		}

	}
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

	Model::PostDraw();
}