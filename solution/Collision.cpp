#include "Collision.h"

using namespace DirectX;

bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter) {
	//���W�n�̌��_���狅�̒��S�ւ̍��W
	const XMVECTOR distV = XMVector3Dot(sphere.center, plane.normal);
	//���ʂ̌��_�������v�Z����ƕ��ʂƋ��̒��S�Ƃ̋������o��
	const float dist = distV.m128_f32[0] - plane.distance;
	//�����̐�Βl�����a���傫����Γ������Ă��Ȃ�
	if (fabsf(dist) > sphere.radius) return false;

	//�^����_���v�Z
	if (inter) {
		//���ʏ�̍ŋߐړ_���^����_�Ƃ���
		*inter = -dist * plane.normal + sphere.center;
	}

	return true;
}

void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point,
	const Triangle& triangle, DirectX::XMVECTOR* closest) {
	// point��p0�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	const XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	const XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	const XMVECTOR p0_pt = point - triangle.p0;

	const XMVECTOR d1 = XMVector3Dot(p0_p1, p0_pt);
	const XMVECTOR d2 = XMVector3Dot(p0_p2, p0_pt);

	if (d1.m128_f32[0] <= 0.0f && d2.m128_f32[0] <= 0.0f) {
		// p0���ŋߖT
		*closest = triangle.p0;
		return;
	}

	// point��p1�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	const XMVECTOR p1_pt = point - triangle.p1;

	const XMVECTOR d3 = XMVector3Dot(p0_p1, p1_pt);
	const XMVECTOR d4 = XMVector3Dot(p0_p2, p1_pt);

	if (d3.m128_f32[0] >= 0.0f && d4.m128_f32[0] <= d3.m128_f32[0]) {
		// p1���ŋߖT
		*closest = triangle.p1;
		return;
	}

	// point��p0_p1�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p1��ɑ΂���ˉe��Ԃ�
	const float vc = d1.m128_f32[0] * d4.m128_f32[0] - d3.m128_f32[0] * d2.m128_f32[0];
	if (vc <= 0.0f && d1.m128_f32[0] >= 0.0f && d3.m128_f32[0] <= 0.0f) {
		float v = d1.m128_f32[0] / (d1.m128_f32[0] - d3.m128_f32[0]);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// point��p2�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	const XMVECTOR p2_pt = point - triangle.p2;

	const XMVECTOR d5 = XMVector3Dot(p0_p1, p2_pt);
	const XMVECTOR d6 = XMVector3Dot(p0_p2, p2_pt);
	if (d6.m128_f32[0] >= 0.0f && d5.m128_f32[0] <= d6.m128_f32[0]) {
		*closest = triangle.p2;
		return;
	}

	// point��p0_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p2��ɑ΂���ˉe��Ԃ�
	float vb = d5.m128_f32[0] * d2.m128_f32[0] - d1.m128_f32[0] * d6.m128_f32[0];
	if (vb <= 0.0f && d2.m128_f32[0] >= 0.0f && d6.m128_f32[0] <= 0.0f) {
		float w = d2.m128_f32[0] / (d2.m128_f32[0] - d6.m128_f32[0]);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// point��p1_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p1_p2��ɑ΂���ˉe��Ԃ�
	const float va = d3.m128_f32[0] * d6.m128_f32[0] - d5.m128_f32[0] * d4.m128_f32[0];
	if (va <= 0.0f && (d4.m128_f32[0] - d3.m128_f32[0]) >= 0.0f && (d5.m128_f32[0] - d6.m128_f32[0]) >= 0.0f) {
		const float w = (d4.m128_f32[0] - d3.m128_f32[0]) / ((d4.m128_f32[0] - d3.m128_f32[0]) + (d5.m128_f32[0] - d6.m128_f32[0]));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	const float denom = 1.0f / (va + vb + vc);
	const float v = vb * denom;
	const float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter) {
	XMVECTOR p{};
	// ���̒��S�ɑ΂���ŋߐړ_�ł���O�p�`��ɂ���_p��������
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	// �_p�Ƌ��̒��S�̍����x�N�g��
	XMVECTOR v = p - sphere.center;
	// �����̓������߂�
	// (�����x�N�g�����m�̓��ς͎O�����̒藝�̃��[�g�����̎��ƈ�v����)
	v = XMVector3Dot(v, v);
	// ���ƎO�p�`�̋��������a�ȉ��Ȃ瓖�����Ă��Ȃ�
	if (v.m128_f32[0] > sphere.radius * sphere.radius) return false;
	// �^����_���v�Z
	if (inter) {
		// �O�p�`��̍ŋߐړ_p���^����_�Ƃ���
		*inter = p;
	}
	return true;
}

bool Collision::CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance, DirectX::XMVECTOR* inter) {
	constexpr float epsilon = 1.0e-5f;	//�덷�z���p�̔����Ȓl
	// �ʕ����ƃ��C�̕����x�N�g���̓���
	const float d1 = XMVector3Dot(plane.normal, ray.dir).m128_f32[0];
	// ���ʂɂ͓�����Ȃ�
	if (d1 > -epsilon) return false;
	// �n�_�ƌ��_�̋���(���ʂ̖@������)
	// �ʕ����ƃ��C�̎n�_���W(�ʒu�x�N�g��)�̓���
	const float d2 = XMVector3Dot(plane.normal, ray.start).m128_f32[0];
	// �n�_�ƕ��ʂ̋���(���ʂ̖@������)
	const float dist = d2 - plane.distance;
	// �n�_�ƕ��ʂ̋���(���C����)
	const float t = dist / -d1;
	// ��_���n�_�����Ȃ瓖����Ȃ�
	if (t < 0) return false;
	// ��������������
	if (distance) *distance = t;
	// ��_���v�Z
	if (inter) *inter = ray.start + t * ray.dir;

	return true;
}

bool Collision::CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance, DirectX::XMVECTOR* inter) {
	//�O�p�`������Ă��镽�ʂ��Z�o
	Plane plane{};
	XMVECTOR interPlane{};
	plane.normal = triangle.normal;
	plane.distance = XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];
	//���C�ƕ��ʂ��������Ă��Ȃ����false��Ԃ�
	if (!CheckRay2Plane(ray, plane, distance, &interPlane)) return false;
	//-----�������Ă����̂ŁA�����ƌ�_���������܂ꂽ
	//��_���O�p�`�̓����ɂ��邩����
	const float epsilon = 1.0e-5f;	//�덷�z���p(���E���肬��𓖂������Ƃ��邽��)
	XMVECTOR m{};
	//��p0_p1�ɂ���
	const XMVECTOR pt_p0 = triangle.p0 - interPlane;
	const XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);
	//�ӂ̊O���Ȃ瓖�����Ă��Ȃ��̂Ŕ���I��
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) return false;
	//��p1_p2�ɂ���
	//�ӂ̊O���Ȃ瓖�����Ă��Ȃ��̂Ŕ���I��
	const XMVECTOR pt_p1 = triangle.p1 - interPlane;
	const XMVECTOR p1_p2 = triangle.p2 - triangle.p1;
	m = XMVector3Cross(pt_p1, p1_p2);
	//�ӂ̊O���Ȃ瓖�����Ă��Ȃ��̂Ŕ���I��
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) return false;
	//��p2_p0�ɂ���
	//�ӂ̊O���Ȃ瓖�����Ă��Ȃ��̂Ŕ���I��
	const XMVECTOR pt_p2 = triangle.p2 - interPlane;
	const XMVECTOR p2_p0 = triangle.p0 - triangle.p2;
	m = XMVector3Cross(pt_p2, p2_p0);
	//�ӂ̊O���Ȃ瓖�����Ă��Ȃ��̂Ŕ���I��
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) return false;
	//�����Ȃ̂œ������Ă���
	if (inter) *inter = interPlane;
	return true;
}

bool Collision::CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance, DirectX::XMVECTOR* inter) {
	XMVECTOR m = ray.start - sphere.center;
	const float b = XMVector3Dot(m, ray.dir).m128_f32[0];
	const float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;
	// lay�̎n�_��sphere�̊O���ɂ���(c > 0)�Alay��sphere���痣�������������ꍇ(b > 0)�A������Ȃ�
	if (c > 0.f && b > 0.f) return false;

	const float discr = b * b - c;
	// ���̔��ʎ��̓��C�����𗣂�Ă��邱�ƂɈ�v
	if (discr < 0.f) return false;

	// ----- ���C�͋��ƌ������Ă���

	// ��������ŏ��̒lt���v�Z
	float t = -b - sqrtf(discr);
	// t�����ł���ꍇ�A���C�͋��̓�������J�n���Ă���̂�t���[���ɃN�����v
	if (t < 0) t = 0.f;
	if (distance) *distance = t;

	if (inter) *inter = ray.start + t * ray.dir;

	return true;
}

bool Collision::CheckSphere2Sphere(const Sphere& sphere1, const Sphere& sphere2) {
	const XMVECTOR length = sphere1.center - sphere2.center;

	const float sqLength =
		length.m128_f32[0] * length.m128_f32[0] +
		length.m128_f32[1] * length.m128_f32[1] +
		length.m128_f32[2] * length.m128_f32[2];

	const float sumRadius = sphere1.radius + sphere2.radius;

	return sumRadius * sumRadius > sqLength;
}