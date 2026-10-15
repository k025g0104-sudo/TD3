#include "Camera.h"
#include <algorithm>
#include <cmath>
#include <imgui.h>

using namespace KamataEngine;

GameCamera::~GameCamera() { delete debugCamera_; }

void GameCamera::Initialize() {
	camera_.Initialize();
	debugCamera_ = new DebugCamera(1280, 720); // 画面サイズに合わせる
}

void GameCamera::SetDistance(float distance) { distance_ = std::clamp(distance, minDistance_, maxDistance_); }

void GameCamera::SetDistanceRange(float minDistance, float maxDistance) {
	minDistance_ = minDistance;
	maxDistance_ = maxDistance;
	distance_ = std::clamp(distance_, minDistance_, maxDistance_);
}

void GameCamera::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("Camera");

	// デバッグカメラ ON/OFF
	ImGui::Checkbox("Use DebugCamera", &isDebug_);
	ImGui::Text(isDebug_ ? "Mode: DebugCamera (mouse operation)" : "Mode: Normal Camera");

	ImGui::Separator();

	// 通常カメラの調整
	ImGui::Text("Normal Camera");
	ImGui::SliderFloat("Distance", &distance_, minDistance_, maxDistance_);
	ImGui::SliderFloat("Pitch", &pitch_, -1.5f, 1.5f);
	ImGui::SliderFloat("Yaw", &yaw_, -3.14f, 3.14f);
	ImGui::DragFloat3("Target", &target_.x, 0.1f);
	ImGui::DragFloat("Distance Speed", &distanceSpeed_, 0.01f, 0.0f, 5.0f);

	if (ImGui::Button("Reset")) {
		target_ = { 0.0f, 0.0f, 0.0f };
		distance_ = 30.0f;
		pitch_ = 0.2f;
		yaw_ = 0.0f;
	}

	ImGui::End();
#endif
}

void GameCamera::Update() {
	Input* input = Input::GetInstance();

#ifdef _DEBUG
	// ImGui で操作
	DrawImGui();

	// C キーでもデバッグカメラ ON/OFF
	if (input->TriggerKey(DIK_C)) {
		isDebug_ = !isDebug_;
	}
#endif

	if (isDebug_) {
		// デバッグカメラ中は、マウスホイールで距離（ズーム）を調整
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
		return;
	}

	// 通常カメラ：Q で離れる / E で近づく
	if (input->PushKey(DIK_Q)) {
		distance_ += distanceSpeed_;
	}
	if (input->PushKey(DIK_E)) {
		distance_ -= distanceSpeed_;
	}
	distance_ = std::clamp(distance_, minDistance_, maxDistance_);

	// 向きから「前方向」を求め、注視点の後ろに distance_ だけ離れて配置する
	Vector3 forward = { std::cos(pitch_) * std::sin(yaw_), -std::sin(pitch_), std::cos(pitch_) * std::cos(yaw_) };

	camera_.translation_ = { target_.x - forward.x * distance_, target_.y - forward.y * distance_, target_.z - forward.z * distance_ };
	camera_.rotation_ = { pitch_, yaw_, 0.0f };

	camera_.UpdateMatrix();
}