#include "Collision.h"

using namespace DirectX;

bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter) {
	//座標系の原点から球の中心への座標
	const XMVECTOR distV = XMVector3Dot(sphere.center, plane.normal);
	//平面の原点距離を計算すると平面と球の中心との距離が出る
	const float dist = distV.m128_f32[0] - plane.distance;
	//距離の絶対値が半径より大きければ当たっていない
	if (fabsf(dist) > sphere.radius) return false;

	//疑似交点を計算
	if (inter) {
		//平面上の最近接点を疑似交点とする
		*inter = -dist * plane.normal + sphere.center;
	}

	return true;
}

void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point,
	const Triangle& triangle, DirectX::XMVECTOR* closest) {
	// pointがp0の外側の頂点領域の中にあるかどうかチェック
	const XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	const XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	const XMVECTOR p0_pt = point - triangle.p0;

	const XMVECTOR d1 = XMVector3Dot(p0_p1, p0_pt);
	const XMVECTOR d2 = XMVector3Dot(p0_p2, p0_pt);

	if (d1.m128_f32[0] <= 0.0f && d2.m128_f32[0] <= 0.0f) {
		// p0が最近傍
		*closest = triangle.p0;
		return;
	}

	// pointがp1の外側の頂点領域の中にあるかどうかチェック
	const XMVECTOR p1_pt = point - triangle.p1;

	const XMVECTOR d3 = XMVector3Dot(p0_p1, p1_pt);
	const XMVECTOR d4 = XMVector3Dot(p0_p2, p1_pt);

	if (d3.m128_f32[0] >= 0.0f && d4.m128_f32[0] <= d3.m128_f32[0]) {
		// p1が最近傍
		*closest = triangle.p1;
		return;
	}

	// pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	const float vc = d1.m128_f32[0] * d4.m128_f32[0] - d3.m128_f32[0] * d2.m128_f32[0];
	if (vc <= 0.0f && d1.m128_f32[0] >= 0.0f && d3.m128_f32[0] <= 0.0f) {
		float v = d1.m128_f32[0] / (d1.m128_f32[0] - d3.m128_f32[0]);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// pointがp2の外側の頂点領域の中にあるかどうかチェック
	const XMVECTOR p2_pt = point - triangle.p2;

	const XMVECTOR d5 = XMVector3Dot(p0_p1, p2_pt);
	const XMVECTOR d6 = XMVector3Dot(p0_p2, p2_pt);
	if (d6.m128_f32[0] >= 0.0f && d5.m128_f32[0] <= d6.m128_f32[0]) {
		*closest = triangle.p2;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5.m128_f32[0] * d2.m128_f32[0] - d1.m128_f32[0] * d6.m128_f32[0];
	if (vb <= 0.0f && d2.m128_f32[0] >= 0.0f && d6.m128_f32[0] <= 0.0f) {
		float w = d2.m128_f32[0] / (d2.m128_f32[0] - d6.m128_f32[0]);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
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
	// 球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	// 点pと球の中心の差分ベクトル
	XMVECTOR v = p - sphere.center;
	// 距離の二乗を求める
	// (同じベクトル同士の内積は三平方の定理のルート内部の式と一致する)
	v = XMVector3Dot(v, v);
	// 球と三角形の距離が半径以下なら当たっていない
	if (v.m128_f32[0] > sphere.radius * sphere.radius) return false;
	// 疑似交点を計算
	if (inter) {
		// 三角形上の最近接点pを疑似交点とする
		*inter = p;
	}
	return true;
}

bool Collision::CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance, DirectX::XMVECTOR* inter) {
	constexpr float epsilon = 1.0e-5f;	//誤差吸収用の微小な値
	// 面方向とレイの方向ベクトルの内積
	const float d1 = XMVector3Dot(plane.normal, ray.dir).m128_f32[0];
	// 裏面には当たらない
	if (d1 > -epsilon) return false;
	// 始点と原点の距離(平面の法線方向)
	// 面方向とレイの始点座標(位置ベクトル)の内積
	const float d2 = XMVector3Dot(plane.normal, ray.start).m128_f32[0];
	// 始点と平面の距離(平面の法線方向)
	const float dist = d2 - plane.distance;
	// 始点と平面の距離(レイ方向)
	const float t = dist / -d1;
	// 交点が始点より後ろなら当たらない
	if (t < 0) return false;
	// 距離を書き込む
	if (distance) *distance = t;
	// 交点を計算
	if (inter) *inter = ray.start + t * ray.dir;

	return true;
}

bool Collision::CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance, DirectX::XMVECTOR* inter) {
	//三角形が乗っている平面を算出
	Plane plane{};
	XMVECTOR interPlane{};
	plane.normal = triangle.normal;
	plane.distance = XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];
	//レイと平面が当たっていなければfalseを返す
	if (!CheckRay2Plane(ray, plane, distance, &interPlane)) return false;
	//-----当たっていたので、距離と交点が書き込まれた
	//交点が三角形の内側にあるか判定
	const float epsilon = 1.0e-5f;	//誤差吸収用(境界ぎりぎりを当たったとするため)
	XMVECTOR m{};
	//辺p0_p1について
	const XMVECTOR pt_p0 = triangle.p0 - interPlane;
	const XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);
	//辺の外側なら当たっていないので判定終了
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) return false;
	//辺p1_p2について
	//辺の外側なら当たっていないので判定終了
	const XMVECTOR pt_p1 = triangle.p1 - interPlane;
	const XMVECTOR p1_p2 = triangle.p2 - triangle.p1;
	m = XMVector3Cross(pt_p1, p1_p2);
	//辺の外側なら当たっていないので判定終了
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) return false;
	//辺p2_p0について
	//辺の外側なら当たっていないので判定終了
	const XMVECTOR pt_p2 = triangle.p2 - interPlane;
	const XMVECTOR p2_p0 = triangle.p0 - triangle.p2;
	m = XMVector3Cross(pt_p2, p2_p0);
	//辺の外側なら当たっていないので判定終了
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) return false;
	//内側なので当たっている
	if (inter) *inter = interPlane;
	return true;
}

bool Collision::CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance, DirectX::XMVECTOR* inter) {
	XMVECTOR m = ray.start - sphere.center;
	const float b = XMVector3Dot(m, ray.dir).m128_f32[0];
	const float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;
	// layの始点がsphereの外側にあり(c > 0)、layがsphereから離れる方向をさす場合(b > 0)、当たらない
	if (c > 0.f && b > 0.f) return false;

	const float discr = b * b - c;
	// 負の判別式はレイが球を離れていることに一致
	if (discr < 0.f) return false;

	// ----- レイは球と交差している

	// 交差する最小の値tを計算
	float t = -b - sqrtf(discr);
	// tが負である場合、レイは球の内側から開始しているのでtをゼロにクランプ
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