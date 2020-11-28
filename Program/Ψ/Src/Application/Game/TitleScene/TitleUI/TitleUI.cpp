#include "TitleUI.h"

#include "./Application/main.h"
#include "./Application/Game/Scene.h"

//初期化
void TitleUI::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//タイトルロゴの読み込み
	if (jsonObj["TitleLogoTex"].is_null() == false)
	{
		m_logo.m_spTexture = ResFac.GetTexture(jsonObj["TitleLogoTex"].string_value());
	}

	//メニューの項目背景の読み込み
	if (jsonObj["MenuBackTex"].is_null() == false)
	{
		m_spMenuBackTex = ResFac.GetTexture(jsonObj["MenuBackTex"].string_value());
	}

	//アイテムの数分メニューに追加
	auto& rItemList = jsonObj["StrTexList"].array_items();
	Item item;
	for (auto&& itemObj : rItemList)
	{
		item.m_name = itemObj.string_value();
		item.m_spTexture = ResFac.GetTexture(item.m_name);
		item.m_rect = { 0,0,400,50 };
		m_menu.push_back(item);
	}

}

//描画	
void TitleUI::Draw2D()
{

	//タイトルの描画
	if (m_logo.m_spTexture)
	{
		//2D描画
		SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
		SHADER.m_spriteShader.DrawTex
		(
			m_logo.m_spTexture.get(),
			m_logo.m_pos.x,
			m_logo.m_pos.y
		);
	}

	//メニュー
	for (UINT i=0;i<m_menu.size();i++)
	{
		Math::Rectangle rect=m_menu[i].m_rect;
		POINT pos = m_menu[i].m_pos;
		Math::Color color = Math::Color(1.0f, 1.0f, 1.0f, 1.0f);
		//マウスポインタと当たっていたら描画方法を変更
		//背景 → 画像のUVを伸ばし、位置を変更
		//文字 → 色を変更
		if (!m_menu[i].m_isHit)
		{
			rect.x = 50;
			pos.x -= 25;
			color = Math::Color(0.75f, 0.75f, 0.75f, 1.0f);
		}
		//背景
		SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
		SHADER.m_spriteShader.DrawTex(m_spMenuBackTex.get(),pos.x,pos.y,&rect);
		//文字
		SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
		SHADER.m_spriteShader.DrawTex(m_menu[i].m_spTexture.get(),m_menu[i].m_pos.x,m_menu[i].m_pos.y,nullptr,&color);

	}
}

//更新
void TitleUI::Update()
{
	//シーン変更中は実行しない
	if (SCENE.IsChangeScene()) { return; }

	//ウィンドウの座標を取得
	RECT rWindow;
	GetWindowRect(APP.m_window.GetWndHandle(), &rWindow);

	m_logo.m_pos.x = (rWindow.right - rWindow.left) / 2 / 2 - 50;
	m_logo.m_pos.y = 0;

	for (UINT i = 0; i < m_menu.size(); i++)
	{
		m_menu[i].m_pos.x = -(rWindow.right - rWindow.left) / 2 / 4 - 100;
		m_menu[i].m_pos.y = -(rWindow.bottom - rWindow.top) / 2 / 4 - 51 * i;
		//RECTの生成
		RECT rect;
		rect.left   = m_menu[i].m_pos.x - m_menu[i].m_rect.width / 2;
		rect.right  = m_menu[i].m_pos.x + m_menu[i].m_rect.width / 2;
		rect.top    = m_menu[i].m_pos.y - m_menu[i].m_rect.height / 2;
		rect.bottom = m_menu[i].m_pos.y + m_menu[i].m_rect.height / 2;
		m_menu[i].m_isHit = HitCheckByRect(rect);
	}

	//左クリックで決定
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		for (auto item:m_menu)
		{
			//マウスポインタが当たっていた時
			if (item.m_isHit)
			{
				//要素がGameStartのとき
				if (item.m_name.find("GameStart") != std::string::npos)
				{
					//アクションゲームに移動
					Scene::GetInstance().RequestChangeScene("Data/JsonData/Stage1Scene.json");
				}

				//要素がQuitのとき
				if (item.m_name.find("Quit") != std::string::npos)
				{
					//ゲームを終了
					APP.End();
				}
			}
		}
	}
}

bool TitleUI::HitCheckByRect(RECT rect)
{
	//ウィンドウの座標を取得
	RECT rWindow;
	GetWindowRect(APP.m_window.GetWndHandle(), &rWindow);
	//マウスポイントの取得
	POINT nowMousePos;
	GetCursorPos(&nowMousePos);
	//テクスチャの座標系に変換
	nowMousePos.x -= rWindow.left + (rWindow.right - rWindow.left) / 2;
	nowMousePos.y -= rWindow.top + (rWindow.bottom - rWindow.top) / 2;
	nowMousePos.y = -nowMousePos.y;

	//渡されたRectに内包されていたらtrue
	if (nowMousePos.x >= rect.left && nowMousePos.x <= rect.right &&
		nowMousePos.y >= rect.top && nowMousePos.y <= rect.bottom)
	{
		return true;
	}
	return false;
}
