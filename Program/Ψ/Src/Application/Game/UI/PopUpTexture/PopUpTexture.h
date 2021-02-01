#pragma once

#include"../../GameObject.h"

class PopUpTexture :public GameObject
{
public:
	//2D描画
	void Draw2D()override;
	//更新
	void Update()override;

	//テクスチャセット
	inline void SetTexture(std::shared_ptr<Texture> tex) { m_spPopUpTex = tex; }
	//表示非表示のセット
	inline void SetVisible(bool visible) { m_isVisible = visible; }
	//有効無効のセット
	inline void SetEnable(bool enable) { m_isEnable = enable; }

private:
	//表示するか
	bool m_isVisible = false;
	//有効か
	bool m_isEnable = true;
	//フェード中か
	bool m_isFade = false;

	//画像
	std::shared_ptr<Texture> m_spPopUpTex;
	//画像の不透明度
	float m_texAlpha = 0.0f;
};