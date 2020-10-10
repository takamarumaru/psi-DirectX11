﻿#pragma once
class Mesh;

//レイ判定をした時の結果情報
struct RayResult
{
	float	m_distance	= FLT_MAX;		//当たったところまでの距離
	bool	m_hit		= false;		//当たったかどうか
	Vector3	m_hitPos = {};				//当たった座標
	Vector3 m_polyDir = {};				//当たったポリゴンの向き
};

//レイによる当たり判定
bool RayToMesh(
	const DirectX::XMVECTOR& rRayPos,
	const DirectX::XMVECTOR& rRayDir,
	float maxDistance,
	const Mesh& rMesh,
	const Matrix& rMatrix,
	RayResult& rResult
);

//球とメッシュの当たり判定
bool SphereToMesh(
	const Math::Vector3& rSpherePos,	//球の中心座標
	float radius,						//半径
	const Mesh& mesh,					//判定するメッシュ情報
	const DirectX::XMMATRIX& matrix,	//判定する相手の行列
	Math::Vector3& rPushedPos			//当たってた場合、押し返された球の中心点
);

//点とメッシュの当たり判定
void PointToTriangle(
	const DirectX::XMVECTOR& p,
	const DirectX::XMVECTOR& a,
	const DirectX::XMVECTOR& b,
	const DirectX::XMVECTOR& c,
	DirectX::XMVECTOR& outPt
);