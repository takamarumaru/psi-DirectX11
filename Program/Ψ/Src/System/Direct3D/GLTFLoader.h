﻿#pragma once

//=====================================================
//
// GLTF形式の3Dモデルファイルを読み込む
//
//=====================================================


//============================
// マテリアル
//============================
struct GLTFMaterial
{
	std::string		Name;						// マテリアル名


	std::string		AlphaMode = "OPAQUE";		// "OPAQUE" : レンダリングされた出力は完全に不透明で、アルファ値は無視されます。
												// "MASK"   : レンダリングされた出力は、アルファ値と指定されたアルファカットオフ値に応じて、完全に不透明または完全に透明になります。このモードは、木の葉やワイヤーフェンスなどのジオメトリをシミュレートするために使用されます。
												// "BLEND"  : 

	float			AlphaCutoff = 0.5f;			// MASKモード時に、カットオフの閾値として使用　それ以外のモードでは使用されない
	bool			DoubleSided = false;		// 両面か？

	//------------------------------
	// PBR材質データ
	//------------------------------
	std::string		BaseColorTexture;			// 基本色テクスチャのファイル名
	Math::Vector4	BaseColor = { 1,1,1,1 };		// 上記テクスチャのRGBAのスケーリング要素

	// 金属性、粗さ
	std::string		MetallicRoughnessTexture;	// メタリックとラフネスのテクスチャ　青成分 = メタリック 緑成分 = ラフネス
	float			Metallic = 1.0f;			// 上記テクスチャのメタリック要素の乗算用　テクスチャが無い時は乗算ではなくそのまま使用する
	float			Roughness = 1.0f;			// 上記テクスチャのラフネス要素の乗算用　テクスチャが無い時は乗算ではなくそのまま使用する

	// エミッシブ：自己発光 つまり表面から放出される光　RGBのみ使用
	std::string		EmissiveTexture;			// エミッシブテクスチャ　RGBを使用
	Math::Vector3	Emissive = { 0,0,0 };		// 上記テクスチャのRGBのスケーリング要素

	//------------------------------
	// その他テクスチャ
	//------------------------------
	std::string		NormalTexture;				// 法線マップテクスチャ
	std::string		OcclusionTexture;			// 光の遮蔽度テクスチャ　赤成分のみ使用
};

//============================
// ノード １つのメッシュやマテリアルなど
//============================
struct GLTFNode
{
	//---------------------------
	// 基本情報
	//---------------------------

	// 名前
	std::string								Name;

	// 子Indexリスト
	std::vector<int>						Children;
	// 親Index
	int										Parent = -1;

	// 行列
	Math::Matrix							LocalTransform;
	Math::Matrix							WorldTransform;

	//---------------------------
	// Mesh専用情報
	//---------------------------
	bool									IsMesh = false;
	struct Mesh
	{
		// 頂点配列
		std::vector<MeshVertex>				Vertices;
		// 面情報配列
		std::vector<MeshFace>				Faces;
		// マテリアル番号配列
		std::vector<UINT>					Materials;
		// サブセット情報配列
		std::vector<MeshSubset>				Subsets;
	};
	Mesh									Mesh;

};

//============================
// アニメーションデータ
//============================
struct GLTFAnimationData
{
	//アニメーション名
	std::string				m_name;
	//アニメの長さ
	float					m_maxLength = 0;
	//1ノードのアニメーションデータ
	struct Node
	{
		//対象ノードのOffset
		int									m_nodeOffset = -1;
		//各チャンネル
		std::vector<AnimKeyQuaternion>	m_rotations;	//回転キーリスト
		std::vector<AnimKeyVector3>		m_scales;		//拡縮キーリスト
		std::vector<AnimKeyVector3>		m_translations;	//位置キーリスト
	};
	//全ノード用アニメーションデータ
	std::vector<std::shared_ptr<Node>> m_nodes;
};

//============================
// モデルデータ
//============================
struct GLTFModel
{
	// 全ノードデータ
	std::vector<GLTFNode>						Nodes;

	// 全ノード中のルートノードのみの参照リスト
	std::vector<GLTFNode*>					RootNodes;

	// マテリアル一覧
	std::vector<GLTFMaterial>					Materials;

	// アニメーションデータリスト
	std::vector<std::shared_ptr<GLTFAnimationData>>		Animations;

};

//===================================================
// GLTF形式の3Dモデルを読み込む
// LoaderはTinygltfを使用しています。
// github:https://github.com/syoyo/tinygltf
// 
// ・path				… .glflファイルのパス
//===================================================
std::shared_ptr<GLTFModel> KdLoadGLTFModel(const std::string& path);
