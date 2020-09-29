#include "TextureEffect.h"


TextureEffect::TextureEffect(){}

TextureEffect::~TextureEffect(){}


void TextureEffect::SetEffectInfo(const std::shared_ptr<Texture>& rTex, float size, float splitX, float splitY, float angle, bool isAnim, bool isBillboard)
{
	//ポリゴンの色と大きさを設定
	m_poly.Init(size, size, { 1,1,1,1 });
	//アニメーションの分割数を設定
	m_poly.SetAnimationInfo(splitX, splitY);
	//アニメーションの位置を０コマ目に移動しておく
	m_poly.SetAnimationPos(0);
	//渡されたテクスチャを設定する
	m_poly.SetTexture(rTex);
	//Z軸の回転角度を覚える
	m_angleZ = angle;

	//アニメーションの是非
	m_isAnim = isAnim;
	//ビルボードの是非
	m_isBillboard = isBillboard;
}

void TextureEffect::Update()
{
	//アニメーションしない場合は返る
	if (!m_isAnim) { return; }

	//アニメション進行
	m_poly.Animation(0.5f, false);
	//アニメ終了
	if (m_poly.IsAnimationEnd())
	{
		Destroy();
	}
}

void TextureEffect::DrawEffect()
{
	//各軸の拡大率を取得
	float scaleX = m_mWorld.GetAxisX().Length();
	float scaleY = m_mWorld.GetAxisY().Length();
	float scaleZ = m_mWorld.GetAxisZ().Length();

	
	Matrix drawMat;
	drawMat.CreateScalling(scaleX, scaleY, scaleZ);
	drawMat.RotateZ(m_angleZ * ToRadians);

	if (m_isBillboard)
	{
		//ビルボード処理
		drawMat.SetBillboard(SHADER.m_cb7_Camera.GetWork().mV);
		////座標は自分のものを使う
		drawMat.SetTranslation(m_mWorld.GetTranslation());
	}

	//描画
	SHADER.m_effectShader.SetWorldMatrix(m_mWorld);
	SHADER.m_effectShader.WriteToCB();
	m_poly.Draw(0);
}
