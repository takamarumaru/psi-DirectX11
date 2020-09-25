#pragma once

class Model
{
public:
	//コンストラクタ
	Model();
	//デストラクタ
	~Model();

	bool Load(const std::string& filename);

	//アクセサ
	const std::shared_ptr<Mesh> GetMesh(UINT index) const 
	{
		return index < m_originalNodes.size() ? m_originalNodes[index].m_spMesh:nullptr; 
	}
	const std::vector<Material>& GetMaterials()const { return m_materials; }

	//ノード：モデルを形成するメッシュを扱うための最小単位
	struct Node
	{
		std::string					m_name;				//ノード名
		Matrix					m_localTransform;	//変数行列
		std::shared_ptr <Mesh>	m_spMesh;			//メッシュ情報
	};

	//文字列を元にノードの検索
	inline Node* FindNode(const std::string& name)
	{
		for (auto&& node:m_originalNodes)
		{
			if (node.m_name == name) 
			{
				return &node;
			}
		}
		return nullptr;
	}

	//ノード配列取得
	const std::vector<Node>& GetOriginalNodes() const { return m_originalNodes; }

private:

	void Release();							//解放
	std::vector<Node>	m_originalNodes;	//データのノード配列

	//マテリアル配列
	std::vector<Material> m_materials;
};