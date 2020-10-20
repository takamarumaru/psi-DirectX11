#pragma once

#include"../Game/GameObject.h"

//======================================
//モデルコンポーネント
//======================================

class ModelComponent
{
public:
	ModelComponent(GameObject& owner) :m_owner(owner) {}

	//有効フラグ
	bool IsEnable()const { return m_enable; }
	void SetEnable(bool enable) { m_enable = enable; }

	//ノード取得
	const std::vector<Model::Node>& GetNodes()const { return m_coppiedNode; }

	//メッシュ取得
	inline const std::shared_ptr<Mesh>GetMesh(UINT index)const
	{
		if (index >= m_coppiedNode.size()) { return nullptr; }
		return m_coppiedNode[index].m_spMesh;
	}


	inline Model::Node* FindNode(const std::string& name)
	{
		//コピーしたノードの中から検索をかける
		for (auto&& node : m_coppiedNode)
		{
			if(node.m_name==name)
			{
				return &node;
			}
		}
		return nullptr;
	}

	//アニメーションデータ取得
	const std::shared_ptr<AnimationData>GetAnimation(const std::string& animName)const
	{
		if (!m_spModel) { return nullptr; }
		return m_spModel->GetAnimation(animName);
	}

	std::vector<Model::Node>& GetChangeableNodes() { return m_coppiedNode; }

	//モデルセット
	void SetModel(const std::shared_ptr<Model>& model);

	//StandardShaderで描画
	void Draw();

private:

	//個別管理のため、オリジナルからコピーして保持する配列
	std::vector<Model::Node>	m_coppiedNode;

	//有効
	bool m_enable = true;

	//モデルデータの参照
	std::shared_ptr<Model> m_spModel;

	GameObject& m_owner;

};