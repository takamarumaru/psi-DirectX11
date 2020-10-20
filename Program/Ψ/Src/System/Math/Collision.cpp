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

				//外積計算のためのベクトル算出
				const Vector3 BtoC = rMesh.GetVertexPositions()[idx[1]] - rMesh.GetVertexPositions()[idx[0]];
				const Vector3 AtoB = rMesh.GetVertexPositions()[idx[2]] - rMesh.GetVertexPositions()[idx[0]];
				//ポリゴンの外積を求める
				rResult.m_polyDir = Vector3::Cross(BtoC,AtoB);
				rResult.m_polyDir.Normalize();
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


//球とメッシュの当たり判定
bool SphereToMesh(const Math::Vector3& rSpherePos, float radius, const Mesh& mesh, const DirectX::XMMATRIX& matrix, Math::Vector3& rPushedPos)
{
	//======================================================
	// ブロードフェイズ
	//======================================================
	{
		//メッシュのAABBを元に、行列で変換したAABBを作成
		BoundingBox aabb;
		mesh.GetBoundingBox().Transform(aabb, matrix);

		//判定した結果当たっていなければ帰る
		BoundingSphere bs(rSpherePos, radius);
		if (aabb.Intersects(bs) == false)return false;
	}

	//======================================================
	// ナローフェイズ
	//======================================================

	bool bHit = false;

	//DEBAGビルドでも速度を維持するために、別変数に拾っておく
	const auto* pFaces = &mesh.GetFaces()[0];
	UINT faceNum = mesh.GetFaces().size();
	const Math::Vector3* vertices = &mesh.GetVertexPositions()[0];

	//メッシュの逆行列で球の中心座標を変換
	XMMATRIX invM = XMMatrixInverse(0, matrix);
	XMVECTOR QPos = rSpherePos;
	QPos = XMVector3TransformCoord(QPos, invM);

	//半径の二乗を計算しておく
	float radiusSq = radius * radius;

	//行列の各軸の拡大率を計算しておく
	XMVECTOR scale;
	scale.m128_f32[0] = XMVector3Length(matrix.r[0]).m128_f32[0];
	scale.m128_f32[1] = XMVector3Length(matrix.r[1]).m128_f32[0];
	scale.m128_f32[2] = XMVector3Length(matrix.r[2]).m128_f32[0];
	scale.m128_f32[3] = 0;

	//計算用変数
	XMVECTOR nearPt;
	XMVECTOR vToCenter;

	//全ての面と判定
	//判定はメッシュのローカル空間で行われる
	for (UINT fi = 0; fi < faceNum; fi++)
	{
		//点と三角形の最近接点を求める
		PointToTriangle
		(
			QPos,
			vertices[pFaces[fi].Idx[0]],
			vertices[pFaces[fi].Idx[1]],
			vertices[pFaces[fi].Idx[2]],
			nearPt
		);

		//最近接点から球の中心へのベクトルを求める
		vToCenter = QPos - nearPt;

		//拡大率を加味
		vToCenter *= scale;

		//vToCenterが半径以内の場合は衝突している
		//XMVector3LengthSq関数はベクトルの長さの二乗を返す。
		//XMVector3Length関数に比べて平方根の計算が入らない分軽い
		if (XMVector3LengthSq(vToCenter).m128_f32[0] <= radiusSq)
		{
			//最近接点を出力データに記憶する（逆行列で変換した物を元に戻す）
			nearPt = XMVector3TransformCoord(nearPt, matrix);

			//押し戻し計算
			XMVECTOR v = XMVector3Normalize(vToCenter);
			v *= radius - XMVector3Length(vToCenter).m128_f32[0];

			//拡縮を考慮した座標系に戻す
			v /= scale;

			//球の座標を移動させる
			QPos += v;

			//当たったフラグをON
			bHit = true;
		}
	}

	//当たっていた場合押し戻された球の座標を格納
	if (bHit)
	{
		rPushedPos = XMVector3TransformCoord(QPos, matrix);
	}

	return bHit;
}

void PointToTriangle(const XMVECTOR& p, const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c, XMVECTOR& outPt)
{
	// ※参考:[書籍]「ゲームプログラミングのためのリアルタイム衝突判定」
	// pがaの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR ab = b - a;
	XMVECTOR ac = c - a;
	XMVECTOR ap = p - a;
	float d1 = XMVector3Dot(ab, ap).m128_f32[0];//ab.Dot(ap);
	float d2 = XMVector3Dot(ac, ap).m128_f32[0];//ac.Dot(ap);
	if (d1 <= 0.0f && d2 <= 0.0f) {
		outPt = a; // 重心座標(1,0,0)
		return;
	}
	// pがbの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR bp = p - b;
	float d3 = XMVector3Dot(ab, bp).m128_f32[0];//ab.Dot(bp);
	float d4 = XMVector3Dot(ac, bp).m128_f32[0];//ac.Dot(bp);
	if (d3 >= 0.0f && d4 <= d3) {
		outPt = b; // 重心座標(0,1,0)
		return;
	}
	// pがabの辺領域の中にあるかどうかチェックし、あればpのab上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		outPt = a + ab * v; // 重心座標(1-v,v,0)
		return;
	}
	// pがcの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR cp = p - c;
	float d5 = XMVector3Dot(ab, cp).m128_f32[0];//ab.Dot(cp);
	float d6 = XMVector3Dot(ac, cp).m128_f32[0];;//ac.Dot(cp);
	if (d6 >= 0.0f && d5 <= d6) {
		outPt = c; // 重心座標(0,0,1)
		return;
	}
	// pがacの辺領域の中にあるかどうかチェックし、あればpのac上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		outPt = a + ac * w; // 重心座標(1-w,0,w)
		return;
	}
	// pがbcの辺領域の中にあるかどうかチェックし、あればpのbc上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		outPt = b + (c - b) * w; // 重心座標(0,1-w,w)
		return;
	}
	// pは面領域の中にある。Qをその重心座標(u,v,w)を用いて計算
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	outPt = a + ab * v + ac * w; // = u*a + v*b + w*c, u = va*demon = 1.0f - v - w
}

bool BoxToBox(const Mesh& myMesh, const DirectX::XMMATRIX& myMatrix, const Mesh& mesh, const DirectX::XMMATRIX& matrix)
{
	//1つ目のAABBを作成
	BoundingBox aabb1;
	myMesh.GetBoundingBox().Transform(aabb1, myMatrix);

	//2つ目のAABBを作成
	BoundingBox aabb2;
	mesh.GetBoundingBox().Transform(aabb2, matrix);

	//判定
	if (aabb1.Intersects(aabb2) == false)
	{
		return false;
	}
	return true;
}
