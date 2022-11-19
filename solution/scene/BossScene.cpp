#include "BossScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

#include "FbxObject3d.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Collision.h"
#include "ParticleManager.h"

#include "PostEffect.h"

using namespace DirectX;

void BossScene::Initialize(DirectXCommon* dxcommon)
{
	// スプライト共通テクスチャ読み込み
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/gameplay.png");
	SpriteCommon::GetInstance()->LoadTexture(2, L"Resources/aim.png");

	// スプライトの生成
	sprite.reset(Sprite::Create(1, { 0,0 }, false, false));
	aim.reset(Sprite::Create(2));
	aim->SetPosition({ WinApp::window_width / 2.0f ,WinApp::window_height / 2.0f,0.f });

	// カメラの初期化
	camera.reset(new TrackingCamera());
	//camera->SetEye({ 0, 5, -20 });
	// camera->SetTarget({ 0, 0, 50 });

	ObjObject3d::SetCamera(camera.get());

	// OBJからモデルデータを読み込む

	// スカイドーム
	skyDomeModel.reset(ObjModel::LoadFromObj("skydome"));
	skyDomeObj = ObjObject3d::Create();
	skyDomeObj->SetModel(skyDomeModel.get());
	skyDomeObj->SetScale({ 5,5,5 });

	// 地面
	groundModel.reset(ObjModel::LoadFromObj("ground"));
	groundModel->SetTiling({ 10.f,10.f });
	groundObj = ObjObject3d::Create();
	groundObj->SetModel(groundModel.get());
	groundObj->SetScale({ 100,100,100 });
	groundObj->SetPosition({ 0,-5,0 });

	// 自機の読み込み
	pBulletModel.reset(ObjModel::LoadFromObj("playerBullet"));
	// 敵の読み込み
	enemyModel.reset(ObjModel::LoadFromObj("obake"));

	//デバイスをセット
	FbxObject3d::SetDevice(dxcommon->GetDev());
	//カメラをセット
	FbxObject3d::SetCamera(camera.get());
	//グラフィックスパイプライン生成
	FbxObject3d::CreateGraphicsPipeline();
	// プレイヤー初期化
	player = std::make_unique<Player>();

	// カメラをプレイヤーの位置にセット
	camera->SetTrackingTarget(player.get());
	camera->SetTarget(player->GetPosition());
	XMFLOAT3 eye = player->GetPosition();
	eye.z -= 50;
	eye.y += 10;
	camera->SetEye(eye);

	// ボス
	boss.reset(new Boss(enemyModel.get(), { 0.f,0.f,20.f }));

	boss->SetPhaseApproach();
	boss->SetAttackTarget(player.get());

	boss->SetBulletModel(enemyModel.get());

	// パーティクル初期化
	ParticleManager::GetInstance()->SetCamera(camera.get());

	// 更新処理の関数を入れる
	updateProcess = std::bind(&BossScene::start, this);

	// 音声読み込み
	Audio::GetInstance()->LoadWave("Alarm01.wav");
}

void BossScene::Finalize()
{
	DamageEffect(1, 1);
}

void BossScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
		pause = !pause;
	}

	if (!pause) {

		// マウスの固定
		{
			POINT oldMousePos = Input::GetInstance()->GetMousePos();

			Input::GetInstance()->SetMousePos({ long(WinApp::window_width / 2.f),long(WinApp::window_height / 2.f) });
			// 前座標との差分
			mousePosDiff = Input::GetInstance()->GetMousePos();
			mousePosDiff.x -= oldMousePos.x;
			mousePosDiff.y -= oldMousePos.y;
		}

		// シーン遷移
		updateProcess();

		// パーティクル更新
		ParticleManager::GetInstance()->Update();

		camera->Update();

		//fbxObj->Update();
		player->Update();
		//}

		boss->Update();

		groundObj->Update();
		skyDomeObj->Update();

		// スプライト更新
		sprite->Update();

		// aim->SetPosition({player->GetScreenAimPos().x,player->GetScreenAimPos().y,0});
		// aim->SetPosition({ (float)Input::GetInstance()->GetMousePos().x,(float)Input::GetInstance()->GetMousePos().y,0 });
		aim->Update();
	}
}

// シーン遷移
void BossScene::start()
{
	Input* input = Input::GetInstance();
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 40;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcessにplay関数をセット
		updateProcess = std::bind(&BossScene::play, this);
	}
	else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void BossScene::play()
{
	Input* input = Input::GetInstance();

	{
		char tmp[32]{};
		sprintf_s(tmp, 32, "%.2f,%.2f,%.2f", player->GetPosition().x, player->GetPosition().y, player->GetPosition().z);
		DebugText::GetInstance()->Print(tmp, 0, 100);
	}

	// レーンの位置
	{
		skyDomeObj->SetPosition(player->GetPosition());
	}

	// プレイヤーの移動と回避
	{
		const bool hitW = Input::GetInstance()->PushKey(DIK_W);
		const bool hitS = Input::GetInstance()->PushKey(DIK_S);
		const bool hitA = Input::GetInstance()->PushKey(DIK_A);
		const bool hitD = Input::GetInstance()->PushKey(DIK_D);
		const bool hitZ = Input::GetInstance()->PushKey(DIK_Z);
		const bool hitX = Input::GetInstance()->PushKey(DIK_X);

		const bool hitSpace = Input::GetInstance()->TriggerKey(DIK_SPACE);

		if (avoidFrame >= 1) {
			avoidFrame--;
		}

		if (hitW || hitS || hitA || hitD || hitZ || hitX) {
			auto pos = player->GetPosition();
			float moveSpeed = 0.2f;
			if (hitSpace && avoidFrame == 0) {
				moveSpeed *= 10;
				avoidFrame = avoidFrameMax;
			}

			// 前方向と右方向の単位ベクトルを作る
			XMVECTOR forwardVec = XMVectorSet(0, 0, 1, 1);
			XMVECTOR rightVec = XMVectorSet(1, 0, 0, 1);
			// プレイヤーの回転に合わせて回転させる
			forwardVec = XMVector3Rotate(forwardVec, XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(player->GetRotation().x),
				XMConvertToRadians(player->GetRotation().y),
				XMConvertToRadians(player->GetRotation().z)));

			rightVec = XMVector3Rotate(rightVec, XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(player->GetRotation().x),
				XMConvertToRadians(player->GetRotation().y),
				XMConvertToRadians(player->GetRotation().z)));
			// 大きさをmoveSpeedにする
			forwardVec = XMVectorScale(forwardVec, moveSpeed);
			rightVec = XMVectorScale(rightVec, moveSpeed);

			XMFLOAT3 forward;
			XMStoreFloat3(&forward, forwardVec);

			XMFLOAT3 right;
			XMStoreFloat3(&right, rightVec);

			if (hitW) {
				pos.x += forward.x;
				pos.y += forward.y;
				pos.z += forward.z;
			}
			else if (hitS) {
				pos.x -= forward.x;
				pos.y -= forward.y;
				pos.z -= forward.z;
			}

			if (hitD) {
				pos.x += right.x;
				pos.y += right.y;
				pos.z += right.z;
			}
			else if (hitA) {
				pos.x -= right.x;
				pos.y -= right.y;
				pos.z -= right.z;
			}

			player->SetPosition(pos);
		}
	}

	// プレイヤーの回転
	{
		XMFLOAT3 playerRot = player->GetRotation();
		constexpr float rotSpeed = 0.1f;

		playerRot.y -= rotSpeed * mousePosDiff.x;
		playerRot.x -= rotSpeed * mousePosDiff.y;
		player->SetRotation(playerRot);
	}

	if (boss->GetAlive())
	{
		// 画像の左上と右下
		XMFLOAT2 aimLT = { aim->GetPosition().x - aim->GetSize().x / 2,
		 aim->GetPosition().y - aim->GetSize().y / 2 };

		XMFLOAT2 aimRB = { aim->GetPosition().x + aim->GetSize().x / 2,
		 aim->GetPosition().y + aim->GetSize().y / 2 };

		// 敵の場所
		XMFLOAT2 enemyPos;

		bool flag = false;

		// 照準と敵のスクリーン座標の当たり判定
		player->SetShotTarget(nullptr);

		enemyPos = { boss->GetFloat2ScreenPos().x,boss->GetFloat2ScreenPos().y };

		// 当たり判定
		if (aimLT.x <= enemyPos.x && aimLT.y <= enemyPos.y &&
			aimRB.x >= enemyPos.x && aimRB.y >= enemyPos.y) {
			flag = true;
			player->SetShotTarget(boss.get());
		}

		{
			char tmp[128];
			sprintf_s(tmp, 128, "boss : %.2f,%.2f", enemyPos.x, enemyPos.y);
			DebugText::GetInstance()->Print(tmp, 0, 0);
			sprintf_s(tmp, 128, "mouse : %.2f,%.2f", aim->GetPosition().x, aim->GetPosition().y);
			DebugText::GetInstance()->Print(tmp, 0, 20);
		}

		if (flag) {
			aim->SetColor({ 1,0,0,1 });
			// 照準が合っていたら左クリック
			if (input->TriggerMouse(Input::LEFT)) {
				// 自機の弾の発射
				player->Shot(pBulletModel.get(), pBulletScale);
			}
		}
		else {
			aim->SetColor({ 1,1,1,1 });

			if (input->TriggerMouse(Input::LEFT)) {
				// 自機の弾の発射
				player->Shot(pBulletModel.get(), pBulletScale);
			}
		}
	}
	else {// ボスが死んだら
		SceneManager::GetInstance()->ChangeScene("END");
	}

	// 敵と自機の弾の当たり判定
	{
		Sphere pBulletShape;

		for (auto& pb : player->GetBullet()) {
			if (!pb.GetAlive())continue;
			pBulletShape.center = XMLoadFloat3(&pb.GetPosition());
			pBulletShape.radius = pb.GetScale().x;

			// 衝突判定をする
			Sphere enemyShape;
			enemyShape.center = XMLoadFloat3(&boss->GetPosition());
			enemyShape.radius = boss->GetScale().x;

			if (!boss->GetAlive()) break;

			// 当たったら消える
			if (Collision::CheckSphere2Sphere(pBulletShape, enemyShape)) {
				boss->SetAlive(false);
				pb.SetAlive(false);
				aim->SetColor({ 1,1,1,1 });

				// パーティクルの発生
				ParticleManager::GetInstance()->CreateParticle(boss->GetPosition(), 100, 10, 10);
			}
		}
	}

	// 敵の弾と自機
	{
		Sphere playerShape;

		playerShape.center = XMLoadFloat3(&player->GetPosition());
		playerShape.radius = player->GetScale().z;

		// 衝突判定をする
		if (player->GetAlive()) {
			for (auto& eb : boss->GetBullet()) {
				Sphere eBulletShape;
				eBulletShape.center = XMLoadFloat3(&eb.GetPosition());
				eBulletShape.radius = eb.GetScale().z;

				// 当たったら消える
				if (Collision::CheckSphere2Sphere(playerShape, eBulletShape)) {
					eb.SetAlive(false);				// 敵の弾を消す
					player->Damage(1);				// プレイヤーにダメージ
					shiftFlag = true;				// RGBずらしをする
					nowFrame = 0;
					if (player->GetHp() == 0) {		// 体力が0になったら
						player->SetAlive(false);

						SceneManager::GetInstance()->ChangeScene("END");
					}
				}
			}
		}

	}

	if (shiftFlag) {

		DamageEffect(maxFrame, nowFrame);
		nowFrame++;
		if (maxFrame < nowFrame) {
			shiftFlag = false;
			nowFrame = 0;
		}
	}

	// モザイク切り替え
	{
		const bool TriggerP = input->TriggerKey(DIK_P);
		if (TriggerP) {
			// モザイクの細かさ
			// ウインドウサイズと同じ細かさのモザイク = モザイクなし
			DirectX::XMFLOAT2 mosaicNum{ WinApp::window_width, WinApp::window_height };

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

	frame++;
}

void BossScene::Draw(DirectXCommon* dxcommon)
{
	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();
	// スプライト描画
	sprite->Draw();

	// 3Dオブジェクト描画前処理
	ObjObject3d::PreDraw();

	// 地面
	groundObj->Draw();

	//スカイドームの描画
	skyDomeObj->Draw();

	// プレイヤーの描画
	player->Draw();

	boss->Draw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	ObjObject3d::PostDraw();
	// パーティクル描画
	ParticleManager::GetInstance()->Draw(dxcommon->GetCmdList());
	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();
}

void BossScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();
	aim->Draw();
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(1.f, 0.f, 1.f, 0.5f));

	if (pause) {
		ImGui::SetNextWindowPos(ImVec2(10.f, 10.f));
		ImGui::SetNextWindowSize(ImVec2(WinApp::window_width - 20, WinApp::window_height - 20));
		ImGui::Begin("pause", nullptr, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text(u8"ESCで戻る");
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x,
			ImGui::GetWindowPos().y + ImGui::GetWindowSize().y));
		ImGui::End();
	}
	else {
		ImGui::SetNextWindowSize(ImVec2(200, 200));
		ImGui::Begin("aaa", nullptr, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text(u8"フレーム %u", frame);
		ImGui::Text(u8"体力 %u", player->GetHp());
		ImGui::Text(u8"WASD:移動");
		ImGui::Text(u8"左クリック:撃つ");
		ImGui::Text(u8"マウス差分 %d,%d", mousePosDiff.x, mousePosDiff.y);
		ImGui::Text(u8"マウス %d,%d", Input::GetInstance()->GetMousePos().x, Input::GetInstance()->GetMousePos().y);

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x,
			ImGui::GetWindowPos().y + ImGui::GetWindowSize().y));
		ImGui::End();
	}

	ImGui::PopStyleColor();
}

XMVECTOR BossScene::SplinePosition(const std::vector<XMVECTOR>& posints, size_t startIndex, float t)
{
	size_t n = posints.size() - 2;

	if (startIndex > n)return posints[n];
	if (startIndex < 1)return posints[1];

	XMVECTOR p0 = posints[startIndex - 1];
	XMVECTOR p1 = posints[startIndex];
	XMVECTOR p2 = posints[startIndex + 1];
	XMVECTOR p3 = posints[startIndex + 2];

	XMVECTOR position = 0.5 * ((2 * p1 + (-p0 + p2) * t) +
		(2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
		(-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t);

	return position;
}

void BossScene::DamageEffect(UINT maxFrame, UINT nowFrame) {
	float rate = (float)nowFrame / (float)maxFrame;

	rate = 1 - rate;
	constexpr float  c4 = 2.f * XM_PI / 3.f;
	float easeRate = -powf(2.f, 10.f * rate - 10.f) * sinf((rate * 10.f - 10.75f) * c4);

	float shiftNum = easeRate * 0.1f;

	PostEffect::GetInstance()->SetShiftG({ shiftNum,0.f });
}