#include "ModelComponent.h"

//モデルセット
void ModelComponent::SetModel(const std::shared_ptr<Model>& model)
{
	//使用しているモデルをセット
	m_spModel = model;

	//念のためコピー用配列はクリア
	m_coppiedNode.clear();

	//ノードのコピー
	if (model)
	{
		m_coppiedNode = model->GetOriginalNodes();
	}
}

void ModelComponent::Draw()
{
	//有効じゃないとき
	if (m_enable == false) { return; }

	//モデルがないとき
	if (m_spModel == nullptr) { return; }

	
	//全てのノードを一つ一つ描画
	for (UINT i = 0; i < m_coppiedNode.size(); i++)
	{
		auto& rNode = m_coppiedNode[i];
		if (rNode.m_spMesh == nullptr) { continue; }

		SHADER.m_standardShader.SetWorldMatrix(rNode.m_localTransform * m_owner.GetMatrix());
		SHADER.m_standardShader.DrawMesh(rNode.m_spMesh.get(), m_spModel->GetMaterials());
	}
}
