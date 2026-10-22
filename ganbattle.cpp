#include "ganbattle.h"
#include <cmath>
#include <random>

using namespace KamataEngine;

namespace {

// カメラの初期位置・角度(ImGuiで調整して、良い値をここに書き戻す)
const Vector3 kCamPos = {0.0f, 4.0f, 0.0f};
const Vector3 kCamRot = {0.0f, 0.0f, 0.0f};

// カウント数字の表示
//const Vector2 kNumCenter = {640.0f, 360.0f}; // 画面中央(解像度に合わせて変更)
//constexpr float kNumSize = 256.0f;           // 数字の基本サイズ(px)

constexpr float kDt = 1.0f / 60.0f; // 1フレームの時間
constexpr float kPi = 3.14159265f;

// カウント数字(元は10x10のXZ平面なので、X回転で立ててカメラへ向ける)
const Vector3 kNumPos = {0.0f, 4.0f, 10.0f};
const Vector3 kNumRot = {kPi * 0.5f, kPi, 0.0f};
constexpr float kNumScale = 0.5f;

// カウント
constexpr float kCountBase = 3.0f;   // 最低待ち時間
constexpr float kCountRandom = 1.5f; // さらにランダムで足す最大秒数

// CPUの反応時間(秒) 短いほど強い
constexpr float kCpuReactMin = 0.35f;
constexpr float kCpuReactMax = 0.65f;

// 結果表示の長さ
constexpr float kResultTime = 1.8f;

// 相手ガンマン(モデルは高さ約7)
const Vector3 kManPos = {-1.2f, 4.8f, 29.65f};
const Vector3 kManRot = {0.2f, 3.14f, 0.16f}; // プレイヤー側を向かせる(逆なら 0.0f)
constexpr float kManScale = 0.850f;

// プレイヤーの銃(モデルは全長約16なので縮小)
const Vector3 kGunPos = {-0.6f, 3.75f, 18.95f};
const Vector3 kGunRot = {0.22f, 3.11f, 0.24f}; // 銃口を相手(+Z)へ向ける(逆なら 0.0f)
constexpr float kGunScale = 0.6f;
} // namespace

ganbattle::~ganbattle() {
	delete ganModel_;
	delete ganManModel_;
	for (int i = 0; i < 3; ++i) {
		delete numModel_[i];
	}
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

	// カウントダウン数字
	// カウントダウン数字(3Dモデル)
	numModel_[0] = Model::CreateFromOBJ("numbers1");
	numModel_[1] = Model::CreateFromOBJ("numbers2");
	numModel_[2] = Model::CreateFromOBJ("numbers3");
	numWorldTransform_.Initialize();

	camera_.Initialize();
	camera_.translation_ = kCamPos;
	camera_.rotation_ = kCamRot;
	camera_.UpdateMatrix();

	ResetGunBase();
	ResetManBase();
	ResetNumBase();  

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

	currentNumber_ = 0;

	paused_ = false;

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
	UpdateCameraDebug();
	UpdateObjectDebug(); 
	Input* input = Input::GetInstance();

#ifdef _DEBUG
	// Pキーで一時停止の切り替え
	if (input->TriggerKey(DIK_P)) {
		paused_ = !paused_;
	}
	// 停止中は進行を止める(見た目の更新だけ行う)
	if (paused_) {
		UpdateTransforms();
		return;
	}
#endif

	const bool pressed = input->TriggerKey(DIK_SPACE);
	switch (phase_) {
	case Phase::Countdown: {
		timer_ += kDt;

		// 残り秒数(切り上げ)
		float left = countdownTime_ - timer_;
		int remain = static_cast<int>(std::ceil(left));

		// 最後の3秒だけ数字を出す
		if (remain >= 1 && remain <= 3) {
			currentNumber_ = remain;
			numFrac_ = left - static_cast<float>(remain - 1); // 1.0 → 0.0
			if (remain != lastCountPrint_) {
				lastCountPrint_ = remain;
				// TODO: ここでカウントのSEを鳴らす
			}
		} else {
			currentNumber_ = 0;
		}

		// お手つき
		if (pressed) {
			currentNumber_ = 0;
			ShowResult(Result::Foul);
			break;
		}

		// 合図
		if (timer_ >= countdownTime_) {
			currentNumber_ = 0;
			phase_ = Phase::Signal;
			timer_ = 0.0f;
			DebugText::GetInstance()->ConsolePrintf("DRAW!!\n");
		}
		break;
	}

	case Phase::Signal:
		timer_ += kDt;

		// 同フレームならCPU優先(先に撃たれた扱い)
		if (timer_ >= cpuReaction_) {
			ShowResult(Result::Lose);
		} else if (pressed) {
			ShowResult(Result::Win);
		}
		break;

	case Phase::Result:
		resultTimer_ += kDt;
		if (resultTimer_ >= kResultTime) {
			finished_ = true;
		}
		break;
	}

	UpdateTransforms();
}

void ganbattle::UpdateCameraDebug() {
#ifdef _DEBUG
	ImGui::Begin("ganbattle camera");
	ImGui::Checkbox("pause (P key)", &paused_);
	ImGui::DragFloat3("position", &camera_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_.rotation_.x, 0.01f);
	ImGui::DragFloat("fovY", &camera_.fovAngleY, 0.01f, 0.1f, 3.0f);
	if (ImGui::Button("reset")) {
		camera_.translation_ = kCamPos;
		camera_.rotation_ = kCamRot;
		camera_.fovAngleY = 0.7854f; // 45度
	}
	ImGui::Text("pos: %.2f, %.2f, %.2f", camera_.translation_.x, camera_.translation_.y, camera_.translation_.z);
	ImGui::Text("rot: %.2f, %.2f, %.2f", camera_.rotation_.x, camera_.rotation_.y, camera_.rotation_.z);
	ImGui::End();
#endif
	camera_.UpdateMatrix();
}
void ganbattle::ResetGunBase() {
	gunBasePos_ = kGunPos;
	gunBaseRot_ = kGunRot;
	gunBaseScale_ = kGunScale;
}

void ganbattle::ResetManBase() {
	manBasePos_ = kManPos;
	manBaseRot_ = kManRot;
	manBaseScale_ = kManScale;
}

void ganbattle::ResetNumBase() {
	numBasePos_ = kNumPos;
	numBaseRot_ = kNumRot;
	numBaseScale_ = kNumScale;
}

void ganbattle::UpdateObjectDebug() {
#ifdef _DEBUG
	// ---- 銃 ----
	ImGui::Begin("ganbattle gun");
	ImGui::DragFloat3("position", &gunBasePos_.x, 0.05f);
	ImGui::DragFloat3("rotation", &gunBaseRot_.x, 0.01f);
	ImGui::DragFloat("scale", &gunBaseScale_, 0.01f, 0.01f, 10.0f);
	if (ImGui::Button("reset")) {
		ResetGunBase();
	}
	ImGui::Text("pos: %.2f, %.2f, %.2f", gunBasePos_.x, gunBasePos_.y, gunBasePos_.z);
	ImGui::Text("rot: %.2f, %.2f, %.2f", gunBaseRot_.x, gunBaseRot_.y, gunBaseRot_.z);
	ImGui::Text("scale: %.2f", gunBaseScale_);
	ImGui::End();

	//----number---
	ImGui::Begin("ganbattle number");
	ImGui::DragFloat3("position", &numBasePos_.x, 0.05f);
	ImGui::DragFloat3("rotation", &numBaseRot_.x, 0.01f);
	ImGui::DragFloat("scale", &numBaseScale_, 0.01f, 0.01f, 10.0f);
	if (ImGui::Button("reset")) {
		ResetNumBase();
	}
	ImGui::End();

	// ---- 相手ガンマン ----
	ImGui::Begin("ganbattle man");
	ImGui::DragFloat3("position", &manBasePos_.x, 0.05f);
	ImGui::DragFloat3("rotation", &manBaseRot_.x, 0.01f);
	ImGui::DragFloat("scale", &manBaseScale_, 0.01f, 0.01f, 10.0f);
	if (ImGui::Button("reset")) {
		ResetManBase();
	}
	ImGui::Text("pos: %.2f, %.2f, %.2f", manBasePos_.x, manBasePos_.y, manBasePos_.z);
	ImGui::Text("rot: %.2f, %.2f, %.2f", manBaseRot_.x, manBaseRot_.y, manBaseRot_.z);
	ImGui::Text("scale: %.2f", manBaseScale_);
	ImGui::End();
#endif
}

void ganbattle::UpdateTransforms() {
	// ---------- 相手ガンマン ----------
	Vector3 manPos = manBasePos_;
	Vector3 manRot = manBaseRot_;

	if (phase_ == Phase::Signal) {
		// 合図: ピョンと跳ねて知らせる
		manPos.y += 1.5f * std::sin(std::fmin(timer_ / 0.15f, 1.0f) * kPi);
	} else if (phase_ == Phase::Result) {
		float t = std::fmin(resultTimer_ / 0.6f, 1.0f);
		if (result_ == Result::Win) {
			// 撃たれて後ろに倒れる
			manRot.x += -kPi * 0.5f * t;
		} else if (result_ == Result::Lose) {
			// 勝ち誇ってわずかに前へ
			manPos.z -= 2.0f * t;
		}
	}

	ganManWorldTransform_.translation_ = manPos;
	ganManWorldTransform_.rotation_ = manRot;
	ganManWorldTransform_.scale_ = {manBaseScale_, manBaseScale_, manBaseScale_};

	// ---------- プレイヤーの銃 ----------
	Vector3 gunPos = gunBasePos_;
	Vector3 gunRot = gunBaseRot_;

	// 待機中のゆらぎ(緊張感)
	if (phase_ == Phase::Countdown) {
		gunPos.y += 0.05f * std::sin(timer_ * 4.0f);
	}

	// 発砲の反動(勝ち or お手つき時にプレイヤーが撃った扱い)
	if (phase_ == Phase::Result && (result_ == Result::Win || result_ == Result::Foul)) {
		float t = std::fmin(resultTimer_ / 0.2f, 1.0f);
		gunRot.x += -0.5f * std::sin(t * kPi);
	}
	// やられた時は銃が下がる
	if (phase_ == Phase::Result && result_ == Result::Lose) {
		gunPos.y -= 1.5f * std::fmin(resultTimer_ / 0.5f, 1.0f);
	}

	ganWorldTransform_.translation_ = gunPos;
	ganWorldTransform_.rotation_ = gunRot;
	ganWorldTransform_.scale_ = {gunBaseScale_, gunBaseScale_, gunBaseScale_};

		// カウント数字(大きく出て縮む)
	float numScale = numBaseScale_ * (0.8f + 0.4f * numFrac_);
	numWorldTransform_.translation_ = numBasePos_;
	numWorldTransform_.rotation_ = numBaseRot_;
	numWorldTransform_.scale_ = {numScale, numScale, numScale};

	UpdateWorldMatrix(ganManWorldTransform_);
	UpdateWorldMatrix(ganWorldTransform_);
	UpdateWorldMatrix(numWorldTransform_);
}

void ganbattle::Draw() {
	Model::PreDraw();

	ganManModel_->Draw(ganManWorldTransform_, camera_);
	ganModel_->Draw(ganWorldTransform_, camera_);
	if (currentNumber_ >= 1 && currentNumber_ <= 3) {
		numModel_[currentNumber_ - 1]->Draw(numWorldTransform_, camera_);
	}

	Model::PostDraw();
}

void ganbattle::DrawUI() {
	//if (currentNumber_ < 1 || currentNumber_ > 3) {
	//	return;
	//}
	//Sprite* s = numSprite_[currentNumber_ - 1];

	//// 大きく出て、縮みながら薄くなる
	//float size = kNumSize * (0.8f + 0.4f * numFrac_);
	//float alpha = std::fmin(1.0f, numFrac_ * 2.0f + 0.2f);
	//s->SetSize({size, size});
	//s->SetColor({1.0f, 1.0f, 1.0f, alpha});
	//s->Draw();
}