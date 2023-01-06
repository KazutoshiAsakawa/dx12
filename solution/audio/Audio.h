#pragma once

#include <xaudio2.h>
#include <cstdint>
#include <wrl.h>
// 連想配列
#include <map>
#include <string>

/// <summary>
/// オーディオ
/// </summary>
class Audio {
public:
	// チャンクヘッダ
	struct ChunkHeader {
		char id[4]; // チャンク毎のID
		int32_t size;  // チャンクサイズ
	};

	// RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk;   // "RIFF"
		char type[4]; // "WAVE"
	};

	// FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk; // "fmt "
		WAVEFORMATEX fmt; // 波形フォーマット
	};

	// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE* pBuffer;
		// バッファのサイズ
		unsigned int bufferSize;

		IXAudio2SourceVoice* pSourceVoice = nullptr;
	};

	static Audio* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="">ファイルパス(Resources/に続く)</param>
	void Initialize(const std::string& directoryPath = "Resources/");

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// WAV音声読み込み
	/// </summary>
	/// <param name="filename">WAVファイル名</param>
	void LoadWave(const std::string filename);

	

	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="filename">サウンドデータ名</param>
	/// <param name="volume">音量</param>
	/// <param name="loopCount">ループ回数。XAUDIO2_LOOP_INFINITE(255)で無限ループ</param>
	void PlayWave(const std::string filename, float volume = 0.5f, UINT loopCount = XAUDIO2_LOOP_INFINITE);

	/// <summary>
	/// 音声停止
	/// </summary>
	/// <param name="soundData">サウンドデータ</param>
	void StopWave(const std::string filename);

	/// <summary>
	/// 音声データを取得
	/// </summary>
	/// <param name="filename">サウンドデータ名</param>
	/// <returns>音声データ</returns>
	inline SoundData& GetSoundData(const std::string& filename) {
		return soundDatas[filename];
	}

private:

	/// <summary>
	/// サウンドデータの解放
	/// </summary>
	/// <param name="soundData">サウンドデータ</param>
	void Unload(SoundData* soundData);

	// XAudio2のインスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	// サウンドデータの連想配列
	std::map<std::string, SoundData> soundDatas;
	// サウンド格納ディレクトリ
	std::string directoryPath;
};

