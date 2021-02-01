#include "PopUpTexture.h"
#include "Application/main.h"

void PopUpTexture::Draw2D()
{
	if (!m_isAlive)return;
	//ポイントの描画
	if (m_spPopUpTex)
	{
		RECT rClient;
		GetClientRect(APP.m_window.GetWndHandle(), &rClient);
		//2D描画
		SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
		Math::Color color(1,1,1,m_texAlpha);
		SHADER.m_spriteShader.DrawTex
		(
			m_spPopUpTex.get(),
			(rClient.right - rClient.left) / 2 * m_pos.x,
			(rClient.top - rClient.bottom) / 2 * m_pos.y,
			nullptr,
			&color
		);
	}
}

void PopUpTexture::Update()
{
	//フェード処理
	if (m_isVisible)
	{
		m_isFade = true;
		m_texAlpha += 0.05f;
		if (m_texAlpha >= 0.5f)
		{
			m_texAlpha = 0.5f;
		}
	}
	else
	{
		m_texAlpha -= 0.05f;
		if (m_texAlpha <= 0.0f)
		{
			m_texAlpha = 0.0f;
			m_isFade = false;
		}
	}

	if ((!m_isFade)&&(!m_isEnable))
	{
		m_isAlive = false;
	}
}
