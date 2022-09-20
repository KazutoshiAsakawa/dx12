#include "GamePlayScene.h"
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

void GamePlayScene::Initialize(DirectXCommon* dxcommon)
{
	// スプライト共通テクスチャ読み込み
	SpriteCommon::GetInstance()->LoadTexture(1, L"Resources/gameplay.png");
	SpriteCommon::GetInstance()->LoadTexture(2, L"Resources/aim.png");

	// スプライトの生成
	sprite.reset(Sprite::Create(1, { 0,0 }, false, false));
	aim.reset(Sprite::Create(2));

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
	groundObj = ObjObject3d::Create();
	groundObj->SetModel(groundModel.get());
	groundObj->SetScale({1,1,1});
	groundObj->SetPosition({0,-5,0});

	// 自機の読み込み
	pBulletModel.reset(ObjModel::LoadFromObj("playerBullet"));
	// 敵の読み込み
	enemyModel.reset(ObjModel::LoadFromObj("enemy"));

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
	camera->SetTarget(player->GetPos());
	XMFLOAT3 eye = player->GetPos();
	eye.z -= 50;
	eye.y += 10;
	camera->SetEye(eye);

	// 敵
	enemy.resize(0);

	// パーティクル初期化
	ParticleManager::GetInstance()->SetCamera(camera.get());

	// 更新処理の関数を入れる
	updateProcess = std::bind(&GamePlayScene::start, this);

	// 音声読み込み
	Audio::GetInstance()->LoadWave("Alarm01.wav");

	// 音声再生
	// audio->PlayWave("Alarm01.wav");

	// スプライン曲線
	// posints = { start, start, p2, p3, end, end }
	points.emplace_back(XMVectorSet(0, 0, 0, 0));
	points.emplace_back(XMVectorSet(0, 0, 0, 0));
	points.emplace_back(XMVectorSet(0, 0, 20, 0));
	points.emplace_back(XMVectorSet(0, 20, 40, 0));
	points.emplace_back(XMVectorSet(0, 0, 0, 0));
	points.emplace_back(XMVectorSet(0, 0, 0, 0));

	splineStartIndex = 1;

	// CSV読み込み
	csv = Enemy::LoadCsv("Resources/enemy.csv");

	{
		UINT nowframe = 0;

		for (auto& y : csv) {
			if (y[0].find("//") == 0) {
				continue;
			}
			if (y[0] == "POPPOSITION") {
				// 敵を出す
				enemyPos.emplace_back(std::stof(y[1]), std::stof(y[2]), std::stof(y[3]));
			}
			else if (y[0] == "POPTIME") {
				// 指定時間
				nowframe += std::stof(y[1]);
				enemyFrame.emplace_back(nowframe, nowframe + 60 * 40);// (敵が出る時間,敵が消える時間)
			}
		}
	}
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	// シーン遷移
	updateProcess();

	// パーティクル更新
	ParticleManager::GetInstance()->Update();

	camera->Update();
	//fbxObj->Update();
	player->Update();
	//}
	for (auto& i : enemy) {
		i->Update();
	}

	groundObj->Update();
	skyDomeObj->Update();

	// スプライト更新
	sprite->Update();

	// aim->SetPosition({player->GetScreenAimPos().x,player->GetScreenAimPos().y,0});
	aim->SetPosition({ (float)Input::GetInstance()->GetMousePos().x,(float)Input::GetInstance()->GetMousePos().y,0 });
	aim->Update();
}

// シーン遷移
void GamePlayScene::start()
{
	Input* input = Input::GetInstance();
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 40;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcessにplay関数をセット
		updateProcess = std::bind(&GamePlayScene::play, this);
	}
	else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void GamePlayScene::play()
{
	Input* input = Input::GetInstance();

	{
		char tmp[32]{};
		sprintf_s(tmp, 32, "%u", frame);
		DebugText::GetInstance()->Print(tmp, 0, 50);
	}

	// 座標操作
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{
		// プレイヤーの回転
		XMFLOAT3 playerRot = player->GetRotation();
		constexpr float rotSpeed = 1.f;

		if (input->PushKey(DIK_RIGHT)) {
			playerRot.y += rotSpeed;
		}
		if (input->PushKey(DIK_LEFT)) {
			playerRot.y -= rotSpeed;
		}
		if (input->PushKey(DIK_UP)) {
			playerRot.x -= rotSpeed;
		}
		if (input->PushKey(DIK_DOWN)) {
			playerRot.x += rotSpeed;
		}
		player->SetRotation(playerRot);
	}

	// 0を押したら
	if (input->TriggerKey(DIK_0)) {
		// 自機の弾の発射
		player->Shot(pBulletModel.get(), pBulletScale);
		// パーティクルの発生
		ParticleManager::GetInstance()->CreateParticle(player->GetPos(), 100, 1, 1);
	}

	// 敵を発生
	for (auto& i : enemyFrame) {
		if (i.first <= frame) {
			auto& a = enemyAdd(enemyPos[addedEnemyNum], { 0, 0, -0.1 });
			addedEnemyNum++;
			a->SetLifeSpan(i.second);
		}
	}

	enemyFrame.remove_if([&](std::pair<UINT, UINT>& i) {return i.first <= frame; });

	// 敵がZ軸0に行ったら行動パターンをleaveに変える
	for (auto& i : enemy) {
		if (i->GetPos().z < 0) {
			i->leaveChange(XMFLOAT3(0.5, 0.5, 0));
		}

	}
	enemy.remove_if([&](std::unique_ptr<Enemy>& i) {return i->GetLifeSpan() <= frame; });

	if (!enemy.empty())
	{
		// 画像の左上と右下
		XMFLOAT2 aimLT = { (float)input->GetMousePos().x - aim->GetSize().x / 2,
		(float)input->GetMousePos().y - aim->GetSize().y / 2 };

		XMFLOAT2 aimRB = { (float)input->GetMousePos().x + aim->GetSize().x / 2,
		(float)input->GetMousePos().y + aim->GetSize().y / 2 };

		// 敵の場所
		XMFLOAT2 enemyPos;

		bool flag = false;

		player->SetShotTarget(nullptr);
		for (auto& i : enemy) {
			if(!i->GetAlive())continue;
			enemyPos = { i->GetFloat2ScreenPos().x,i->GetFloat2ScreenPos().y};

			// 当たり判定
			if (aimLT.x <= enemyPos.x && aimLT.y <= enemyPos.y &&
				aimRB.x >= enemyPos.x && aimRB.y >= enemyPos.y) {
				flag = true;
				player->SetShotTarget(i.get());
			}
		}

		if (flag) {
			DebugText::GetInstance()->Print("aaaa",0,0,5);
		}
	}

	// 敵と自機の弾の当たり判定
	{
		Sphere pBulletShape;

		for (auto& pb : player->GetBullet()) {
			if (!pb.GetAlive())continue;
			pBulletShape.center = XMLoadFloat3(&pb.GetPos());
			pBulletShape.radius = pb.GetScale().x;

			// 衝突判定をする
			for (auto& e : enemy) {
				if (!e->GetAlive())continue;
				Sphere enemyShape;
				enemyShape.center = XMLoadFloat3(&e->GetPos());
				enemyShape.radius = e->GetScale().x;

				// 当たったら消える
				if (Collision::CheckSphere2Sphere(pBulletShape, enemyShape)) {
					e->SetAlive(false);
					pb.SetAlive(false);
					break;
				}
			}
		}
		// 敵を消す
		enemy.erase(std::remove_if(enemy.begin(), enemy.end(),
			[](const std::unique_ptr <Enemy>& i) {return !i->GetAlive() && i->GetBullet().empty(); }), enemy.end());
	}

	// 敵の弾と自機
	{
		Sphere playerShape;

		playerShape.center = XMLoadFloat3(&player->GetPos());
		playerShape.radius = player->GetScale().z;

		// 衝突判定をする
		if (player->GetAlive()) {
			for (auto& e : enemy) {
				if (!e->GetAlive())continue;
				for (auto& eb : e->GetBullet()) {
					Sphere eBulletShape;
					eBulletShape.center = XMLoadFloat3(&eb.GetPos());
					eBulletShape.radius = eb.GetScale().z;

					// 当たったら消える
					if (Collision::CheckSphere2Sphere(playerShape, eBulletShape)) {
						eb.SetAlive(false);
						// player->SetAlive(false);
						break;
					}
				}
			}
		}
	}

	// スプライン曲線で移動
	//{
	//	frame++;
	//	float timeRate = (float)frame / 120.f;

	//	if (timeRate >= 1.0f)
	//	{
	//		if (splineStartIndex < points.size() - 3) {
	//			splineStartIndex++;
	//			timeRate -= 1.0f;
	//			frame = 0;
	//		}
	//		else
	//		{
	//			timeRate = 1.0f;
	//		}
	//	}

	//	// ベクターをフロートに変換
	//	XMFLOAT3 splineFloat;
	//	XMStoreFloat3(&splineFloat, splinePosition(points, splineStartIndex, timeRate));

	//	player->SetPos(splineFloat);
	//}

	// シーン切り替え
	if (input->TriggerKey(DIK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene("END");
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

void GamePlayScene::Draw(DirectXCommon* dxcommon)
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
	// 敵の複数描画
	for (auto& i : enemy) {
		i->Draw();
	}
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

void GamePlayScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();
	aim->Draw();
}

XMVECTOR GamePlayScene::splinePosition(const std::vector<XMVECTOR>& posints, size_t startIndex, float t)
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

std::unique_ptr<Enemy>& GamePlayScene::enemyAdd(XMFLOAT3 pos, XMFLOAT3 vel)
{
	enemy.emplace_back();
	auto& e = enemy.back();

	// 敵の初期座標
	e = std::make_unique<Enemy>(enemyModel.get(), pos);
	// 敵の大きさ
	e->SetScale(XMFLOAT3(enemyScale, enemyScale, enemyScale));
	// 敵の速度
	e->SetVel(vel);
	// 敵の標的
	e->SetShotTarget(player.get());

	return e;
}