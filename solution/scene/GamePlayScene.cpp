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
	SpriteCommon::GetInstance()->LoadTexture(3, L"Resources/hp/hp.png");
	SpriteCommon::GetInstance()->LoadTexture(6, L"Resources/pouse/pouseBack.png");
	SpriteCommon::GetInstance()->LoadTexture(7, L"Resources/pouse/pouseTitle.png");
	SpriteCommon::GetInstance()->LoadTexture(8, L"Resources/pouse/pouseClose.png");


	// スプライトの生成
	sprite.reset(Sprite::Create(1, { 0,0 }, false, false));
	aim.reset(Sprite::Create(2));


	// hp画像のtexNumberの最初が3
	playerHpSprite.reset(Sprite::Create(3, { 0,1 }));
	playerHpSprite->SetPosition(XMFLOAT3(0, WinApp::window_height, 0));

	// ポーズ画面の画像を作る
	pouseSprite.resize(pouseMax);
	for (UINT i = 0; i < pouseMax; i++) {
		// hp画像のtexNumberの最初が6
		pouseSprite[i].reset(Sprite::Create(i + 6, { 0,0 }));
	}

	// カメラの初期化
	camera.reset(new TrackingCamera());
	//camera->SetEye({ 0, 5, -20 });
	// camera->SetTarget({ 0, 0, 50 });

	ObjObject3d::SetCamera(camera.get());

	// レーン
	lane.reset(new GameObject(nullptr));
	// ターゲットが無い時
	nullTarget.reset(new GameObject(nullptr));

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
	enemyModel.reset(ObjModel::LoadFromObj("enemy"));// enemy

	//デバイスをセット
	FbxObject3d::SetDevice(dxcommon->GetDev());
	//カメラをセット
	FbxObject3d::SetCamera(camera.get());
	//グラフィックスパイプライン生成
	FbxObject3d::CreateGraphicsPipeline();
	// プレイヤー初期化
	player = std::make_unique<Player>();

	// プレイヤーの親を設定
	player->SetParent(lane->GetObj());

	// カメラをプレイヤーの位置にセット
	/*camera->SetTrackingTarget(player.get());
	camera->SetTarget(player->GetPos());
	XMFLOAT3 eye = player->GetPos();
	eye.z -= 50;
	eye.y += 10;
	camera->SetEye(eye);*/

	// カメラをレーンの位置にセット
	camera->SetTrackingTarget(player.get());
	camera->SetTarget(lane->GetPosition());
	XMFLOAT3 eye = lane->GetPosition();
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
	csv = Enemy::LoadCsv("Resources/spline.csv");

	// スタートは2個必要だから最初の座標をあらかじめ入れておく
	points.emplace_back(XMVectorSet(std::stof(csv[0][0]),
		std::stof(csv[0][1]),
		std::stof(csv[0][2]),
		1));

	// csvの中身を格納
	for (auto& i : csv) {
		points.emplace_back(XMVectorSet(std::stof(i[0]),
			std::stof(i[1]),
			std::stof(i[2]),
			1));
	}

	// エンドも2個必要
	points.emplace_back(points.back());

	splineStartIndex = 1;

	// 壁
	wallModel.reset(ObjModel::LoadFromObj("wall"));
	wallObj.resize(points.size());

	for (UINT i = 0; i < wallObj.size(); i++) {
		wallObj[i].resize(2);
		for (UINT x = 0; x < wallObj[i].size(); x++) {
			wallObj[i][x] = ObjObject3d::Create();
			wallObj[i][x]->Initialize();
			wallObj[i][x]->SetModel(wallModel.get());
			wallObj[i][x]->SetScale(XMFLOAT3(1,100,50));
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, points[i]);
			pos.x -= 12.f;
			pos.x += 24.f * (float)x;
			wallObj[i][x]->SetPosition(pos);
		}
	}

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
				nowframe += std::stoul(y[1]);
				enemyFrame.emplace_back(nowframe, nowframe + 60 * 39);// (敵が出る時間,敵が消える時間)
			}
		}
	}
}

void GamePlayScene::Finalize()
{
	DamageEffect(1, 1);
}

void GamePlayScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
		pause = !pause;
	}

	//const bool TriggerESC = Input::GetInstance()->TriggerKey(DIK_ESCAPE);
	//if (TriggerESC) {
	//	WM_DESTROY; //ウィンドウが破棄された
	//	PostQuitMessage(0); //OSに対して、アプリの終了を伝える
	//}

	{
		char tmp[128];
		sprintf_s(tmp, 128, "mouse : %.2f,%.2f", aim->GetPosition().x, aim->GetPosition().y);
		DebugText::GetInstance()->Print(tmp, 0, 20);
	}

	if (!pause) {
		// シーン遷移
		updateProcess();


		// パーティクル更新
		ParticleManager::GetInstance()->Update();

		camera->Update();
		lane->Update();

		//fbxObj->Update();
		player->Update();
		//}
		for (auto& i : enemy) {
			i->Update();
		}

		for (auto& i : wallObj) {
			for (auto& x : i) {
				x->Update();
			}
		}

		groundObj->Update();
		skyDomeObj->Update();

		// スプライト更新
		sprite->Update();

		// aim->SetPosition({player->GetScreenAimPos().x,player->GetScreenAimPos().y,0});
		aim->SetPosition({ (float)Input::GetInstance()->GetMousePos().x,(float)Input::GetInstance()->GetMousePos().y,0 });
		aim->Update();

		if (player->GetHp() > 0) {

			// スプライト横幅 = 最大値 * hpの割合
			playerHpSprite->SetSize(XMFLOAT2(playerHpSprite->GetTexSize().x * (float)player->GetHp() / playerHpMax,
				playerHpSprite->GetSize().y));

			playerHpSprite->TransferVertexBuffer();

			// 体力バー
			playerHpSprite->Update();
		}

	}// ポーズ画面
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
void GamePlayScene::start()
{
	Input* input = Input::GetInstance();
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 50;

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
		sprintf_s(tmp, 32, "%.2f,%.2f,%.2f", player->GetPosition().x, player->GetPosition().y, player->GetPosition().z);
		DebugText::GetInstance()->Print(tmp, 0, 100);
	}

	// レーンの位置
	/*{
		auto pos = lane->GetPosition();
		pos.z += 0.2;
		lane->SetPosition(pos);

	}*/
		skyDomeObj->SetPosition(lane->GetPosition());

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
			XMFLOAT3 rot {};

			if (hitSpace && avoidFrame == 0) {
				moveSpeed *= 10;
				avoidFrame = avoidFrameMax;
			}

			if (hitW && pos.y < 8.f) {
				pos.y += moveSpeed;
				rot.x -= 5.f;
			}
			else if (hitS && pos.y > -4.f) {
				pos.y -= moveSpeed;
				rot.x += 5.f;
			}

			if (hitD && pos.x < 10.f) {
				pos.x += moveSpeed;
				rot.z -= 5.f;
			}
			else if (hitA && pos.x > -10.f) {
				pos.x -= moveSpeed;
				rot.z += 5.f;
			}

			player->SetPosition(pos);
			player->SetRotation(rot);
		}
	}

	// プレイヤーの回転
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{
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

	// 敵を発生
	for (auto& i : enemyFrame) {
		if (i.first <= frame) {// 敵の速度を設定
			auto& a = EnemyAdd(enemyPos[addedEnemyNum], { 0.f, 0.f, -0.25f });
			addedEnemyNum++;
			a->SetLifeSpan(i.second);
		}
	}

	enemyFrame.remove_if([&](std::pair<UINT, UINT>& i) {return i.first <= frame; });

	// 敵が全部居なくなったらエンドシーンに行く
	{
		if (enemyFrame.empty() && enemy.empty()) {
			SceneManager::GetInstance()->ChangeScene("END");
		}
	}


	for (auto& i : enemy) {
		// 敵がZ軸0に行ったら行動パターンをleaveに変える
		if (i->GetPosition().z < 0) {
			i->LeaveChange(XMFLOAT3(0.5f, 0.5f, 0.f));
		}

		// z軸が0座標y軸が10以上(画面外)に行ったら消す
		if (i->GetPosition().z < 0.f && i->GetPosition().y > 10.f) {
			i->SetAlive(false);
		}

		// x軸が13以上だったら左に行く
		if (i->GetPosition().x >= 13.f) {
			i->DirectionChange(XMFLOAT3(-0.1f, 0.f, 0.f));
		}

		// x軸が-13以下だったら右に行く
		if (i->GetPosition().x <= -13.f) {
			i->DirectionChange(XMFLOAT3(0.4f, 0.f, 0.f));
		}
	}
	enemy.remove_if([&](std::unique_ptr<Enemy>& i) {return i->GetLifeSpan() <= frame; });

	// 敵が居たら照準と敵の当たり判定をする
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

		// 照準と敵のスクリーン座標の当たり判定
		player->SetShotTarget(nullptr);
		for (auto& i : enemy) {
			if (!i->GetAlive())continue;
			enemyPos = { i->GetFloat2ScreenPos().x,i->GetFloat2ScreenPos().y };

			// 当たり判定
			if (aimLT.x <= enemyPos.x && aimLT.y <= enemyPos.y &&
				aimRB.x >= enemyPos.x && aimRB.y >= enemyPos.y) {
				flag = true;

				player->SetShotTarget(i.get());
			}
		}

		if (flag) {
			aim->SetColor({ 1,0,0,1 });
			// 照準が合っていたら左クリック
			if (input->TriggerMouse(Input::LEFT)) {
				// 自機の弾の発射
				player->Shot(pBulletModel.get(), pBulletScale);

				XMFLOAT3 pos = lane->GetPosition();
				pos.x += player->GetPosition().x;
				pos.y += player->GetPosition().y;
				pos.z += player->GetPosition().z;
			}
		}
		else {
			// 照準が白
			aim->SetColor({ 1,1,1,1 });
		}
	}


	// 敵と自機の弾の当たり判定
	{
		Sphere pBulletShape;

		for (auto& pb : player->GetBullet()) {
			if (!pb.GetAlive())continue;
			pBulletShape.center = XMLoadFloat3(&pb.GetPosition());
			pBulletShape.radius = pb.GetScale().x;

			// 衝突判定をする
			for (auto& e : enemy) {
				if (!e->GetAlive())continue;
				Sphere enemyShape;
				enemyShape.center = XMLoadFloat3(&e->GetPosition());
				enemyShape.radius = e->GetScale().x;

				// 当たったら
				if (Collision::CheckSphere2Sphere(pBulletShape, enemyShape)) {
					pb.SetAlive(false);

					e->Damage(1);				// 敵にダメージ

					if (e->GetHp() <= 0) {		// 体力が0以下になったら
						e->SetAlive(false);

						// パーティクルの場所を設定
						XMFLOAT3 pos = lane->GetPosition();
						pos.x += e->GetPosition().x;
						pos.y += e->GetPosition().y;
						pos.z += e->GetPosition().z;

						// パーティクルの発生
						ParticleManager::GetInstance()->CreateParticle(pos, 100, 4, 10);
					}

					// パーティクルの場所を設定
					XMFLOAT3 pos = lane->GetPosition();
					pos.x += e->GetPosition().x;
					pos.y += e->GetPosition().y;
					pos.z += e->GetPosition().z;

					// 振動
					e->SetShake(true);
					// ヒットストップ
					e->SetHitStop(true);

					// パーティクルの発生
					ParticleManager::GetInstance()->CreateParticle(pos, 10, 4, 5);
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

		playerShape.center = XMLoadFloat3(&player->GetPosition());
		playerShape.radius = player->GetScale().z;

		// 衝突判定をする
		if (player->GetAlive()) {
			for (auto& e : enemy) {
				for (auto& eb : e->GetBullet()) {
					Sphere eBulletShape;
					eBulletShape.center = XMLoadFloat3(&eb.GetPosition());
					eBulletShape.radius = eb.GetScale().z;

					// 当たったら消える
					if (Collision::CheckSphere2Sphere(playerShape, eBulletShape)) {
						eb.SetAlive(false);				// 敵の弾を消す
						// player->Damage(1);				// プレイヤーにダメージ
						shiftFlag = true;				// RGBずらしをする
						shiftNowFrame = 0;
						if (player->GetHp() == 0) {		// 体力が0になったら
							player->SetAlive(false);

							SceneManager::GetInstance()->ChangeScene("END");
						}
						break;
					}
				}
			}
		}
	}

	// ボスシーンに行く
	if (input->TriggerKey(DIK_F)) {
		SceneManager::GetInstance()->ChangeScene("BOSSPLAY");
	}

	if (shiftFlag) {

		DamageEffect(shiftMaxFrame, shiftNowFrame);
		shiftNowFrame++;
		if (shiftMaxFrame < shiftNowFrame) {
			shiftFlag = false;
			shiftNowFrame = 0;
		}
	}

	// スプライン曲線で移動
	{
		splineFrame++;
		float timeRate = (float)splineFrame / 120.f;
		if (timeRate >= 1.0f)
		{
			if (splineStartIndex < points.size() - 3) {
				splineStartIndex++;
				timeRate -= 1.0f;
				splineFrame = 0;
			}
			else
			{
				timeRate = 1.0f;
			}
		}
		// ベクターをフロートに変換
		XMFLOAT3 splineFloat;
		XMStoreFloat3(&splineFloat, SplinePosition(points, splineStartIndex, timeRate));
		lane->SetPosition(splineFloat);
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

	for (auto& i : wallObj) {
		for (auto& x : i) {
			x->Draw();
		}
	}

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

	if (player->GetHp() > 0) {
		playerHpSprite->Draw();
	}

	aim->Draw();
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(1.f, 0.f, 1.f, 0.5f));

	if (pause) {
		/*ImGui::SetNextWindowPos(ImVec2(10.f, 10.f));
		ImGui::SetNextWindowSize(ImVec2(WinApp::window_width - 20, WinApp::window_height - 20));
		ImGui::Begin("pause", nullptr, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text(u8"ESCで戻る");
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x,
			ImGui::GetWindowPos().y + ImGui::GetWindowSize().y));
		ImGui::End();*/
		// ポーズ画面描画
		pouseSprite[pouse]->Draw();

	}
	else {
		ImGui::SetNextWindowSize(ImVec2(100, 200));
		ImGui::Begin("aaa", nullptr, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text(u8"フレーム %u", frame);
		ImGui::Text(u8"体力 %u", player->GetHp());
		ImGui::Text(u8"WASD:移動");
		ImGui::Text(u8"左クリック:撃つ");
		ImGui::Text(u8"スペース:回避");

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x,
			ImGui::GetWindowPos().y + ImGui::GetWindowSize().y));
		ImGui::End();
	}

	ImGui::PopStyleColor();
}

XMVECTOR GamePlayScene::SplinePosition(const std::vector<XMVECTOR>& posints, size_t startIndex, float t)
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

std::unique_ptr<Enemy>& GamePlayScene::EnemyAdd(XMFLOAT3 pos, XMFLOAT3 vel)
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

	e->SetParent(lane->GetObj());

	return e;
}

void GamePlayScene::DamageEffect(UINT maxFrame, UINT nowFrame) {
	float rate = (float)nowFrame / (float)maxFrame;

	rate = 1 - rate;
	constexpr float  c4 = 2.f * XM_PI / 3.f;
	float easeRate = -powf(2, 10.f * rate - 10.f) * sinf((rate * 10.f - 10.75f) * c4);

	float shiftNum = easeRate * 0.1f;

	PostEffect::GetInstance()->SetShiftG({ shiftNum,0.f });
}