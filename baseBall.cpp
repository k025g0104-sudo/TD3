#include "baseBall.h"
using namespace KamataEngine;

baseBall::baseBall() {};

baseBall::~baseBall() {
	delete model_;
}

void baseBall::Initialize() {
	model_=Model::CreateFromOBJ("BatterBox",true);

	worldTransform_.Initialize();
	//camera_.Initialize();
}

void baseBall::Update() {
	worldTransform_.rotation_.y += 0.02f;
	worldTransform_.TransferMatrix();
}

void baseBall::Draw(const Camera& camera) { 
	Model::PreDraw();
	model_->Draw(worldTransform_, camera);  
	Model::PostDraw();
}