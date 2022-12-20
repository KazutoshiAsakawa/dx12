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

XMFLOAT3 lerp(const XMFLOAT3& a, const XMFLOAT3& b, float t)
{
	XMFLOAT3 ret;
	ret.x = a.x + t * (b.x - a.x);
	ret.y = a.y + t * (b.y - a.y);
	ret.z = a.z + t * (b.z - a.z);
	return ret;
}

void BossScene::Initialize(DirectXCommon* dxcommon)
{
	// マウスカーソルを消す
	ShowCursor(false);

	// スプライト共通テクスチャ読み込み
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/gameplay.png");
	SpriteCommon::GetInstance()->LoadTexture(2, L"Resources/aim.png");
	SpriteCommon::GetInstance()->LoadTexture(3, L"Resources/hp/hp.png");
	SpriteCommon::GetInstance()->LoadTexture(4, L"Resources/hp/hpSlide.png");
	SpriteCommon::GetInstance()->LoadTexture(6, L"Resources/pouse/pauseBack.png");
	SpriteCommon::GetInstance()->LoadTexture(7, L"Resources/pouse/pauseTitle.png");
	SpriteCommon::GetInstance()->LoadTexture(8, L"Resources/pouse/pauseClose.png");

	// スプライトの生成
	sprite.reset(Sprite::Create(1, { 0,0 }, false, false));
	aim.reset(Sprite::Create(2));
	aim->SetPosition({ WinApp::window_width / 2.0f ,WinApp::window_height / 2.0f,0.f });

	// hp画像
	playerHpSprite.reset(Sprite::Create(3, { 0,1 }));
	playerHpSprite->SetPosition(XMFLOAT3(40, WinApp::window_height - 40, 0));

	playerHpSlide.reset(Sprite::Create(4, { 0,1 }));
	playerHpSlide->SetPosition(XMFLOAT3(35, WinApp::window_height - 35, 0));

	// ポーズ画面の画像を作る
	pouseSprite.resize(pouseMax);
	for (UINT i = 0; i < pouseMax; i++) {
		// hp画像のtexNumberの最初が6
		pouseSprite[i].reset(Sprite::Create(i + 6, { 0,0 }));
	}

	// カメラの初期化
	camera.reset(new TrackingCamera());

	ObjObject3d::SetCamera(camera.get());

	// OBJからモデルデータを読み込む

	// スカイドーム
	skyDomeModel.reset(ObjModel::LoadFromObj("skydome"));
	skyDomeObj = ObjObject3d::Create();
	skyDomeObj->SetModel(skyDomeModel.get());
	skyDomeObj->SetScale({ 5,5,5 });

	// 地面
	groundModel.reset(ObjModel::LoadFromObj("ground"));
	groundModel->SetTiling({ 100.f,100.f });
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

	player->SetHp(4);

	// カメラをプレイヤーの位置にセット
	camera->SetTrackingTarget(player.get());
	camera->SetTarget(player->GetPosition());
	XMFLOAT3 eye = player->GetPosition();
	eye.z -= 500;
	eye.y += 50;
	camera->SetEye(eye);

	// ボス
	boss.reset(new Boss(enemyModel.get(), { 0.f,0.f,50.f }));

	boss->SetPhaseApproach();
	boss->SetAttackTarget(player.get());
	boss->SetBulletModel(enemyModel.get());
	boss->SetScale({2,2,2});

	// ボスの体力
	boss->SetHp(30);

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

	// マウスカーソルを出す
	ShowCursor(true);
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

		player->Update();

		boss->Update();

		groundObj->Update();
		skyDomeObj->Update();

		// スプライト更新
		sprite->Update();

		aim->Update();

		if (player->GetHp() > 0) {

			// スプライト横幅 = 最大値 * hpの割合
			playerHpSprite->SetSize(XMFLOAT2(playerHpSprite->GetTexSize().x * (float)player->GetHp() / playerHpMax,
				playerHpSprite->GetSize().y));

			playerHpSprite->TransferVertexBuffer();

			// 体力バー
			playerHpSprite->Update();

			playerHpSlide->Update();
		}
	}
	else {
		pouseSprite[pouse]->Update();
		if (Input::GetInstance()->TriggerKey(DIK_W)) {
			pouse--;
		}
		if (Input::GetInstance()->TriggerKey(DIK_S)) {
			pouse++;
		}
		if (pouse >= 3) {
			pouse = 2;
		}
		if (pouse <= -1) {
			pouse = 0;
		}

		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			if (pouse == 0) {
				pause = !pause;
			}
			else if (pouse == 1) {
				SceneManager::GetInstance()->ChangeScene("TITLE");
			}
			else if (pouse == 2) {
				WM_DESTROY; //ウィンドウが破棄された
				PostQuitMessage(0); //OSに対して、アプリの終了を伝える
			}
		}
	}
}

// シーン遷移
void BossScene::start()
{
	Input* input = Input::GetInstance();
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 120;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcessにplay関数をセット
		updateProcess = std::bind(&BossScene::play, this);
		mosaicFrame = 0;
	}
	else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);

		rate *= rate * rate;

		constexpr XMFLOAT3 startEye = { 100,0,-100 };
		constexpr XMFLOAT3 endEye = { 0,0,0 };

		XMFLOAT3 eye;
		eye = lerp(startEye, endEye, rate);
		player->SetPosition(eye);

		constexpr XMFLOAT3 startRota = {0.f, 360.f, 0.f};
		constexpr XMFLOAT3 endRota = {0.f, 0.f, 0.f};

		XMFLOAT3 rota;
		rota = lerp(startRota, endRota, rate);
		player->SetRotation(rota);
	}
}

void BossScene::play()
{
	Input* input = Input::GetInstance();

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

		if (hitW || hitS || hitA || hitD || hitZ || hitX || hitSpace) {
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

			// 移動制限(地面)
			if (0 > pos.y) {
				pos.y = 0;
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

		// プレイヤー回転制限
		if (playerRot.x > 60.f) {
			playerRot.x = 60.f;
		}
		if (playerRot.x < -60.f) {
			playerRot.x = -60.f;
		}

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
	else if(killBossFlag){// ボスが死んだら
		updateProcess = std::bind(&BossScene::end, this, "CLEAR");
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
				boss->Damage(1);
				pb.SetAlive(false);
				aim->SetColor({ 1,1,1,1 });

				if (boss->GetHp() == 0)
				{
					boss->SetAlive(false);
					killBossFlag = true;

					camera->SetTrackingTarget(boss.get());
					updateProcess = std::bind(&BossScene::killEffect, this);
					XMFLOAT3 angle = camera->GetAngle();
					angle.y += 180.f;
					camera->SetAngle(angle);
				}

				// パーティクルの発生
				ParticleManager::GetInstance()->CreateParticle(boss->GetPosition(), 10, 4, 5);
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
				if (!eb.GetAlive())continue;
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

						updateProcess = std::bind(&BossScene::end, this, "GAMEOVER");
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

void BossScene::killEffect()
{
	constexpr UINT effectFrameMax = 180;

	if(++nowEffectFrame > effectFrameMax)
	{
		updateProcess = std::bind(&BossScene::end, this, "GAMEOVER");
	} else
	{
		// パーティクルの発生
		ParticleManager::GetInstance()->CreateParticle(boss->GetPosition(), 10, 1, 5);
	}
}

void BossScene::end(const std::string& nextScene)
{
	Input* input = Input::GetInstance();
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 50;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		//PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// ボスシーンへ
		SceneManager::GetInstance()->ChangeScene(nextScene);
	}
	else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		rate = 1 - rate;// 1から0
		rate *= rate;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
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

	// 体力
	if (player->GetHp() > 0) {
		playerHpSprite->Draw();
	}
	playerHpSlide->Draw();


	aim->Draw();
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(1.f, 0.f, 1.f, 0.5f));

	if (pause) {
		pouseSprite[pouse]->Draw();
	}
	else {
		ImGui::SetNextWindowSize(ImVec2(200, 200));
		ImGui::Begin(u8"説明", nullptr, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text(u8"WASD:移動");
		ImGui::Text(u8"左クリック:撃つ");
		ImGui::Text(u8"スペース:回避");
		ImGui::Text(u8"ESC:ポーズ画面");
		ImGui::Text(u8"ボス体力:%u", boss->GetHp());

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