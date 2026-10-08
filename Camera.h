#pragma once
#include "KamataEngine.h"

// KamataEngine::Camera との名前衝突を避けるため、クラス名は GameCamera にしています
class GameCamera {
public:
	~GameCamera();

	void Initialize();
	void Update();

	// 描画時に Model::Draw(worldTransform, camera) へ渡す
	const KamataEngine::Camera& GetCamera() const { return camera_; }

	// ---- 通常カメラの設定 ----
	// 注視点（カメラが見る位置）。プレイヤー追従ならここに毎フレーム座標を渡す
	void SetTarget(const KamataEngine::Vector3& target) { target_ = target; }
	// 注視点からの距離
	void SetDistance(float distance);
	float GetDistance() const { return distance_; }
	// 距離の調整可能範囲
	void SetDistanceRange(float minDistance, float maxDistance);
	// 向き（ラジアン）。pitch=上下, yaw=左右
	void SetAngle(float pitch, float yaw) {
		pitch_ = pitch;
		yaw_ = yaw;
	}

	// ---- デバッグカメラ ----
	void ToggleDebug() { isDebug_ = !isDebug_; }
	bool IsDebug() const { return isDebug_; }

private:
	// ImGui の操作ウィンドウ（Debugビルドのみ）
	void DrawImGui();

	KamataEngine::Camera camera_;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	bool isDebug_ = false;

	KamataEngine::Vector3 target_ = { 0.0f, 0.0f, 0.0f };
	float distance_ = 30.0f;      // 注視点からの距離
	float minDistance_ = 5.0f;    // 最短距離
	float maxDistance_ = 100.0f;  // 最長距離
	float distanceSpeed_ = 0.5f;  // 1フレームあたりの距離変化量
	float pitch_ = 0.2f;          // 上下の角度
	float yaw_ = 0.0f;            // 左右の角度
};