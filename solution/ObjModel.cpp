#include "ObjModel.h"
#include <string>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <DirectXMath.h>
#include <DirectXTex.h>


using namespace DirectX;

ID3D12Device* ObjModel::device = nullptr;

ObjModel* ObjModel::LoadFromObj(const std::string& modelname)
{
	ObjModel* model = new ObjModel();

	// �f�X�N���v�^�q�[�v����
	model->InitializeDescriptorHeap();

	// OBJ�t�@�C������f�[�^��ǂݍ���
	model->LoadFromOBJInternal(modelname);

	// �o�b�t�@����
	model->CreateBuffers();

	return model;


}

void ObjModel::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	// �t�@�C���X�g���[��
	std::ifstream file;
	// �}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	// 1�s���ǂݍ���
	std::string line;
	while (getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		getline(line_stream, key, ' ');

		// �擪�̃^�u�����͖�������
		if (key[0] == '\t') {
			key.erase(key.begin()); // �擪�̕������폜
		}

		// �擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl") {
			// �}�e���A�����ǂݍ���
			line_stream >> material_.name;
		}
		// �擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka") {
			line_stream >> material_.ambient.x;
			line_stream >> material_.ambient.y;
			line_stream >> material_.ambient.z;
		}
		// �擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd") {
			line_stream >> material_.diffuse.x;
			line_stream >> material_.diffuse.y;
			line_stream >> material_.diffuse.z;
		}
		// �擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks") {
			line_stream >> material_.specular.x;
			line_stream >> material_.specular.y;
			line_stream >> material_.specular.z;
		}
		// �擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd") {
			// �e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> material_.textureFilename;
			// �e�N�X�`���ǂݍ���
			LoadTexture(directoryPath, material_.textureFilename);
		}
	}
	// �t�@�C�������
	file.close();
}

bool ObjModel::LoadTexture(const std::string& directoryPath, const std::string& filename)
{
	HRESULT result = S_FALSE;

	// WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// �t�@�C���p�X������
	std::string filepath = directoryPath + filename;
	wchar_t wfilepath[128];

	// wstring�ł̒�����
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));
	directoryPath + filename;

	result = LoadFromWICFile(
		wfilepath, WIC_FLAGS_NONE,
		&metadata, scratchImg);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	const Image* img = scratchImg.GetImage(0, 0, 0); // ���f�[�^���o

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	// �e�N�X�`���p�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // �e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(&texbuff_));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texbuff_->WriteToSubresource(
		0,
		nullptr, // �S�̈�փR�s�[
		img->pixels,    // ���f�[�^�A�h���X
		(UINT)img->rowPitch,  // 1���C���T�C�Y
		(UINT)img->slicePitch // 1���T�C�Y
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �V�F�[�_���\�[�X�r���[�쐬
	cpuDescHandleSRV_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap_->GetCPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);
	gpuDescHandleSRV_ = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap_->GetGPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // �ݒ�\����
	D3D12_RESOURCE_DESC resDesc = texbuff_->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texbuff_.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc, //�e�N�X�`���ݒ���
		cpuDescHandleSRV_
	);

	return true;
}

void ObjModel::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial)
{
	// ���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &vbView_);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView_);
	// �}�e���A���p�萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(rootParamIndexMaterial, constBuffB1_->GetGPUVirtualAddress());
	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	if (material_.textureFilename.size() > 0) {
		// �V�F�[�_���\�[�X�r���[���Z�b�g
		cmdList->SetGraphicsRootDescriptorTable(2, gpuDescHandleSRV_);
	}
	// �`��R�}���h
	cmdList->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);
}

void ObjModel::LoadFromOBJInternal(const std::string& modelname)
{
	const std::string filename = modelname + ".obj";
	const std::string directoryPath = "Resources/" + modelname + "/";

	HRESULT result;
	// �t�@�C���X�g���[��
	std::ifstream file;
	// .obj�t�@�C�����J��
	file.open(directoryPath + filename);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	std::vector<XMFLOAT3> positions;	// ���_���W
	std::vector<XMFLOAT3> normals;	// �@���x�N�g��
	std::vector<XMFLOAT2> texcoords;	// �e�N�X�`��UV
	// 1�s���ǂݍ���
	std::string line;
	while (getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		getline(line_stream, key, ' ');

		// �擪������v�Ȃ璸�_���W
		if (key == "v") {
			// X,Y,Z���W�ǂݍ���
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.emplace_back(position);

			//VertexPosNormalUv vertex{};
			//vertex.pos = position;
			//vertices.emplace_back(vertex);
		}
		// �擪������vt�Ȃ�e�N�X�`��
		if (key == "vt")
		{
			// U,V�����ǂݍ���
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			// V�������]
			texcoord.y = 1.0f - texcoord.y;
			// �e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);
		}
		// �擪������vn�Ȃ�@���x�N�g��
		if (key == "vn") {
			// X,Y,Z�����ǂݍ���
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			// �@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}
		// �擪������f�Ȃ�|���S���i�O�p�`�j
		if (key == "f")
		{
			// ���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			std::string index_string;
			while (getline(line_stream, index_string, ' ')) {
				// ���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, std::ios_base::cur); // �X���b�V�����΂�
				index_stream >> indexTexcoord;
				index_stream.seekg(1, std::ios_base::cur); // �X���b�V�����΂�
				index_stream >> indexNormal;
				// ���_�f�[�^�̒ǉ�
				VertexPosNormalUv vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				vertices_.emplace_back(vertex);
				// �C���f�b�N�X�f�[�^�̒ǉ�
				indices_.emplace_back((unsigned short)indices_.size());
			}
		}
		//�}�e���A��
		if (key == "mtllib")
		{
			// �}�e���A���̃t�@�C�����ǂݍ���
			std::string filename;
			line_stream >> filename;
			// �}�e���A���ǂݍ���
			LoadMaterial(directoryPath, filename);
		}
	}
	file.close();
}

void ObjModel::InitializeDescriptorHeap()
{
	assert(device);

	HRESULT result = S_FALSE;

	// �f�X�N���v�^�q�[�v�𐶐�	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	descHeapDesc.NumDescriptors = 1; // �V�F�[�_�[���\�[�X�r���[1��
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap_));//����
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// �f�X�N���v�^�T�C�Y���擾
	descriptorHandleIncrementSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ObjModel::CreateBuffers()
{
	HRESULT result = S_FALSE;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices_.size());
	// ���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());
	// �C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff_));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);
		indexBuff_->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;

	// �}�e���A���p�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1_));
	assert(SUCCEEDED(result));

	// �}�e���A���p�萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataB1* constMap1 = nullptr;
	result = constBuffB1_->Map(0, nullptr, (void**)&constMap1);
	assert(SUCCEEDED(result));
	constMap1->ambient = material_.ambient;
	constMap1->diffuse = material_.diffuse;
	constMap1->specular = material_.specular;
	constMap1->alpha = material_.alpha;
	constBuffB1_->Unmap(0, nullptr);
}
