#include "TrailPolygon.h"

#include"./Application/Game/Scene.h"

//通常描画
void TrailPolygon::Draw(float width)
{
	//ポイントが２追加の場合は描画不可
	if (m_pointList.size() < 2) { return; }

	//軌跡画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);

	//頂点配列
	std::vector<Vertex> vertex;
	//ポイント数分サイズ確保
	vertex.resize(m_pointList.size() * 2);

	//===============================
	//頂点データ作成
	//===============================
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		//登録行列の参照（ショートカット）
		Math::Matrix& mat = m_pointList[i];

		//二つの頂点の参照(ショートカット)
		Vertex& v1 = vertex[i * 2];
		Vertex& v2 = vertex[i * 2 + 1];

		//X方向
		Math::Vector3 axisX = mat.Right();
		axisX.Normalize();

		//座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		//UV
		float uvY = i / sliceCount;
		v1.UV = { 0,uvY };
		v2.UV = { 1,uvY };
	}
	SHADER.m_effectShader.SetWorldMatrix(Matrix());

	SHADER.m_effectShader.WriteToCB();

	//テクスチャセット
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	//指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));
}

void TrailPolygon::DrawDetached(float width, Vector3 startDir)
{
	//ポイントが２追加の場合は描画不可
	if (m_pointList.size() < 2) { return; }

	//軌跡画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);
	Vector3 prevUp = startDir;

	//===============================
	//頂点データ作成
	//===============================
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		if (i == 0) { continue; }

		//頂点配列
		std::vector<Vertex> vertex;
		//ポイント数分サイズ確保
		vertex.resize(4);

		//登録行列の参照（ショートカット）
		Math::Matrix& mat = m_pointList[i];
		Math::Matrix& prevMat = m_pointList[i-1];

		//ラインの向き
		Math::Vector3 vDir;
		vDir = mat.Translation() - prevMat.Translation();
		SCENE.AddDebugLine(prevMat.Translation(), mat.Translation(), {1,0,0,1});
		SCENE.AddDebugLine(prevMat.Translation(), prevMat.Translation() + Vector3::VectorNomalize(prevUp), { 1,1,0,1 });

		Math::Vector3 axisX = DirectX::XMVector3Cross(Vector3::VectorNomalize(vDir), prevUp);
		axisX.Normalize();
		axisX.x = round(axisX.x * 10.0f) * 0.1f;
		axisX.y = round(axisX.y * 10.0f) * 0.1f;
		axisX.z = round(axisX.z * 10.0f) * 0.1f;

		//次に使う上側座標を求めておく
		prevUp = DirectX::XMVector3Cross(vDir, axisX);
		prevUp.Normalize();


		//座標
		vertex[0].Pos = mat.Translation() + axisX * width * 0.5f;
		vertex[1].Pos = mat.Translation() - axisX * width * 0.5f;
		vertex[2].Pos = prevMat.Translation() + axisX * width * 0.5f;
		vertex[3].Pos = prevMat.Translation() - axisX * width * 0.5f;
		SCENE.AddDebugLine(mat.Translation(), vertex[0].Pos, { 1,0,0,1 });
		SCENE.AddDebugLine(mat.Translation(), vertex[1].Pos, { 1,0,0,1 });
		SCENE.AddDebugLine(prevMat.Translation(), vertex[2].Pos, { 1,0,0,1 });
		SCENE.AddDebugLine(prevMat.Translation(), vertex[3].Pos, { 1,0,0,1 });


		//UV
		float uvY = i / sliceCount;
		float pUvY = i-1 / sliceCount;
		vertex[0].UV = { 0,0 };
		vertex[1].UV = { 1,0 };
		vertex[2].UV = { 0,1 };
		vertex[3].UV = { 1,1 };

		SHADER.m_effectShader.SetWorldMatrix(Matrix());

		SHADER.m_effectShader.WriteToCB();

		//テクスチャセット
		if (m_texture)
		{
			D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
		}
		else
		{
			D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
		}

		//指定した頂点配列を描画する関数
		D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &vertex[0], sizeof(Vertex));
	}
}

//ビルボード描画
void TrailPolygon::DrawBillboard(float width)
{
	//ポイントが２追加の場合は描画不可
	if (m_pointList.size() < 2) { return; }

	//カメラの情報
	Math::Matrix mCam = SHADER.m_cb7_Camera.GetWork().mV.Invert();

	//軌跡画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);

	//頂点配列
	std::vector<Vertex> vertex;
	//ポイント数分サイズ確保
	vertex.resize(m_pointList.size() * 2);

	//===============================
	//頂点データ作成
	//===============================
	Math::Vector3 prevPos;
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		//登録行列の参照（ショートカット）
		Math::Matrix& mat = m_pointList[i];

		//二つの頂点の参照(ショートカット)
		Vertex& v1 = vertex[i * 2];
		Vertex& v2 = vertex[i * 2 + 1];

		//ラインの向き
		Math::Vector3 vDir;
		if (i==0)
		{
			//初回時のみ次のポイントを使用
			vDir = m_pointList[1].Translation() - mat.Translation();
		}
		else
		{
			//2回目以降は、前回の座標から向きを決定する
			vDir = mat.Translation() - prevPos;
		}

		//カメラからポイントへの向き
		Math::Vector3 v = mat.Translation() - mCam.Translation();
		Math::Vector3 axisX = DirectX::XMVector3Cross(vDir, v);
		axisX.Normalize();

		//座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		//UV
		float uvY = i / sliceCount;
		v1.UV = { 0,uvY };
		v2.UV = { 1,uvY };

		//座標を記憶しておく
		prevPos = mat.Translation();
	}
	SHADER.m_effectShader.SetWorldMatrix(Matrix());

	SHADER.m_effectShader.WriteToCB();

	//テクスチャセット
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	//指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));
}

//頂点情報をそのまま繋げてポリゴンを描画
void TrailPolygon::DrawStrip()
{
	UINT pointListSize = m_pointList.size();
	if (pointListSize < 4) { return; }

	//頂点配列
	std::vector<Vertex> vertex;
	vertex.resize(pointListSize);

	//軌跡画像の分割数
	float sliceNum = pointListSize * 0.5f;

	//頂点データ作成
	for (UINT i=0;i<pointListSize;i++)
	{
		Vertex& rVertex = vertex[i];

		//頂点座標
		rVertex.Pos = m_pointList[i].Translation();

		//UV
		//X座標は左右の頂点として画像の両端　0.0と1.0を往復する
		rVertex.UV.x = (float)(i % 2);
		rVertex.UV.y = (i * 0.5f) / sliceNum;
	}

	//テクスチャセット
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	//指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));
}
