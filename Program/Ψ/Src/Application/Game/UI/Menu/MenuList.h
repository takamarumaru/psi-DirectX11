#pragma once

#include"../../GameObject.h"

enum MENU_STATE
{
	MS_NONE,
	MS_TITLE,
	MS_ACTION,
};

//メニューのデータ
struct ListItem
{
	std::shared_ptr<Texture> m_spTexture;	//画像
	bool m_isHit = false;					//当たり判定
	POINT m_pos;							//座標
	Math::Rectangle m_rect;					//RECT
	int m_space = 0;
	std::string m_name;						//名前
	bool m_canPlaySelectSE = false;			//SE連続防止用フラグ
};

class MenuList:public GameObject
{
public:
	//初期化
	void Deserialize(const json11::Json& jsonObj)override;
	//2D描画
	void Draw2D()override;
	//更新
	void Update()override;

private:

	//メニュー背景
	std::shared_ptr<Texture> m_spMenuBackTex;
	//メニュー
	std::vector<ListItem> m_itemList;

	//マウスポインタとの当たり判定
	bool HitCheckByRect(RECT rect);

	bool m_isVisible = true;

///ステート管理=====================================
	//基底ステート
	class BaseState
	{
	public:
		virtual void Deserialize(MenuList& rOwner, const json11::Json& jsonObj) = 0;
		virtual void Draw2D(MenuList& rOwner) = 0;
		virtual void Update(MenuList& rOwner) = 0;
	};

	//タイトルシーンのメニュー
	class TitleSceneState :public BaseState
	{
	public:
		virtual void Deserialize(MenuList& rOwner, const json11::Json& jsonObj)override;
		virtual void Draw2D(MenuList& rOwner)override {};
		virtual void Update(MenuList& rOwner)override {};
	};

	//アクションシーンのメニュー
	class ActionSceneState :public BaseState
	{
	public:
		virtual void Deserialize(MenuList& rOwner, const json11::Json& jsonObj)override;
		virtual void Draw2D(MenuList& rOwner)override;
		virtual void Update(MenuList& rOwner)override;
	private:
		//メニューテキスト
		std::shared_ptr<Texture> m_spMenuTex;
	};

	//アクションステート
	std::shared_ptr<BaseState> m_spState;
	//ステートタグ
	UINT m_stateTag = MS_NONE;
};