#include <Windows.h>
#include "KamataEngine.h"
#include "gameScene.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {	
	KamataEngine::Initialize(L"TD3");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	gameScene* scene = new gameScene();
	scene->Intialize();
	
	while (true) {		
		if (KamataEngine::Update()) {
			break;
		}

		scene->Update();

		dxCommon->PreDraw();
		scene->Draw();
		dxCommon->PostDraw();
	}

	delete scene;
	KamataEngine::Finalize();
	return 0;
}