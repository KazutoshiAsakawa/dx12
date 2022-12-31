#pragma once

#include <xaudio2.h>
#include <cstdint>
#include <wrl.h>
// �A�z�z��
#include <map>
#include <string>

/// <summary>
/// �I�[�f�B�I
/// </summary>
class Audio
{
public:
	// �`�����N�w�b�_
	struct ChunkHeader
	{
		char id[4]; // �`�����N����ID
		int32_t size;  // �`�����N�T�C�Y
	};

	// RIFF�w�b�_�`�����N
	struct RiffHeader
	{
		ChunkHeader chunk;   // "RIFF"
		char type[4]; // "WAVE"
	};

	// FMT�`�����N
	struct FormatChunk
	{
		ChunkHeader chunk; // "fmt "
		WAVEFORMATEX fmt; // �g�`�t�H�[�}�b�g
	};

	// �����f�[�^
	struct SoundData
	{
		// �g�`�t�H�[�}�b�g
		WAVEFORMATEX wfex;
		// �o�b�t�@�̐擪�A�h���X
		BYTE* pBuffer;
		// �o�b�t�@�̃T�C�Y
		unsigned int bufferSize;

		IXAudio2SourceVoice* pSourceVoice = nullptr;
	};

	static Audio* GetInstance();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="">�t�@�C���p�X(Resources/�ɑ���)</param>
	void Initialize(const std::string& directoryPath = "Resources/");

	/// <summary>
	/// �I������
	/// </summary>
	void Finalize();

	/// <summary>
	/// WAV�����ǂݍ���
	/// </summary>
	/// <param name="filename">WAV�t�@�C����</param>
	void LoadWave(const std::string filename);

	/// <summary>
	/// �T�E���h�f�[�^�̉��
	/// </summary>
	/// <param name="soundData">�T�E���h�f�[�^</param>
	void Unload(SoundData* soundData);

	/// <summary>
	/// �����Đ�
	/// </summary>
	/// <param name="">�T�E���h�f�[�^��</param>
	void PlayWave(const std::string filename,float volume = 0.5);

	/// <summary>
	/// ������~
	/// </summary>
	/// <param name="soundData">�T�E���h�f�[�^</param>
	void StopWave(SoundData* soundData);

	/// <summary>
	/// �����f�[�^���擾
	/// </summary>
	/// <param name="filename">�T�E���h�f�[�^��</param>
	/// <returns>�����f�[�^</returns>
	inline SoundData& GetSoundData(const std::string& filename){
		return soundDatas[filename];
	}

private:
	// XAudio2�̃C���X�^���X
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	// �T�E���h�f�[�^�̘A�z�z��
	std::map<std::string,SoundData> soundDatas;
	// �T�E���h�i�[�f�B���N�g��
	std::string directoryPath;
};

