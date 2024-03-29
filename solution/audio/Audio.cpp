#include "Audio.h"
#include <windows.h>
#include <cassert>
#include <fstream>

#pragma comment(lib,"xaudio2.lib")

Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}

void Audio::Initialize(const std::string& directoryPath) {
	this->directoryPath = directoryPath;

	HRESULT result;
	IXAudio2MasteringVoice* masterVoice;

	// XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
}

void Audio::Finalize() {
	// XAudio2解放
	xAudio2.Reset();
	// 音声データ解放
	std::map<std::string, SoundData>::iterator it = soundDatas.begin();


	for (; it != soundDatas.end(); ++it) {

		Unload(&it->second);
	}
	soundDatas.clear();
}

void Audio::LoadWave(const std::string filename) {
	if (soundDatas.find(filename) != soundDatas.end()) {
		// 重複読み込みなので、何もせず抜ける
		return;
	}
	// ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullpath = directoryPath + filename;

	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(fullpath, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK ", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data ", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();

	// returnする為の音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	HRESULT result = xAudio2->CreateSourceVoice(&soundData.pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	// サウンドデータを連想配列に格納
	soundDatas.insert(std::make_pair(filename, soundData));
}

void Audio::Unload(SoundData* soundData) {
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::PlayWave(const std::string filename, float volume, UINT loopCount) {
	HRESULT result;

	// 該当するイテレータを取得
	std::map<std::string, SoundData>::iterator it = soundDatas.find(filename);
	// 未読み込みの検出
	// (読み込み済みのみ許可する)
	assert(it != soundDatas.end());
	// イテレータからサウンドデータの参照を取得
	SoundData& soundData = it->second;

	// 波形フォーマットを元にSourceVoiceの生成
	result = xAudio2->CreateSourceVoice(&soundData.pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopCount;

	// 波形データの再生
	result = soundData.pSourceVoice->SubmitSourceBuffer(&buf);
	result = soundData.pSourceVoice->SetVolume(volume);
	result = soundData.pSourceVoice->Start();
}

void Audio::StopWave(const std::string filename) {
	// 該当するイテレータを取得
	std::map<std::string, SoundData>::iterator it = soundDatas.find(filename);
	// 未読み込みの検出
	// (読み込み済みのみ許可する)
	assert(it != soundDatas.end());
	// イテレータからサウンドデータの参照を取得
	SoundData& soundData = it->second;

	HRESULT result = soundData.pSourceVoice->Stop();
	assert(SUCCEEDED(result));

	result = soundData.pSourceVoice->FlushSourceBuffers();
	assert(SUCCEEDED(result));
}
