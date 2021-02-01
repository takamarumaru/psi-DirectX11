#include "ModelComponent.h"

//モデルセット
void ModelComponent::SetModel(const std::shared_ptr<Model>& model)
{
	//使用しているモデルをセット
	m_spModel = model;

	//念のためコピー用配列はクリア
	m_coppiedNodes.clear();

	//ノードのコピー
	if (model)
	{
		m_coppiedNodes = model->GetOriginalNodes();
	}
}

void ModelComponent::Draw()
{
	//有効じゃないとき
	if (m_enable == false) { return; }

	//モデルがないとき
	if (m_spModel == nullptr) { return; }


	//エミッシブをセット
	SHADER.m_modelShader.SetEmissive(m_isEmissive);

	//------------------------
	// リムライト設定
	//------------------------
	SHADER.m_modelShader.SetRimColor(m_rimColor);


	//全てのノードを一つ一つ描画
	for (UINT i = 0; i < m_coppiedNodes.size(); i++)
	{
		auto& rNode = m_coppiedNodes[i];
		if (rNode.m_spMesh == nullptr) { continue; }

		//当たり判定用ならカリングしない
		if (rNode.m_name == "Collision")
		{
			//Z情報は使うが、Z書き込みOFF
			D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);
			//カリングなし
			D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);
		}

		// 行列セット
		SHADER.m_modelShader.SetWorldMatrix(rNode.m_localTransform * m_owner.GetMatrix());

		// 描画
		SHADER.m_modelShader.DrawMesh(rNode.m_spMesh.get(), m_spModel->GetMaterials());

		//透過オブジェクトの場合もとに戻す
		if (rNode.m_name == "Collision")
		{
			D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
			D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);
		}

	}
}

void ModelComponent::DrawShadowMap()
{
	// 有効じゃないときはスキップ
	if (m_enable == false)return;
	// モデルがないときはスキップ
	if (m_spModel == nullptr)return;


	// 全ノード(メッシュ)を描画
	for (UINT i = 0; i < m_coppiedNodes.size(); i++)
	{
		auto& rNode = m_coppiedNodes[i];

		//当たり判定用のNodeならスキップ
		if (rNode.m_name=="Collision") { continue; }

		// メッシュがない場合はスキップ
		if (rNode.m_spMesh == nullptr) { continue; }

		// 行列セット
		SHADER.m_genShadowMapShader.SetWorldMatrix(rNode.m_localTransform * m_owner.GetMatrix());

		// 描画
		SHADER.m_genShadowMapShader.DrawMeshDepth(rNode.m_spMesh.get(), m_spModel->GetMaterials());
	}
}
