#pragma once

#include "EffectShader/EffectShader.h"
#include "StandardShader/StandardShader.h"
#include "SpriteShader/SpriteShader.h"

//==========================================================
//
// シェーダ関係をまとめたクラス
//
//==========================================================
class ShaderManager
{
public:

	//==============================================================
	//
	// 初期化・解放
	//
	//==============================================================

	// 初期化
	void Init();

	// 解放
	void Release();

	//==========================
	//
	// シェーダ
	//
	//==========================
	StandardShader		m_standardShader;		// 3Dモデル描画シェーダ
	EffectShader			m_effectShader;			// エフェクト描画シェーダ
	SpriteShader			m_spriteShader;			// 2Dテクスチャ描画シェーダ


	//==========================
	//
	// カメラ関係
	//
	//==========================

	// カメラ定数バッファ用構造体
	struct cbCamera
	{
		Math::Matrix		mV;			// ビュー行列
		Math::Matrix		mP;			// 射影行列
		Math::Vector3		CamPos;		// カメラワールド座標

		int					DistanceFogEnable = 0;			// 距離フォグ有効
		Math::Vector3		DistanceFogColor = { 1, 1, 1 };	// 距離フォグ色
		float				DistanceFogDensity = 0.001f;	// 距離フォグ減衰率
	};

	// カメラ定数バッファ
	KdConstantBuffer<cbCamera>	m_cb7_Camera;


	//==========================
	//
	// ライト関係
	//
	//==========================

	// ライト定数バッファ用構造体
	struct cbLight
	{
		// 環境光
		Math::Vector3		AmbientLight = {0.3f, 0.3f, 0.3f};
		float					tmp;

		// 平行光
		Math::Vector3		DL_Dir = {0,-1,0};    // 光の方向
		float					tmp2;
		Math::Vector3		DL_Color = {1,1,1};  // 光の色
		float					tmp3;

	};

	// ライト定数バッファ
	KdConstantBuffer<cbLight>		m_cb8_Light;


	//==========================
	//
	// パイプラインステート
	//
	//==========================

	//深度ステンシル
	ID3D11DepthStencilState* m_ds_ZEnable_ZWhiteEnable = nullptr;
	ID3D11DepthStencilState* m_ds_ZDisable_ZWhiteDisable = nullptr;
	ID3D11DepthStencilState* m_ds_ZEnable_ZWriteDisable = nullptr;

	//ラスタライザステート
	ID3D11RasterizerState* m_rs_CullBack = nullptr;
	ID3D11RasterizerState* m_rs_CullNone = nullptr;

	//ブレンドステート
	ID3D11BlendState* m_bs_Alpha = nullptr;
	ID3D11BlendState* m_bs_Add = nullptr;


	//==========================
	//
	// その他
	//
	//==========================





//-------------------------------
// シングルトン
//-------------------------------
private:

	ShaderManager() {}

public:
	static ShaderManager &GetInstance() {
		static ShaderManager instance;
		return instance;
	}
};

#define SHADER ShaderManager::GetInstance()
