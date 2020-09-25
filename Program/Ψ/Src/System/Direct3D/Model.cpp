#include"Model.h"
#include"GLTFLoader.h"

//コンストラクタ
Model::Model()
{

}

//デストラクタ
Model::~Model()
{
	Release();
}

//ロード関数
bool Model::Load(const std::string& filename)
{
	//ファイルの完全パスを取得
	std::string fileDir = GetDirFromPath(filename);

	//GLTFの読み込み
	std::shared_ptr<GLTFModel> spGltfModel = KdLoadGLTFModel(filename);
	if (spGltfModel == nullptr) { return false; }

	//ノード格納場所のメモリ確保
	m_originalNodes.resize(spGltfModel->Nodes.size());

	//メッシュの受け取り
	for (UINT i = 0; i < spGltfModel->Nodes.size(); ++i)
	{
		//入力元ノード
		const GLTFNode& rSrcNode = spGltfModel->Nodes[i];

		//出力先ノード
		Node& rDstNode = m_originalNodes[i];

		//ノード情報のセット
		rDstNode.m_name = rSrcNode.Name;
		rDstNode.m_localTransform = rSrcNode.LocalTransform;



		//ノードの内容がメッシュだったら
		if (rSrcNode.IsMesh)
		{
			//リストにメッシュのインスタンス化
			rDstNode.m_spMesh=std::make_shared<Mesh>();

			//メッシュ情報の作成
			if (rDstNode.m_spMesh)
			{
				std::string vecStr;
				vecStr = std::to_string(rSrcNode.IsMesh);
				OutputDebugStringA(vecStr.append("\n").c_str());

				rDstNode.m_spMesh->Create
				(
					rSrcNode.Mesh.Vertices,
					rSrcNode.Mesh.Faces,
					rSrcNode.Mesh.Subsets
				);
			}
		}
	}

	//マテリアルを受け取れるサイズのメモリを確保
	m_materials.resize(spGltfModel->Materials.size());

	for (UINT i=0;i<m_materials.size();++i)
	{
		const GLTFMaterial& rSrcMaterial = spGltfModel->Materials[i];
		Material& rDstMaterial = m_materials[i];

		//名前
		rDstMaterial.Name = rSrcMaterial.Name;

		//基本色
		rDstMaterial.BaseColor = rSrcMaterial.BaseColor;
		rDstMaterial.BaseColorTex = std::make_shared<Texture>();

		//テクスチャー読み込み
		rDstMaterial.BaseColorTex = ResFac.GetTexture(fileDir + rSrcMaterial.BaseColorTexture);
		if (rDstMaterial.BaseColorTex==nullptr)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}
	}

	return true;
}

void Model::Release()
{
	m_materials.clear();
	m_originalNodes.clear();
}
