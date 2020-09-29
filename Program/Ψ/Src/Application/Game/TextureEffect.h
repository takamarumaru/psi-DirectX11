#pragma once

#include"./GameObject.h"

class TextureEffect :public GameObject
{
public:
	TextureEffect();
	~TextureEffect();

	//アニメーション情報の設定
	void SetEffectInfo(const std::shared_ptr<Texture>& tex, float size, float splitX, float splitY, float angle,bool isAnim, bool isBillboard);

	//アニメーションの更新
	virtual void Update()override;
	
	//半透明物の描画
	virtual void DrawEffect()override;

private:
	//アニメーションをするかどうか
	bool m_isAnim = false;
	//ビルボードするかどうか
	bool m_isBillboard = false;

	//四角形ポリゴン
	SquarePolygon		m_poly;

	//Z軸の回転
	float				m_angleZ = 0;
};