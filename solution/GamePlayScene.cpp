#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

#include "FbxObject3d.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"

#include "PostEffect.h"

void GamePlayScene::Initialize(DirectXCommon* dxcommon)
{
	// スプライト共通テクスチャ読み込み
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/gameplay.png");
	// spriteCommon->LoadTexture(2, L"Resources/house.png");

	// スプライトの生成
	sprite = Sprite::Create(1, { 0,0 }, false, false);

	//for (int i = 0; i < 20; i++)// 20個生成するとき
	//{
	//	int texNumber = 1;
	//	// スプライトの原点が左上になる
	//	// sprites[i] = SpriteCreate(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height, texNumber, spriteCommon, { 0,0 }, false, false);
	//	Sprite* sprite = Sprite::Create(spriteCommon, texNumber, { 0,0 }, false, false);

	//	// スプライトの座標変更
	//	sprite->SetPosition({ (float)(rand() % 1280),(float)(rand() % 720),0 });
	//	// sprite->SetRotation((float)(rand() % 360));
	//	// sprite->SetSize({ (float)(rand() % 400) ,(float)(rand() % 400) });
	//	//sprites[i].isInvisible = true;

	//	//sprites[i].size.x = 400.0f;
	//	//sprites[i].size.y = 100.0f;
	//	// 頂点バッファに反映
	//	sprite->TransferVertexBuffer();
	//	sprites.push_back(sprite);
	//}

	camera.reset(new DebugCamera(WinApp::window_width, WinApp::window_height));
	//camera->Initialize(WinApp::window_width, WinApp::window_height);
	// camera = new DebugCamera(WinApp::window_width, WinApp::window_height);

	ObjObject3d::SetCamera(camera.get());

	// OBJからモデルデータを読み込む
	// model_1 = Model::LoadFromObj("ground");
	model = ObjModel::LoadFromObj("triangle_mat");
	// 3Dオブジェクト生成

	object3d = ObjObject3d::Create();
	// object3d.reset(Object3d::Create());

	object3d->SetModel(model);

	// 3Dオブジェクトに3Dモデルをひもづけ
	//object3d_1->SetModel(model_1);

	// Fbx読み込み
	fbxModel = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	//デバイスをセット
	FbxObject3d::SetDevice(dxcommon->GetDev());
	//カメラをセット
	FbxObject3d::SetCamera(camera.get());
	//グラフィックスパイプライン生成
	FbxObject3d::CreateGraphicsPipeline();

	fbxObj = new FbxObject3d();
	fbxObj->Initialize();
	fbxObj->SetModel(fbxModel);
	fbxObj->SetPosition({ 0,0,0 });

	// 音声読み込み
	Audio::GetInstance()->LoadWave("Alarm01.wav");

	// 音声再生
	// audio->PlayWave("Alarm01.wav");
}

void GamePlayScene::Finalize()
{
	// スプライト個別解放
		/*for (auto& sprite : sprites) {
			delete sprite;
		}*/
	delete sprite;

	// delete fbxModel;
	delete fbxObj;

	// sprites.clear();

	// 3Dモデル解放
	delete model;
}

void GamePlayScene::Update()
{
	Input* input = Input::GetInstance();

	// 入力の確認
	//if (input->TriggerKey(DIK_0)) // 数字の0キーが押されていたら
	//{
	//	OutputDebugStringA("Hit 0\n");  // 出力ウィンドウに「Hit 0」と表示
	//}

	//if (input->Triggerkey(DIK_SPACE))     // スペースキーが押されていたら
	//{
	//    // 画面クリアカラーの数値を書き換える
	//    clearColor[1] = 1.0f;
	//}

	// 座標操作
	if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_RIGHT) || input->TriggerKey(DIK_LEFT))
	{

	}

	if (input->TriggerKey(DIK_D) || input->TriggerKey(DIK_A))
	{

	}

	// シーン切り替え
	if (input->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	// X座標,Y座標を指定して表示
	DebugText::GetInstance()->Print("Hello,DirectX!!", 200, 100);
	// X座標,Y座標,縮尺を指定して表示
	DebugText::GetInstance()->Print("Nihon Kogakuin", 200, 200, 2.0f);

	//sprite.rotation = 45;
	//sprite.position = {1280/2, 720/2, 0};
	//sprite.color = {0, 0, 1, 1};

	if (input->PushKey(DIK_0)) {
		fbxObj->PlayAnimation();
	}

	// モザイク切り替え
	{
		const bool TriggerP = input->TriggerKey(DIK_P);
		if (TriggerP) {
			// モザイクの細かさ
			// ウインドウサイズと同じ細かさのモザイク = モザイクなし
			DirectX::XMFLOAT2 mosaicNum { WinApp::window_width, WinApp::window_height};

			// モザイクをかける場合は、細かさを変更
			if (mosaicFlag) {
				mosaicNum = DirectX::XMFLOAT2(100, 100);
			}

			// モザイクをかける
			PostEffect::GetInstance()->SetMosaicNum(mosaicNum);

			// フラグを反転
			mosaicFlag = !mosaicFlag;
		}
	}

	// カメラ移動
	{
		const bool hitW = input->PushKey(DIK_W);
		const bool hitS = input->PushKey(DIK_S);
		const bool hitA = input->PushKey(DIK_A);
		const bool hitD = input->PushKey(DIK_D);

		DirectX::XMFLOAT3 camMoveVal{};
		constexpr float camMoveLen = 0.1f;

		if (hitW || hitS) {
			if (hitW) {
				camMoveVal.z += camMoveLen;
			}
			else if (hitS) {
				camMoveVal.z -= camMoveLen;
			}
		}

		if (hitA || hitD) {
			if (hitA) {
				camMoveVal.x -= camMoveLen;
			}
			else if (hitD) {
				camMoveVal.x += camMoveLen;
			}
		}

		camera->MoveEyeVector(camMoveVal);
		DirectX::XMFLOAT3 camTarget = camera->GetTarget();
		camTarget.x += camMoveVal.x;
		camTarget.y += camMoveVal.y;
		camTarget.z += camMoveVal.z;
		camera->SetTarget(camTarget);

		camera->Update();
		fbxObj->Update();
	}

	// 3Dオブジェクト更新
	object3d->Update();

	// スプライト更新

	sprite->Update();
	/*for (auto& sprite : sprites) {
		sprite->Update();
	}*/
}

void GamePlayScene::Draw(DirectXCommon* dxcommon)
{
	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();
	// スプライト描画
	/*for (auto& sprite : sprites) {
		sprite->Draw();
	}*/
	sprite->Draw();

	// 3Dオブジェクト描画前処理
	ObjObject3d::PreDraw();
	// 3Dオブジェクトの描画
	object3d->Draw();

	fbxObj->Draw(dxcommon->GetCmdList());

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	ObjObject3d::PostDraw();
	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();
}

void GamePlayScene::DrawFrontSprite(DirectXCommon* dxcommon){
	SpriteCommon::GetInstance()->PreDraw();

}