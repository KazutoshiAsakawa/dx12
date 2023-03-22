#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

#include "FbxObject3d.h"
#include "DirectXCommon.h"
#include "Collision.h"
#include "ParticleLoad.h"

#include "PostEffect.h"
#include "Game.h"

using namespace DirectX;

namespace {
	XMFLOAT3 lerp(const XMFLOAT3& a, const XMFLOAT3& b, float t) {
		XMFLOAT3 ret;
		ret.x = a.x + t * (b.x - a.x);
		ret.y = a.y + t * (b.y - a.y);
		ret.z = a.z + t * (b.z - a.z);
		return ret;
	}
}

void GamePlayScene::Initialize(DirectXCommon* dxcommon) {

	// Inputクラスはシングルトンクラス
	// 【シングルトンクラス】オブジェクトを一つしか作れないクラス
	// 【インスタンス】クラス型の変数のこと
	// 【Input::GetInstance関数】Inputクラスのインスタンス(のポインタ)を取得する関数

	// Inputクラスのインスタンス(へのポインタ)を変数に入れておく
	input = Input::GetInstance();

	// マウスカーソルを消す
	ShowCursor(false);

#pragma region 操作説明のスプライト

	// 操作説明のスプライト
	operationSprite["W"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/operation/W.png"), { 0.5f, 0.5f }, false, false));
	operationSprite["S"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/operation/S.png"), { 0.5f, 0.5f }, false, false));
	operationSprite["A"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/operation/A.png"), { 0.5f, 0.5f }, false, false));
	operationSprite["D"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/operation/D.png"), { 0.5f, 0.5f }, false, false));
	operationSprite["L_Click"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/operation/L_Click.png"), { 0.f, 0.f }, false, false));
	operationSprite["ESC_Pause"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/operation/ESC_Pause.png"), { 0.f, 0.f }, false, false));
	// 最初は表示する
	operationSprite["ESC_Pause"]->SetIsInvisible(false);

	operationSprite["W"]->SetPosition({ WinApp::window_width / 2.f,
										WinApp::window_height / 2.f + 50.f,
										0.f
		});
	operationSprite["S"]->SetPosition({ WinApp::window_width / 2.f,
										WinApp::window_height / 2.f + 225.f,
										0.f
		});
	operationSprite["A"]->SetPosition({ WinApp::window_width / 2.f - 100.f,
										WinApp::window_height / 2.f + 150.f,
										0.f
		});
	operationSprite["D"]->SetPosition({ WinApp::window_width / 2.f + 100.f,
										WinApp::window_height / 2.f + 150.f,
										0.f
		});

	// ESC以外の操作説明を非表示にする
	for (auto& i : operationSprite) {
		if (i.first != "ESC_Pause") {
			i.second->SetIsInvisible(true);
		}
	}

#pragma endregion 操作説明のスプライト

	// スプライトの生成
	aim.reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/aim.png")));

#pragma region プレイヤー
	// hp画像
	playerHpSpriteSize = { 330.f, 30.f };

	playerHpSprite.reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/hp/playerHp.png"), { 0,1 }));
	playerHpSprite->SetPosition(XMFLOAT3(35, WinApp::window_height - 35, 0));

	playerHpSlide.reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/hp/hpSlide.png"), { 0,1 }));
	playerHpSlide->SetPosition(XMFLOAT3(35, WinApp::window_height - 35, 0));
	playerHpSlide->SetSize(playerHpSpriteSize);
	playerHpSlide->TransferVertexBuffer();

#pragma endregion プレイヤー

#pragma region ポーズ画面

	// ポーズ画面の画像を作る
	pauseSprite.resize(pauseMax);
	pauseSprite[0].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/pause/pauseBack.png"), { 0.f, 0.f }));
	pauseSprite[1].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/pause/pauseTitle.png"), { 0.f, 0.f }));
	pauseSprite[2].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/pause/pauseClose.png"), { 0.f, 0.f }));

#pragma endregion ポーズ画面

	// カメラの初期化
	camera.reset(new TrackingCamera());
	normalCamera.reset(new Camera(WinApp::window_width, WinApp::window_height));

	// カメラをセット
	nowCamera = normalCamera.get();
	ObjObject3d::SetCamera(normalCamera.get());

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
	groundModel->SetTiling({ 5.f,5.f });
	groundObj = ObjObject3d::Create();
	groundObj->SetModel(groundModel.get());
	groundObj->SetScale({ 100,100,100 });
	groundObj->SetPosition({ 0,-5,0 });

	// 自機の読み込み
	pBulletModel.reset(ObjModel::LoadFromObj("bullet"));
	// 敵の読み込み
	enemyModel.reset(ObjModel::LoadFromObj("butterfly")); // enemy2

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

	// プレイヤーの体力を設定
	player->SetHp(playerHpMax);

	// プレイヤー登場演出の開始時座標
	playerEntryStartPos = { 0,0,-100 };
	// プレイヤー登場演出の終了時座標
	playerEntryEndPos = { 0,0,0 };
	player->SetPosition(playerEntryStartPos);

#pragma region カメラ
	camera->SetTarget(lane->GetPosition());
	XMFLOAT3 eye = lane->GetPosition();
	eye.z -= 50;
	eye.y += 10;
	camera->SetEye(eye);

	eye.x += 25.f;
	normalCamera->SetEye(eye);
	normalCamera->SetUp({ 0,1,0 });
	XMFLOAT3 targetPos = lane->GetPosition();
	targetPos.x += player->GetPosition().x;
	targetPos.y += player->GetPosition().y;
	targetPos.z += player->GetPosition().z;
	normalCamera->SetTarget(targetPos);
#pragma endregion カメラ

	// 敵
	enemy.resize(0);

	// パーティクル初期化
	ParticleLoad::GetInstance()->SetCamera(camera.get());

	// 更新処理の関数を入れる
	updateProcess = std::bind(&GamePlayScene::start, this);

#pragma region 音

	// SE読み込み
	Audio::GetInstance()->LoadWave("sound/Kagura_Suzu02-1.wav");

	// Resources/に続くファイルパス
	bgmFileName = "sound/Gap-Ver.2.wav";

	// BGM読み込み
	Audio::GetInstance()->LoadWave(bgmFileName);
	// BGMを再生
	Audio::GetInstance()->PlayWave(bgmFileName, 0.04f);

#pragma endregion 音

#pragma region スプライン曲線
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
#pragma endregion スプライン曲線

	// 神社
	shrineObj = ObjObject3d::Create();
	shrineObj->SetModel(ObjModel::LoadFromObj("shrine"));
	shrineObj->SetScale({ 3,3,3 });
	shrineObj->SetRotation({ 0,0,0 });
	XMFLOAT3 shrinePos = playerEntryStartPos;	// プレイヤーのスタート位置に合わせる
	shrinePos.z -= player->GetScale().z * 2.f;	// 後ろにずらす
	shrinePos.y -= 9.f;							// 下にずらす
	shrineObj->SetPosition(shrinePos);

	// 篝火
	bonfireR = ObjObject3d::Create();
	bonfireR->SetModel(ObjModel::LoadFromObj("bonfire"));
	bonfireR->SetScale({ 2,2,2 });
	// 神社から計算して配置
	XMFLOAT3 bonfireRPos = shrinePos;
	bonfireRPos.x += 5.f;
	bonfireRPos.y += 4.f;
	bonfireRPos.z += 19.f;
	bonfireR->SetPosition({ bonfireRPos });

	bonfireL = ObjObject3d::Create();
	bonfireL->SetModel(ObjModel::LoadFromObj("bonfire"));
	bonfireL->SetScale({ 2,2,2 });
	XMFLOAT3 bonfireLPos = shrinePos;
	bonfireLPos.x -= 5.f;
	bonfireLPos.y += 4.f;
	bonfireLPos.z += 19.f;
	bonfireL->SetPosition({ bonfireLPos });

	// 鳥居
	toriiModel.reset(ObjModel::LoadFromObj("torii"));
	toriiObj.resize(points.size());

	for (UINT i = 0; i < toriiObj.size(); i++) {
		toriiObj[i].resize(2);
		for (UINT x = 0; x < toriiObj[i].size(); x++) {
			toriiObj[i][x] = ObjObject3d::Create();
			toriiObj[i][x]->Initialize();
			toriiObj[i][x]->SetModel(toriiModel.get());
			toriiObj[i][x]->SetScale(XMFLOAT3(4, 4, 4));
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, points[i]);
			pos.y = -5.f;

			toriiObj[i][x]->SetPosition(pos);
		}
	}

	// 壁
	wallModel.reset(ObjModel::LoadFromObj("fence"));// wall
	wallObj.resize(points.size());

	for (UINT i = 0; i < wallObj.size(); i++) {
		wallObj[i].resize(2);
		for (UINT x = 0; x < wallObj[i].size(); x++) {
			wallObj[i][x] = ObjObject3d::Create();
			wallObj[i][x]->Initialize();
			wallObj[i][x]->SetModel(wallModel.get());
			wallObj[i][x]->SetScale(XMFLOAT3(4, 4, 1));
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, points[i]);
			pos.x -= 40.f;
			pos.x += 80.f * (float)x;
			pos.y = -5.f;
			wallObj[i][x]->SetPosition(pos);
		}
	}

#pragma region 敵CSV

	// CSV読み込み
	csv = Enemy::LoadCsv("Resources/enemy.csv");

	// todo 関数化する
	{
		UINT nowframe = 0;

		for (auto& y : csv) {
			if (y[0].find("//") == 0) {
				continue;
			}
			if (y[0] == "POPPOSITION") {
				// 敵を出す
				popEnemyPos.emplace_back(std::stof(y[1]), std::stof(y[2]), std::stof(y[3]));
			} else if (y[0] == "POPTIME") {
				// 指定時間
				nowframe += std::stoul(y[1]);
				enemyFrame.emplace_back(nowframe, nowframe + 60 * 39);// (敵が出る時間,敵が消える時間)
			}
		}
	}

#pragma endregion 敵CSV

	// プレイヤーの照準を非表示
	aim->SetIsInvisible(true);
}

void GamePlayScene::Finalize() {
	DamageEffect(1, 1);

	// マウスカーソルを出す
	ShowCursor(true);
}

void GamePlayScene::Update() {
	if (input->TriggerKey(DIK_ESCAPE)) {
		bool invisibleFlag = operationSprite["ESC_Pause"]->GetIsInvisible();
		operationSprite["ESC_Pause"]->SetIsInvisible(!invisibleFlag);
	}

	if (operationSprite["ESC_Pause"]->GetIsInvisible() == false) {
		// シーン遷移
		updateProcess();

		// パーティクル更新
		ParticleLoad::GetInstance()->Update();

		nowCamera->Update();
		lane->Update();

		player->Update();

		for (auto& i : enemy) {
			i->Update();
		}

		for (auto& i : toriiObj) {
			for (auto& x : i) {
				x->Update();
			}
		}

		for (auto& i : wallObj) {
			for (auto& x : i) {
				x->Update();
			}
		}

		groundObj->Update();
		skyDomeObj->Update();
		shrineObj->Update();
		bonfireR->Update();
		bonfireL->Update();

		// スプライト更新
		aim->SetPosition({ (float)input->GetMousePos().x,(float)input->GetMousePos().y,0 });
		aim->Update();

		if (player->GetHp() > 0) {

			// スプライト横幅 = 最大値 * hpの割合
			playerHpSprite->SetSize(XMFLOAT2(playerHpSpriteSize.x * (float)player->GetHp() / playerHpMax,
				playerHpSpriteSize.y));

			playerHpSprite->TransferVertexBuffer();

			// 体力バー
			playerHpSprite->Update();

			playerHpSlide->Update();

			for (auto& i : operationSprite) {
				i.second->Update();
			}
		}
	}// ポーズ画面
	else {
		pauseSprite[pause]->Update();
		if (input->TriggerKey(DIK_W)) {
			if (--pause <= -1) {
				pause = 2;
			}
		}
		if (input->TriggerKey(DIK_S)) {
			if (++pause >= 3) {
				pause = 0;
			}
		}

		if (input->TriggerKey(DIK_SPACE)) {
			if (pause == 0) {
				operationSprite["ESC_Pause"]->SetIsInvisible(false);
			} else if (pause == 1) {
				SceneManager::GetInstance()->ChangeScene("TITLE");
				// BGM止める
				Audio::GetInstance()->StopWave(bgmFileName);
			} else if (pause == 2) {
				Game::GetInstance()->SetEndRequest(true);
			}
		}
	}
}

// シーン遷移
void GamePlayScene::start() {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 50;

	// モザイクの時間が最大までいったら
	if (++mosaicFrame > mosaicFrameMax) {
		// モザイクの細かさをもとに戻す
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });

		// updateProcessにentryPlayer関数をセット
		updateProcess = std::bind(&GamePlayScene::entryPlayer, this);

		// ParticleLoad::GetInstance()->SetCamera(normalCamera.get());

		ParticleLoad::GetInstance()->SetCamera(camera.get());

		// モザイクのフレーム数をリセット
		mosaicFrame = 0;
	} else {
		// 経過フレーム数に応じてモザイクをかける
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void GamePlayScene::entryPlayer() {
	constexpr UINT frameMax = 180;

	if (++playerEntryFrame > frameMax) {
		// カメラを変更
		nowCamera = camera.get();
		ObjObject3d::SetCamera(camera.get());

		// updateProcessにplay関数をセット
		updateProcess = std::bind(&GamePlayScene::play, this);

		ParticleLoad::GetInstance()->SetCamera(camera.get());

		// カメラをレーンの位置にセット
		camera->SetTrackingTarget(player.get());

		// プレイヤー登場演出のフレーム数をリセット
		playerEntryFrame = 0;

		player->SetPosition(playerEntryEndPos);

		// プレイヤーの照準を表示
		aim->SetIsInvisible(false);

		// ESC以外の操作説明を表示に戻す
		for (auto& i : operationSprite) {
			if (i.first != "ESC_Pause") {
				i.second->SetIsInvisible(false);
			}
		}
	} else {
		// プレイヤーの位置を変更
		XMFLOAT3 playerPos = lerp(playerEntryStartPos, playerEntryEndPos, (float)playerEntryFrame / frameMax);
		player->SetPosition(playerPos);

		// カメラ注視点の位置を変更
		XMFLOAT3 targetPos = lane->GetPosition();
		targetPos.x += player->GetPosition().x;
		targetPos.y += player->GetPosition().y;
		targetPos.z += player->GetPosition().z;
		normalCamera->SetTarget(targetPos);

		// 篝火の炎パーティクル
		{
			constexpr XMFLOAT3 velocity = { 0.f,0.2f,0.f };
			constexpr XMFLOAT3 accel = {0.f,0.f,0.f};
			constexpr float startScale = {1.f};
			constexpr XMFLOAT3 startCol = {0.7f,0.7f,0.3f};
			constexpr XMFLOAT3 endCol = {1.f,0.f,0.f};

			// 篝火　右
			XMFLOAT3 bonfireRPos = bonfireR->GetWorldPos();
			bonfireRPos.y += 4.f;
			ParticleLoad::GetInstance()->SetRenderAdd(1, rand() % 20, bonfireRPos, velocity, accel,
				startScale, (float)rand() / RAND_MAX * 0.5f, startCol, endCol);

			// 篝火　左
			XMFLOAT3 bonfireLPos = bonfireL->GetWorldPos();
			bonfireLPos.y += 4.f;
			ParticleLoad::GetInstance()->SetRenderAdd(1, rand() % 20, bonfireLPos, velocity, accel,
				startScale, (float)rand() / RAND_MAX * 0.5f, startCol, endCol);

		}

#ifdef _DEBUG
		// ボスシーンに行く
		if (input->TriggerKey(DIK_F)) {
			updateProcess = std::bind(&GamePlayScene::end, this, "BOSSPLAY");
		}
#endif //_DEBUG
	}
}

void GamePlayScene::play() {
	skyDomeObj->SetPosition(lane->GetPosition());

#ifdef _DEBUG
	// ボスシーンに行く
	if (input->TriggerKey(DIK_F)) {
		updateProcess = std::bind(&GamePlayScene::end, this, "BOSSPLAY");
	}
#endif //_DEBUG

	// プレイヤーの移動と回避
	MovePlayer();

	// 左クリックの操作説明画像の座標を更新
	UpdateLClickOperationSpritePosition();

	// 敵を発生
	AddEnemyFromPopData();

	enemyFrame.remove_if(
		[&](std::pair<UINT, UINT>& i) {
			return i.first <= frame;
		}
	);

	// 敵が全部居なくなったら次のシーンに行く
	if (enemyFrame.empty() && enemy.empty()) {
		// 退場演出の位置を保存
		playerExitStartPos = player->GetPosition();

		playerExitEndPos = playerExitStartPos;
		playerExitEndPos.z += 100.f;

		// カメラを変更
		ObjObject3d::SetCamera(normalCamera.get());
		nowCamera = normalCamera.get();
		XMFLOAT3 pos = lane->GetPosition();
		pos.x += player->GetPosition().x;
		pos.y += player->GetPosition().y;
		pos.z += player->GetPosition().z;
		normalCamera->SetEye(pos);

		updateProcess = std::bind(&GamePlayScene::exitPlayer, this);
	}

	// 敵が範囲外になったら行動を変える
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
	if (!enemy.empty()) {
		// 照準と敵のスクリーン座標の当たり判定
		CollisionAimAndEnemyScreenPos();
	}

	// 敵と自機の弾の当たり判定
	CollisionEnemyAndPlayerBullet();

	// 自機と敵の弾の当たり判定
	CollisionPlayerAndEnemyBullet();

	// RGBずらしのフラグ管理
	if (shiftFlag) {
		// 自機のダメージエフェクト
		DamageEffect(shiftMaxFrame, shiftNowFrame);
		shiftNowFrame++;
		if (shiftMaxFrame < shiftNowFrame) {
			shiftFlag = false;
			shiftNowFrame = 0;
		}
	}

	// レールをスプライン曲線で移動
	{
		splineFrame++;
		float timeRate = (float)splineFrame / 120.f;
		if (timeRate >= 1.0f) {
			if (splineStartIndex < points.size() - 3) {
				splineStartIndex++;
				timeRate -= 1.0f;
				splineFrame = 0;
			} else {
				timeRate = 1.0f;
			}
		}
		// ベクターをフロートに変換
		XMFLOAT3 splineFloat;
		XMStoreFloat3(&splineFloat, SplinePosition(points, splineStartIndex, timeRate));
		lane->SetPosition(splineFloat);
	}

	frame++;
}

void GamePlayScene::exitPlayer() {
	constexpr UINT frameMax = 120;

	// 時間が来たら
	if (++playerExitFrame > frameMax) {
		updateProcess = std::bind(&GamePlayScene::end, this, "BOSSPLAY");

		// プレイヤー退場演出のフレーム数をリセット
		playerEntryFrame = 0;
	} else {
		float rate = (float)playerExitFrame / frameMax;

		// プレイヤーの今の位置
		XMFLOAT3 nowPos = lerp(playerExitStartPos, playerExitEndPos, rate);
		player->SetPosition(nowPos);

		// カメラの注視点
		nowPos.x += lane->GetPosition().x;
		nowPos.y += lane->GetPosition().y;
		nowPos.z += lane->GetPosition().z;
		normalCamera->SetTarget(nowPos);

		// プレイヤーをだんだん小さくする(遠近法)
		rate = 1.f - rate;
		player->SetScale({ rate, rate, rate });

		// プレイヤーの照準を表示
		aim->SetIsInvisible(true);
	}

}

void GamePlayScene::deathPlayer() {
	XMFLOAT3 rota = player->GetRotation();
	rota.z += 1.f;

	float scale = player->GetScale().z;
	scale -= 0.01f;

	if (rota.z > 90.f || scale < 0.f) {
		scale = 0.f;
		player->SetAlive(false);
		updateProcess = std::bind(&GamePlayScene::end, this, "GAMEOVER");
	}
	player->SetRotation(rota);
	player->SetScale({ scale, scale, scale });

	// 地面まで落ちる
	XMFLOAT3 pos = player->GetPosition();
	if (pos.y > -4.f) {
		pos.y -= 0.1f;
		player->SetPosition(pos);
	}
}

void GamePlayScene::end(const std::string& sceneName) {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 50;

	// モザイクの時間が最大までいったら
	if (++mosaicFrame > mosaicFrameMax) {
		// プレイヤーの照準を表示
		aim->SetIsInvisible(false);

		// BGM止める
		Audio::GetInstance()->StopWave(bgmFileName);
		// 指定のシーンへ
		SceneManager::GetInstance()->ChangeScene(sceneName);
	} else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		rate = 1 - rate;// 1から0
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void GamePlayScene::Draw(DirectXCommon* dxcommon) {
	// 3Dオブジェクト描画前処理
	ObjObject3d::PreDraw();

	for (auto& i : toriiObj) {
		for (auto& x : i) {
			x->Draw();
		}
	}

	for (auto& i : wallObj) {
		for (auto& x : i) {
			x->Draw();
		}
	}

	// 神社
	shrineObj->Draw();
	// 篝火
	bonfireR->Draw();
	bonfireL->Draw();

	// 地面
	groundObj->Draw();

	//スカイドームの描画
	skyDomeObj->Draw();

	// 敵の複数描画
	for (auto& i : enemy) {
		i->Draw();
	}
	// プレイヤーの描画
	player->Draw();
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	ObjObject3d::PostDraw();
	// パーティクル描画
	ParticleLoad::GetInstance()->Draw(dxcommon->GetCmdList());

	// スプライト共通コマンド
	SpriteCommon::GetInstance()->PreDraw();
}

void GamePlayScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();

	// 体力描画
	if (player->GetHp() > 0) {
		playerHpSprite->Draw();
	}
	playerHpSlide->Draw();

	// 操作説明描画
	for (auto& i : operationSprite) {
		i.second->Draw();
	}

	// 照準描画
	aim->Draw();

	if (operationSprite["ESC_Pause"]->GetIsInvisible()) {
		// ポーズ画面描画
		pauseSprite[pause]->Draw();
	}
}

XMVECTOR GamePlayScene::SplinePosition(const std::vector<XMVECTOR>& posints, size_t startIndex, float t) {
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

std::unique_ptr<Enemy>& GamePlayScene::AddEnemy(XMFLOAT3 pos, XMFLOAT3 vel) {
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

void GamePlayScene::UpdateLClickOperationSpritePosition() {
	if (operationSprite["L_Click"]->GetIsInvisible() == false) {
		operationSprite["L_Click"]->SetPosition({ aim->GetPosition().x, aim->GetPosition().y, 0.f });
	}
}

// プレイヤー移動
void GamePlayScene::MovePlayer() {

	{
		const bool hitW = input->PushKey(DIK_W);
		const bool hitS = input->PushKey(DIK_S);
		const bool hitA = input->PushKey(DIK_A);
		const bool hitD = input->PushKey(DIK_D);
		const bool hitZ = input->PushKey(DIK_Z);
		const bool hitX = input->PushKey(DIK_X);

		const bool hitSpace = input->TriggerKey(DIK_SPACE);

		if (avoidFrame >= 1) {
			avoidFrame--;
		}

		if (hitW || hitS || hitA || hitD || hitZ || hitX) {
			auto pos = player->GetPosition();
			float moveSpeed = 0.2f;
			XMFLOAT3 rot{};

			if (hitSpace && avoidFrame == 0) {
				moveSpeed *= 10;
				avoidFrame = avoidFrameMax;
			}

			if (hitW && pos.y < 8.f) {
				pos.y += moveSpeed;
				rot.x -= 0.5f;
				operationSprite["W"]->SetIsInvisible(true);

			}
			if (hitS && pos.y > -4.f) {
				pos.y -= moveSpeed;
				rot.x += 0.5f;
				operationSprite["S"]->SetIsInvisible(true);
			}

			if (hitD && pos.x < 10.f) {
				pos.x += moveSpeed;
				rot.z -= 1.f;
				operationSprite["D"]->SetIsInvisible(true);
			}
			if (hitA && pos.x > -10.f) {
				pos.x -= moveSpeed;
				rot.z += 1.f;
				operationSprite["A"]->SetIsInvisible(true);
			}

			player->SetPosition(pos);
			player->SetRotation(rot);
		}
	}
}

void GamePlayScene::AddEnemyFromPopData() {

	// 参照型変数「i」に
	// enemyFrameの中身を
	// 最初から最後まで
	// 順番に入れる
	for (auto& i : enemyFrame) {
		if (i.first <= frame) {// 敵の速度を設定
			auto& a = AddEnemy(popEnemyPos[addedEnemyNum], { 0.f, 0.f, -0.25f });
			addedEnemyNum++;
			a->SetLifeSpan(i.second);
		}
	}
}

// 照準と敵のスクリーン座標の当たり判定
void GamePlayScene::CollisionAimAndEnemyScreenPos() {

	// 画像の左上と右下
	XMFLOAT2 aimLT = { (float)input->GetMousePos().x - aim->GetSize().x / 2,
	(float)input->GetMousePos().y - aim->GetSize().y / 2 };

	XMFLOAT2 aimRB = { (float)input->GetMousePos().x + aim->GetSize().x / 2,
	(float)input->GetMousePos().y + aim->GetSize().y / 2 };

	// 敵の場所
	XMFLOAT2 enemyScreenPos;

	bool flag = false;

	// 照準と敵のスクリーン座標の当たり判定
	player->SetShotTarget(nullptr);
	for (auto& i : enemy) {
		if (!i->GetAlive())continue;
		enemyScreenPos = { i->GetFloat2ScreenPos().x,i->GetFloat2ScreenPos().y };

		// 当たり判定
		if (aimLT.x <= enemyScreenPos.x && aimLT.y <= enemyScreenPos.y &&
			aimRB.x >= enemyScreenPos.x && aimRB.y >= enemyScreenPos.y) {
			flag = true;

			player->SetShotTarget(i.get());
		}
	}

	++shotInterval;

	if (flag) {
		aim->SetColor({ 1,0,0,1 });
		// 照準が合っていた場合、左クリックしていたら一定間隔で弾を出す
		if (input->PushMouse(Input::LEFT)) {
			if (shotInterval >= shotIntervalMax) {
				shotInterval = 0;

				// 自機の弾の発射
				player->Shot(pBulletModel.get(), pBulletScale);

				// 操作説明を消す
				operationSprite["L_Click"]->SetIsInvisible(true);
			}
		}
	} else {
		// 照準が白
		aim->SetColor({ 1,1,1,1 });
	}
}

// 敵と自機の弾の当たり判定
void GamePlayScene::CollisionEnemyAndPlayerBullet() {
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
						ParticleLoad::GetInstance()->SetRenderParticle(0, pos, 100, 4, 10);

						// SEを再生
						Audio::GetInstance()->PlayWave("sound/Kagura_Suzu02-1.wav", 0.5f, 0);
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
					ParticleLoad::GetInstance()->SetRenderParticle(0, pos, 100, 4, 5, { 1,0,1 }, { 0.5f,0,0.5f });
					break;
				}
			}
		}
		// 敵を消す
		enemy.erase(std::remove_if(enemy.begin(), enemy.end(),
			[](const std::unique_ptr <Enemy>& i) {return !i->GetAlive() && i->GetBullet().empty(); }), enemy.end());
	}
}
// 自機と敵の弾の当たり判定
void GamePlayScene::CollisionPlayerAndEnemyBullet() {
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
						player->Damage(1);				// プレイヤーにダメージ
						shiftFlag = true;				// RGBずらしをする
						shiftNowFrame = 0;

						if (player->GetHp() == 0) {		// 体力が0になったら
							shiftFlag = false;	// RGBずらしはしない
							// プレイヤー死亡演出へ移動
							updateProcess = std::bind(&GamePlayScene::deathPlayer, this);
						}
						break;
					}
				}
			}
		}
	}
}
