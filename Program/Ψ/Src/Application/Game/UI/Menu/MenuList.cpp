#include "MenuList.h"

#include "../../../main.h"
#include "../../Scene.h"

#include"Application/Component/InputComponent.h"
#include"Application/Component/SoundComponent.h"

void MenuList::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//入力用コンポーネントセット
	m_spInputComponent = std::make_shared<MenuListInputComponent>(*this);

	//メニューの項目背景の読み込み
	if (jsonObj["MenuBackTex"].is_null() == false)
	{
		m_spMenuBackTex = ResFac.GetTexture(jsonObj["MenuBackTex"].string_value());
	}

	//アイテムの数分メニューに追加
	auto& rItemList = jsonObj["MenuList"].array_items();
	auto& rItemRect = jsonObj["MenuRect"].array_items();
	ListItem item;
	for (auto&& itemObj : rItemList)
	{
		item.m_name = itemObj.string_value();
		item.m_spTexture = ResFac.GetTexture(item.m_name);
		item.m_rect.x = rItemRect[0].int_value();
		item.m_rect.y = rItemRect[1].int_value();
		item.m_rect.width = rItemRect[2].int_value();
		item.m_rect.height = rItemRect[3].int_value();
		item.m_space = jsonObj["ListSpace"].int_value();
		m_itemList.push_back(item);
	}

	//タグを読み取る
	if (jsonObj["State"].is_null() == false)
	{
		m_stateTag = (int)jsonObj["State"].number_value();
	}

	//タグによってステートを決定
	switch (m_stateTag)
	{
	case MENU_STATE::MS_TITLE:
		m_spState = std::make_shared<TitleSceneState>();
		break;
	case MENU_STATE::MS_ACTION:
		m_spState = std::make_shared<ActionSceneState>();
		break;
	default:
		m_spState = std::make_shared<ActionSceneState>();
		break;
	}

	//状態によって初期化
	m_spState->Deserialize(*this, jsonObj);
}

void MenuList::Draw2D()
{
	//非表示設定なら返る
	if (!m_isVisible) { return; }

	//状態による描画
	m_spState->Draw2D(*this);

	//メニュー
	for (UINT i = 0; i < m_itemList.size(); i++)
	{
		Math::Rectangle rect = m_itemList[i].m_rect;
		POINT pos = m_itemList[i].m_pos;
		Math::Color color = Math::Color(1.0f, 1.0f, 1.0f, 1.0f);
		//マウスポインタと当たっていたら描画方法を変更
		//背景 → 画像のUVを伸ばし、位置を変更
		//文字 → 色を変更
		if (!m_itemList[i].m_isHit)
		{
			rect.x = 50;
			pos.x -= 25;
			color = Math::Color(0.75f, 0.75f, 0.75f, 1.0f);
		}
		//背景
		SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
		SHADER.m_spriteShader.DrawTex(m_spMenuBackTex.get(), pos.x, pos.y, &rect);
		//文字
		SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
		SHADER.m_spriteShader.DrawTex(m_itemList[i].m_spTexture.get(), m_itemList[i].m_pos.x, m_itemList[i].m_pos.y, nullptr, &color);

	}
}

void MenuList::Update()
{
	GameObject::Update();

	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	//状態による更新
	m_spState->Update(*this);

	//非表示設定なら返る
	if (!m_isVisible) { return; }

	//非表示設定なら返る
	if (SCENE.IsChangeScene()) { return; }


	//ウィンドウの座標を取得
	RECT rClient;
	GetClientRect(APP.m_window.GetWndHandle(), &rClient);

	for (UINT i = 0; i < m_itemList.size(); i++)
	{
		//各アイテムの座標を指定
		m_itemList[i].m_pos.x = (rClient.right - rClient.left)/2 * (m_pos.x * 0.01f);
		m_itemList[i].m_pos.y = (rClient.top - rClient.bottom)/2 * (m_pos.y * 0.01f) - (m_itemList[i].m_rect.height + m_itemList[i].m_space) * i;
		//RECTの生成
		RECT rect;
		rect.left = m_itemList[i].m_pos.x - m_itemList[i].m_rect.width / 2;
		rect.right = m_itemList[i].m_pos.x + m_itemList[i].m_rect.width / 2;
		rect.top = m_itemList[i].m_pos.y - m_itemList[i].m_rect.height / 2;
		rect.bottom = m_itemList[i].m_pos.y + m_itemList[i].m_rect.height / 2;

		//アイテムにセット
		m_itemList[i].m_isHit = HitCheckByRect(rect);

		if (m_itemList[i].m_isHit)
		{
			if (m_itemList[i].m_canPlaySelectSE)
			{
				m_spSoundComponent->SoundPlay("Data/Sound/ListSelect.wav");
				m_itemList[i].m_canPlaySelectSE = false;
			}
		}
		else
		{
			m_itemList[i].m_canPlaySelectSE = true;
		}
	}

	//左クリックで決定
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		for (auto item : m_itemList)
		{
			//マウスポインタが当たっていた時
			if (item.m_isHit)
			{
				//要素がGameStartのとき
				if (item.m_name.find("GameStart") != std::string::npos)
				{
					//アクションシーンに移動
					Scene::GetInstance().RequestChangeScene("Data/JsonData/Area1Scene.json");
				}

				//要素がRetryのとき
				if (item.m_name.find("Retry") != std::string::npos)
				{
					//現在のシーンをやり直す
					Scene::GetInstance().RequestChangeScene("Data/JsonData/Area"+std::to_string(SCENE.GetSceneNo())+"Scene.json");
				}

				//要素がStageSelectのとき
				if (item.m_name.find("AreaSelect") != std::string::npos)
				{
					//ステージセレクトシーンに移行
					Scene::GetInstance().RequestChangeScene("Data/JsonData/AreaSelectScene.json");
				}
				
				//要素がAreaのとき
				if (item.m_name.find("Area") != std::string::npos)
				{
					//指定のシーンに移動
					std::string num = item.m_name.substr(item.m_name.find_last_of("Area")+1, 2);
					Scene::GetInstance().RequestChangeScene("Data/JsonData/Area" + std::to_string(atoi(num.c_str())) + "Scene.json");
				}

				//要素がSettingsのとき
				if (item.m_name.find("Settings") != std::string::npos)
				{

				}

				//要素がBacktoTitleのとき
				if (item.m_name.find("BacktoTitle") != std::string::npos)
				{
					//アクションシーンに移動
					Scene::GetInstance().RequestChangeScene("Data/JsonData/TitleScene.json");
				}

				//要素がQuitGameのとき
				if (item.m_name.find("QuitGame") != std::string::npos)
				{
					//ゲームを終了
					APP.End();
				}
			}
		}
	}
}



bool MenuList::HitCheckByRect(RECT rect)
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

//タイトルシーンの場合
void MenuList::TitleSceneState::Deserialize(MenuList& rOwner, const json11::Json& jsonObj)
{
	rOwner.m_isVisible = true;
}

//アクションシーンの場合
void MenuList::ActionSceneState::Deserialize(MenuList& rOwner, const json11::Json& jsonObj)
{
	rOwner.m_isVisible = false;

	//メニューテキストの読み込み
	if (jsonObj["MenuTex"].is_null() == false)
	{
		m_spMenuTex = ResFac.GetTexture(jsonObj["MenuTex"].string_value());
	}
}

void MenuList::ActionSceneState::Draw2D(MenuList& rOwner)
{
	float alpha = 0.5f;
	//2D描画
	SHADER.m_spriteShader.SetMatrix(DirectX::XMMatrixIdentity());
	SHADER.m_spriteShader.DrawBox(0, 0, 1280 / 2, 720 / 2, &Math::Color(0, 0, 0, alpha));

	SHADER.m_spriteShader.DrawTex(m_spMenuTex.get(), -1280 / 2 + 250, 720 / 2 - 100);
}

void MenuList::ActionSceneState::Update(MenuList& rOwner)
{
	//シーン切り替え中は返る
	if (SCENE.IsChangeScene()) { return; }
	//切り替え処理
	if (rOwner.m_spInputComponent->GetButton(Input::Buttons::L1) & InputComponent::ENTER)
	{
		rOwner.m_isVisible = !rOwner.m_isVisible;
		if (rOwner.m_isVisible)
		{
			SCENE.Stop();
			//マウスカーソルを表示
			while (ShowCursor(true) < 0);
		}
		else
		{
			SCENE.Restart();
			//マウスカーソルを非表示
			while (ShowCursor(false) >= 0);
		}
	}
}
