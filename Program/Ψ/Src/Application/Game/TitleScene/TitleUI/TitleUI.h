#pragma once

#include"../../GameObject.h"

//画面上に出す画像のデータ
struct Item
{
	std::shared_ptr<Texture> m_spTexture;	//画像
	bool m_isHit = false;					//当たり判定
	POINT m_pos;							//座標
	Math::Rectangle m_rect;					//RECT
	std::string m_name;						//名前
};

class TitleUI :public GameObject
{
public:
	TitleUI() {}
	virtual ~TitleUI() {}

	//初期化
	void Deserialize(const json11::Json& jsonObj)override;
	//2D描画
	void Draw2D()override;
	//更新
	void Update()override;
private:

	//タイトルロゴ
	Item m_logo;

	//メニュー背景
	std::shared_ptr<Texture> m_spMenuBackTex;

	//メニュー
	std::vector<Item> m_menu;

	//マウスポインタとの当たり判定
	bool HitCheckByRect(RECT rect);

};