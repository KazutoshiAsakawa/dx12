#pragma once

// �����蔻��v���~�e�B�u

#include <DirectXMath.h>

// ��
struct Sphere {
	// ���S���W
	DirectX::XMVECTOR center = { 0, 0, 0, 1 };
	// ���a
	float radius = 1.f;
};

// ����
struct Plane {
	// �@���x�N�g��
	DirectX::XMVECTOR normal = { 0, 1, 0, 0 };
	// ���_(0, 0, 0)����̋���
	float distance = 0.f;
};

// �@���t���O�p�`(���v��肪�\)
class Triangle {
public:
	//���_���3��
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;
	//�@���x�N�g��
	DirectX::XMVECTOR normal;

	// �@���̌v�Z
	void ComputeNormal();
};

// ���C(������)
struct Ray {
	// �n�_
	DirectX::XMVECTOR start = { 0, 0, 0, 1 };
	// ����
	DirectX::XMVECTOR dir = { 1, 0, 0, 0 };
};