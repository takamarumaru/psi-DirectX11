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
	const std::vector<Model::Node>& GetNodes()const { return m_coppiedNodes; }

	//メッシュ取得
	inline const std::shared_ptr<Mesh>GetMesh(UINT index)const
	{
		if (index >= m_coppiedNodes.size()) { return nullptr; }
		return m_coppiedNodes[index].m_spMesh;
	}


	inline Model::Node* FindNode(const std::string& name)
	{
		//コピーしたノードの中から検索をかける
		for (auto&& node : m_coppiedNodes)
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

	std::vector<Model::Node>& GetChangeableNodes() { return m_coppiedNodes; }

	//モデルセット
	void SetModel(const std::shared_ptr<Model>& model);
	std::shared_ptr<Model> GetModel(){return m_spModel;}

	// エミッシブフラグをセット
	void SetEmissive(const bool val)
	{
		m_isEmissive = val;
	}
	bool GetEmissive(){return m_isEmissive;}

	//StandardShaderで描画
	void Draw();

	// シャドウマップ生成描画
	void DrawShadowMap();


private:

	//個別管理のため、オリジナルからコピーして保持する配列
	std::vector<Model::Node>	m_coppiedNodes;

	//有効
	bool m_enable = true;

	//エミッシブ(自己発光)フラグ
	bool m_isEmissive = false;

	//モデルデータの参照
	std::shared_ptr<Model> m_spModel;

	GameObject& m_owner;

};