#include "System/System.h"

#include "ShaderManager.h"

void ShaderManager::Init()
{

	//============================================
	// シェーダ
	//============================================

	// 基本シェーダ
	m_standardShader.Init();
	// エフェクトシェーダ
	m_effectShader.Init();
	// 2Dスプライトシェーダ
	m_spriteShader.Init();

	//============================================
	// 定数バッファ
	//============================================

	// カメラ
	m_cb7_Camera.Create();
	D3D.GetDevContext()->VSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());

	// 初期ビュー行列
	m_cb7_Camera.Work().mV = Math::Matrix::CreateTranslation(0, 0, -5);
	m_cb7_Camera.Work().CamPos = m_cb7_Camera.Work().mV.Translation();
	m_cb7_Camera.Work().mV = m_cb7_Camera.Work().mV.Invert();

	// 初期射影行列
	m_cb7_Camera.Work().mP = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 1000);
	m_cb7_Camera.Write();

	// ライト
	m_cb8_Light.Create();
	m_cb8_Light.Write();
	D3D.GetDevContext()->VSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());

}

void ShaderManager::Release()
{
	m_standardShader.Release();
	m_effectShader.Release();
	m_spriteShader.Release();

	m_cb7_Camera.Release();
	m_cb8_Light.Release();

}
