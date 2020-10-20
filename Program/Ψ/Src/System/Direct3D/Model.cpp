#include"Model.h"
#include"GLTFLoader.h"

//コンストラクタ
Model::Model(){}

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

	//アニメーションデータ
	m_spAnimations.resize(spGltfModel->Animations.size());

	for (UINT i = 0; i < m_spAnimations.size(); ++i)
	{
		const GLTFAnimationData& rSrcAnimation = *spGltfModel->Animations[i];

		m_spAnimations[i] = std::make_shared<AnimationData>();
		AnimationData& rDstAnimation = *(m_spAnimations[i]);

		rDstAnimation.m_name = rSrcAnimation.m_name;

		rDstAnimation.m_maxLength = rSrcAnimation.m_maxLength;

		rDstAnimation.m_nodes.resize(rSrcAnimation.m_nodes.size());

		for (UINT i = 0; i < rDstAnimation.m_nodes.size(); ++i)
		{
			rDstAnimation.m_nodes[i].m_nodeOffset = rSrcAnimation.m_nodes[i]->m_nodeOffset;
			rDstAnimation.m_nodes[i].m_translations = rSrcAnimation.m_nodes[i]->m_translations;
			rDstAnimation.m_nodes[i].m_rotaions = rSrcAnimation.m_nodes[i]->m_rotations;
		}
	}

	return true;
}

//アニメーションデータ取得
const std::shared_ptr<AnimationData> Model::GetAnimation(const std::string& animName) const
{
	for (auto&& anim : m_spAnimations)
	{
		if (anim->m_name == animName)
		{
			return anim;
		}
	}

	return nullptr;
}

void Model::Release()
{
	m_materials.clear();
	m_originalNodes.clear();
}
