#pragma once
#include "KamataEngine.h"
class baseBall{
public:
	baseBall();
	~baseBall();
	void Initialize();
	void Update();
	void Draw(const KamataEngine::Camera& camera);

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	/*KamataEngine::Camera camera_;*/
};

