#include "BossScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"

#include "FbxObject3d.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"
#include "Collision.h"
#include "ParticleLoad.h"

#include "PostEffect.h"

using namespace DirectX;

XMFLOAT3 lerp(const XMFLOAT3& a, const XMFLOAT3& b, float t) {
	XMFLOAT3 ret;
	ret.x = a.x + t * (b.x - a.x);
	ret.y = a.y + t * (b.y - a.y);
	ret.z = a.z + t * (b.z - a.z);
	return ret;
}

void BossScene::Initialize(DirectXCommon* dxcommon) {
	
	input = Input::GetInstance();

	// マウスカーソルを消す
	ShowCursor(false);

	// スプライトの生成
	aim.reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/aim.png")));
	aim->SetPosition({ WinApp::window_width / 2.f ,WinApp::window_height / 2.f, 0.f });

	operationSprite["ESC_Pause"].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/operation/ESC_Pause.png"), { 0.f, 0.f }, false, false));
	// 最初は表示する
	operationSprite["ESC_Pause"]->SetIsInvisible(false);

	// ポーズ画面の画像を作る
	pauseSprite.resize(pauseMax);
	pauseSprite[0].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/pause/pauseBack.png"), { 0.f, 0.f }));
	pauseSprite[1].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/pause/pauseTitle.png"), { 0.f, 0.f }));
	pauseSprite[2].reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/pause/pauseClose.png"), { 0.f, 0.f }));

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
	groundModel->SetTiling({ 5.f,5.f });
	groundObj = ObjObject3d::Create();
	groundObj->SetModel(groundModel.get());
	groundObj->SetScale({ 100,100,100 });
	groundObj->SetPosition({ 0,-5,0 });

	// 自機の読み込み
	pBulletModel.reset(ObjModel::LoadFromObj("bullet"));
	// 敵の読み込み
	enemyModel.reset(ObjModel::LoadFromObj("obake"));

	//デバイスをセット
	FbxObject3d::SetDevice(dxcommon->GetDev());
	//カメラをセット
	FbxObject3d::SetCamera(camera.get());
	//グラフィックスパイプライン生成
	FbxObject3d::CreateGraphicsPipeline();

#pragma region プレイヤー
	// プレイヤー初期化
	player = std::make_unique<Player>();

	player->SetHp(playerHpMax);

	// hp画像
	playerHpSpriteSize = { 330.f, 30.f };
	playerHpSprite.reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/hp/playerHp.png"), { 0,1 }));
	playerHpSprite->SetPosition(XMFLOAT3(35, WinApp::window_height - 35, 0));
	playerHpSprite->SetSize({ 0.f,playerHpSpriteSize.y });
	playerHpSlide.reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/hp/hpSlide.png"), { 0,1 }));
	playerHpSlide->SetPosition(XMFLOAT3(35, WinApp::window_height - 35, 0));
	playerHpSlide->SetSize(playerHpSpriteSize);

	playerHpSprite->TransferVertexBuffer();
	playerHpSprite->Update();
	playerHpSlide->TransferVertexBuffer();
	playerHpSlide->Update();

#pragma endregion プレイヤー

#pragma region カメラ

	// カメラをプレイヤーの位置にセット
	camera->SetTrackingTarget(player.get());
	camera->SetTarget(player->GetPosition());
	XMFLOAT3 eye = player->GetPosition();
	eye.z -= 500;
	eye.y += 50;
	camera->SetEye(eye);
	cameraLengthDef = camera->GetEyeToCameraTargetLength();

#pragma endregion カメラ

#pragma region ボス

	// ボス
	boss.reset(new Boss(enemyModel.get(), { 0.f, 0.f, 50.f }));

	boss->SetPhaseApproach();
	boss->SetAttackTarget(player.get());
	boss->SetBulletModel(ObjModel::LoadFromObj("enemyBullet"));
	bossScale = 2.f;
	boss->SetScale({ bossScale,bossScale,bossScale });
	boss->SetPosition({0,10,60});


	boss->SetAlive(false);

	bossEntryNowFrame = 0;

#pragma region ボスHP

	// ボスの体力
	boss->SetHp(bossHpMax);

	// ボスHPスプライト
	bossHpSpriteSize = { 430.f, 30.f };
	bossHpSprite.reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/hp/bossHp.png"), { 0.f, 0.f }));
	bossHpSlide.reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/hp/hpSlide.png"), { 0.f, 0.f }));
	
	bossHpSprite->SetPosition({ (float)WinApp::window_width / 2.f - bossHpSpriteSize.x / 2, 5.f, 0.f });
	bossHpSlide->SetPosition({ (float)WinApp::window_width / 2.f - bossHpSpriteSize.x / 2, 5.f, 0.f });

	bossHpSprite->SetSize({ 0.f, bossHpSpriteSize.y });
	bossHpSlide->SetSize(bossHpSpriteSize);

	bossHpSprite->TransferVertexBuffer();
	bossHpSlide->TransferVertexBuffer();

	bossHpSprite->Update();
	bossHpSlide->Update();

#pragma endregion ボスHP

#pragma region ボス名前

	// ボスの名前スプライト
	bossText.reset(Sprite::Create(SpriteCommon::GetInstance()->LoadTexture(L"Resources/bossText.png"), { 0.5f, 0.5f }));
	bossText->SetPosition({ (float)WinApp::window_width / 2.f, (float)WinApp::window_height / 4.f * 3.f, 0.f });

	constexpr float texSizeX = WinApp::window_width * (3.f / 4.f);	// 画像横幅は画面横幅の3/4
	float texRate = bossText->GetTexSize().y / bossText->GetTexSize().x;	// 縦は横の何倍か
	bossText->SetSize({ texSizeX, texSizeX * texRate });

	bossText->SetIsInvisible(true);

	bossText->TransferVertexBuffer();
	bossText->Update();

#pragma endregion ボス名前

#pragma endregion ボス

	// パーティクル初期化
	ParticleLoad::GetInstance()->SetCamera(camera.get());

	// 更新処理の関数を入れる
	updateProcess = std::bind(&BossScene::start, this);

#pragma region 音

	// Resources/に続くファイルパス
	bgmFileName = "sound/zensen he totugekiseyo.wav";

	// BGM読み込み
	Audio::GetInstance()->LoadWave(bgmFileName);
	// BGMを再生
	Audio::GetInstance()->PlayWave(bgmFileName, 0.06f, XAUDIO2_LOOP_INFINITE);

#pragma endregion 音

}

void BossScene::Finalize() {
	DamageEffect(1, 1);

	ParticleLoad::GetInstance()->ClearParticle();

	// マウスカーソルを出す
	ShowCursor(true);
}

void BossScene::Update() {
	if (input->TriggerKey(DIK_ESCAPE)) {
		bool invisibleFlag = operationSprite["ESC_Pause"]->GetIsInvisible();
		operationSprite["ESC_Pause"]->SetIsInvisible(!invisibleFlag);
	}

	if (operationSprite["ESC_Pause"]->GetIsInvisible() == false) {
		// マウスの固定
		{
			POINT oldMousePos = input->GetMousePos();

			input->SetMousePos({ long(WinApp::window_width / 2.f),long(WinApp::window_height / 2.f) });
			// 前座標との差分
			mousePosDiff = input->GetMousePos();
			mousePosDiff.x -= oldMousePos.x;
			mousePosDiff.y -= oldMousePos.y;
		}

		// シーン遷移
		updateProcess();

		// パーティクル更新
		ParticleLoad::GetInstance()->Update();

		camera->Update();

		player->Update();

		boss->Update();

		groundObj->Update();
		skyDomeObj->Update();

		// スプライト更新
		aim->Update();
		operationSprite["ESC_Pause"]->Update();
	} else {
		pauseSprite[pause]->Update();
		if (input->TriggerKey(DIK_W ) || input->TriggerKey(DIK_UP)) {
			if (--pause <= -1) {
				pause = 2;
			}
		}
		if (input->TriggerKey(DIK_S) || input->TriggerKey(DIK_DOWN)) {
			if (++pause >= 3) {
				pause = 0;
			}
		}

		if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN)) {
			if (pause == 0) {
				operationSprite["ESC_Pause"]->SetIsInvisible(false);
			} else if (pause == 1) {
				SceneManager::GetInstance()->ChangeScene("TITLE");
				// BGM止める
				Audio::GetInstance()->StopWave(bgmFileName);
			} else if (pause == 2) {
				PostQuitMessage(0); //OSに対して、アプリの終了を伝える
			}
		}
	}
}

// シーン遷移
void BossScene::start() {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 120;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		PostEffect::GetInstance()->SetMosaicNum({ WinApp::window_width ,WinApp::window_height });
		// updateProcessにbossEntry関数をセット
		camera->SetTrackingTarget(boss.get());
		updateProcess = std::bind(&BossScene::bossEntry, this);
		mosaicFrame = 0;
		bossText->SetIsInvisible(false);
	} else {
		// モザイクがだんだん薄くなる演出
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);

		// プレイヤーが遠くから来る演出
		constexpr XMFLOAT3 startPos = { 0,10,-90 };
		constexpr XMFLOAT3 endPos = { 0,10,10 };

		XMFLOAT3 pos;
		pos = lerp(startPos, endPos, rate);
		player->SetPosition(pos);

		// 体力バーをだんだん増やす
		playerHpSprite->SetSize({ playerHpSpriteSize.x * (1.f - powf(1.f - rate, 4.f)),
			playerHpSprite->GetSize().y });
		playerHpSprite->TransferVertexBuffer();
		playerHpSprite->Update();

	}
}

void BossScene::bossEntry() {
	constexpr UINT frameMax = 360;

	// フレームが最大まで行ったら次の関数へ
	if (bossEntryNowFrame++ > frameMax) {
		camera->SetTrackingTarget(player.get());
		updateProcess = std::bind(&BossScene::play, this);

		camera->SetEyeToCameraTargetLength(cameraLengthDef);

		boss->SetAlive(true);

		playerHpSprite->SetSize(playerHpSpriteSize);
		playerHpSprite->TransferVertexBuffer();
		playerHpSprite->Update();

		bossHpSprite->SetSize(bossHpSpriteSize);
		bossHpSprite->TransferVertexBuffer();
		bossHpSprite->Update();

		// ボスの名前非表示
		bossText->SetIsInvisible(true);
		// プレイヤーの照準を表示
		aim->SetIsInvisible(false);
	} else {
		// 進行度
		float rate = (float)bossEntryNowFrame / (float)frameMax;

		bossHpSprite->SetSize({ bossHpSpriteSize.x * (1.f - powf(1.f - rate, 4.f)),
			bossHpSprite->GetSize().y });
		bossHpSprite->TransferVertexBuffer();
		bossHpSprite->Update();


		// イージング(4乗)
		rate *= rate * rate * rate;

		// カメラの距離の最大値
		const float lengthMax = cameraLengthDef * 2.f;

		// 今のカメラの距離
		float cameraLength = cameraLengthDef + rate * (lengthMax - cameraLengthDef);

		// 今のカメラの距離をセット
		camera->SetEyeToCameraTargetLength(cameraLength);

		// ボスの名前
		bossText->SetColor({ 1.f, 1.f, 1.f, 1.f - (rate * rate * rate * rate) });

		bossText->Update();

		// プレイヤーの照準を表示
		aim->SetIsInvisible(true);
	}
}

void BossScene::play() {

	// スカイドームの位置
	skyDomeObj->SetPosition(player->GetPosition());

	// プレイヤーの移動と回避
	PlayerMove();

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

	if (boss->GetAlive()) {
		// 照準と敵のスクリーン座標の当たり判定
		CollisionAimAndEnemyScreenPos();
	} else if (killBossFlag) {// ボスが死んだら
		updateProcess = std::bind(&BossScene::end, this, "CLEAR");
	}

	// 敵と自機の弾の当たり判定
	CollisionEnemyAndPlayerBullet();

	// 自機と敵の弾の当たり判定
	CollisionPlayerAndEnemyBullet();

	// RGBずらしのフラグ管理
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

	if (player->GetHp() > 0) {

		// スプライト横幅 = 最大値 * hpの割合
		playerHpSprite->SetSize(XMFLOAT2(playerHpSpriteSize.x * (float)player->GetHp() / playerHpMax,
			playerHpSprite->GetSize().y));

		playerHpSprite->TransferVertexBuffer();

		// 体力バー
		playerHpSprite->Update();

		playerHpSlide->Update();
	}
	if (boss->GetHp() > 0) {

		// スプライト横幅 = 最大値 * hpの割合
		bossHpSprite->SetSize(XMFLOAT2(bossHpSpriteSize.x * (float)boss->GetHp() / bossHpMax,
			bossHpSprite->GetSize().y));

		bossHpSprite->TransferVertexBuffer();
		bossHpSprite->Update();
		bossHpSlide->Update();
	}

	frame++;
}

void BossScene::killEffect() {
	constexpr UINT effectFrameMax = 180;

	if (++nowEffectFrame > effectFrameMax) {
		updateProcess = std::bind(&BossScene::end, this, "CLEAR");


	} else {
		// プレイヤーの照準を非表示
		aim->SetIsInvisible(true);

		// パーティクルの発生
		ParticleLoad::GetInstance()->SetRenderParticle(0, boss->GetPosition(), 10, 1, 5);

		// 進行度
		float rate = (float)nowEffectFrame / (float)effectFrameMax;

		// イージング(8乗)
		rate *= rate * rate * rate * rate * rate * rate * rate;

		// 1から0にする
		rate = 1.f - rate;

		// イージングを反映させたボスの大きさ
		float scale = bossScale * rate;

		// 大きさをセット
		boss->SetScale({ scale, scale, scale });
	}
}

void BossScene::deathPlayer() {
	XMFLOAT3 rota = player->GetRotation();
	rota.z += 1.f;

	float scale = player->GetScale().z;
	scale -= 0.01f;

	if (rota.z > 90.f || scale < 0.f) {
		scale = 0.f;
		player->SetAlive(false);
		updateProcess = std::bind(&BossScene::end, this, "GAMEOVER");
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

void BossScene::end(const std::string& nextScene) {
	// モザイクをかける時間
	constexpr UINT mosaicFrameMax = 50;

	// モザイクの時間が最大までいったらplay関数に変える
	if (++mosaicFrame > mosaicFrameMax) {
		// プレイヤーの照準を表示
		aim->SetIsInvisible(false);
		// BGM止める
		Audio::GetInstance()->StopWave(bgmFileName);
		// 指定のエンド画面へ
		SceneManager::GetInstance()->ChangeScene(nextScene);
	} else {
		XMFLOAT2 mosaicLevel = {};
		float rate = (float)mosaicFrame / mosaicFrameMax;
		rate = 1 - rate;// 1から0
		rate *= rate;
		mosaicLevel.x = WinApp::window_width * rate;
		mosaicLevel.y = WinApp::window_height * rate;
		PostEffect::GetInstance()->SetMosaicNum(mosaicLevel);
	}
}

void BossScene::Draw(DirectXCommon* dxcommon) {
	// 3Dオブジェクト描画前処理
	ObjObject3d::PreDraw();

	// 地面
	groundObj->Draw();

	//スカイドームの描画
	skyDomeObj->Draw();

	// ボスの描画
	boss->Draw();

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

void BossScene::DrawFrontSprite(DirectXCommon* dxcommon) {
	SpriteCommon::GetInstance()->PreDraw();

	operationSprite["ESC_Pause"]->Draw();

	// 体力
	if (player->GetHp() > 0) {
		playerHpSprite->Draw();
	}
	playerHpSlide->Draw();

	// ボス体力
	if (boss->GetHp() > 0) {
		bossHpSprite->Draw();
	}
	bossHpSlide->Draw();

	bossText->Draw();

	aim->Draw();

	if (operationSprite["ESC_Pause"]->GetIsInvisible()) {
		pauseSprite[pause]->Draw();
	}
}

XMVECTOR BossScene::SplinePosition(const std::vector<XMVECTOR>& posints, size_t startIndex, float t) {
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

// プレイヤーの移動と回避
void BossScene::PlayerMove() {
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

		if (hitW || hitS || hitA || hitD || hitZ || hitX || hitSpace) {
			auto pos = player->GetPosition();
			float moveSpeed = 0.2f;

			// 回避
			if (hitSpace && avoidFrame == 0) {
				moveSpeed *= 10;
				// クールタイム
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
			if (hitS) {
				pos.x -= forward.x;
				pos.y -= forward.y;
				pos.z -= forward.z;
			}
			if (hitD) {
				pos.x += right.x;
				pos.y += right.y;
				pos.z += right.z;
			}
			if (hitA) {
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
}

// 照準と敵のスクリーン座標の当たり判定
void BossScene::CollisionAimAndEnemyScreenPos() {

	// 画像の左上と右下
	XMFLOAT2 aimLT = { aim->GetPosition().x - aim->GetSize().x / 2,
	 aim->GetPosition().y - aim->GetSize().y / 2 };

	XMFLOAT2 aimRB = { aim->GetPosition().x + aim->GetSize().x / 2,
	 aim->GetPosition().y + aim->GetSize().y / 2 };

	// 敵の場所
	XMFLOAT2 enemyScreenPos;

	bool flag = false;

	// 照準と敵のスクリーン座標の当たり判定
	player->SetShotTarget(nullptr);

	enemyScreenPos = { boss->GetFloat2ScreenPos().x,boss->GetFloat2ScreenPos().y };

	// 当たり判定
	if (aimLT.x <= enemyScreenPos.x && aimLT.y <= enemyScreenPos.y &&
		aimRB.x >= enemyScreenPos.x && aimRB.y >= enemyScreenPos.y) {
		flag = true;
		player->SetShotTarget(boss.get());
	}

	if (flag) {
		aim->SetColor({ 1,0,0,1 });
	} else {
		aim->SetColor({ 1,1,1,1 });
	}

	++shotInterval;

	// 左クリックしていたら
	if (input->PushMouse(Input::LEFT)) {
		// 一定間隔で弾を出す
		if (shotInterval >= shotIntervalMax) {
			shotInterval = 0;
			// 自機の弾の発射
			player->Shot(pBulletModel.get(), pBulletScale);
		}

	}
}

// 敵と自機の弾の当たり判定
void BossScene::CollisionEnemyAndPlayerBullet() {
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

				if (boss->GetHp() == 0) {
					boss->SetAlive(false);
					killBossFlag = true;

					camera->SetTrackingTarget(boss.get());
					updateProcess = std::bind(&BossScene::killEffect, this);
					XMFLOAT3 angle = camera->GetAngle();
					angle.y += 180.f;
					camera->SetAngle(angle);
				}

				// パーティクルの発生
				ParticleLoad::GetInstance()->SetRenderParticle(0, boss->GetPosition(), 10, 4, 5, { 1,0,0 }, { 0.5f,0,0 });
			}
		}
	}
}

// 自機と敵の弾の当たり判定
void BossScene::CollisionPlayerAndEnemyBullet() {
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
				// 敵弾のZ軸の大きさ
				eBulletShape.radius = eb.GetScale().z;

				// 当たったら消える
				if (Collision::CheckSphere2Sphere(playerShape, eBulletShape)) {
					eb.SetAlive(false);				// 敵の弾を消す
					player->Damage(1);				// プレイヤーにダメージ
					shiftFlag = true;				// RGBずらしをする
					nowFrame = 0;

					if (player->GetHp() == 0) {		// 体力が0になったら
						shiftFlag = false;	// RGBずらしはしない
						// プレイヤー死亡演出へ移動
						updateProcess = std::bind(&BossScene::deathPlayer, this);
					}
				}
			}
		}
	}
}
