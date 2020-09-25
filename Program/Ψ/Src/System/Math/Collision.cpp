#include "Collision.h"

using namespace DirectX;

//レイによる当たり判定
bool RayToMesh(const DirectX::XMVECTOR& rRayPos, const DirectX::XMVECTOR& rRayDir, float maxDistance, const Mesh& rMesh, const Matrix& rMatrix, RayResult& rResult)
{
	//モデルの逆行列でレイを変換
	XMMATRIX invMat = XMMatrixInverse(0,rMatrix);

	//レイの判定開始位置を逆変換
	XMVECTOR rayPos = XMVector3TransformCoord(rRayPos,invMat);

	//発射方向は正規化されていないと正しく判定できないので正規化
	XMVECTOR rayDir = XMVector3TransformNormal(rRayDir, invMat);

	//逆行列に拡縮が入っていると
	//レイが当たった距離にも拡縮が反映されしまうので
	//判定用の最大距離にも拡縮を反映させておく
	float dirLength = XMVector3Length(rayDir).m128_f32[0];
	float rayChackRange = maxDistance * dirLength;


	rayDir=XMVector3Normalize(rayDir);

	//------------------------------------------------------
	// ブロードフェイズ
	//	比較的軽量なAABB　vs レイな判定で
	//	明らかにヒットしていない場合は、これ以降の判定を中止
	//------------------------------------------------------
	{
		//AABB vs レイ
		float AABBdist = FLT_MAX;
		if (rMesh.GetBoundingBox().Intersects(rayPos, rayDir, AABBdist) == false) { return false; }

		//最大距離以降なら範囲外なので中止
		if (AABBdist > rayChackRange) { return false; }
	}

	//------------------------------------------------------
	// ナローフェイズ
	//	全ての面　vs レイ
	//------------------------------------------------------

	bool	ret			= false;		//当たったかどうか
	float	closestDist = FLT_MAX;		//当たった面との距離
	Vector3	hitPos = {};				//当たった場所

	//面情報の取得
	const MeshFace* pFaces = &rMesh.GetFaces()[0];//面情報の先頭を取得
	UINT faceNum = rMesh.GetFaces().size();


	//すべての面と当たり判定
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx)
	{
		//三角形を構成する３つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].Idx;

		//レイと三角形の当たり判定
		float triDist = FLT_MAX;
		bool bHit = DirectX::TriangleTests::Intersects
		(
			rayPos,			//発射場所
			rayDir,			//発射方向

			//判定する３角形の頂点情報
			rMesh.GetVertexPositions()[idx[0]],
			rMesh.GetVertexPositions()[idx[1]],
			rMesh.GetVertexPositions()[idx[2]],

			triDist			//当たった場合の距離
		);

		//ヒットしていなかったらスキップ
		if (bHit == false) { continue; }

		//最大距離以内か
		if (triDist <= rayChackRange)
		{
			//return true;
			ret = true;
			//当たり判定でとれる距離は拡縮に影響ないので、実際の長さを計算する
			triDist /= dirLength;
			//近いほうを優先して残す
			if (triDist<closestDist)
			{
				closestDist = triDist;	//距離を更新
			}
		}
	}
	//判定距離の格納
	rResult.m_distance = closestDist;
	//当たった場所の算出
	rResult.m_hitPos = rayPos + (rayDir * rResult.m_distance);
	//ワールド座標に変換
	rResult.m_hitPos = rMatrix.GetTranslation() + rResult.m_hitPos;

	//当たったかの真偽
	rResult.m_hit = ret;

	return ret;
}