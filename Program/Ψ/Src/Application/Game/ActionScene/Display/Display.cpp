#include "Display.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"

void Display::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//行列から座標へ代入
	m_pos = m_mWorld.GetTranslation();

	//表示する画像をロード
	auto& rDisplayTex = jsonObj["DisplayTexList"].array_items();
	for (auto&& texture : rDisplayTex)
	{
		//画像の情報を生成
		DisplayTexInfo info;
		info.m_size		= Math::Vector2(texture[1].number_value(), texture[2].number_value());
		info.m_pos		= Math::Vector2(texture[3].number_value(), texture[4].number_value());
		info.m_angleZ = texture[5].number_value();
		info.m_texture.SetTexture(ResFac.GetTexture(texture[0].string_value()));
		info.m_texture.Init(info.m_size.x*0.01f, info.m_size.y * 0.01f, Math::Color(0,0,0,1));

		//リストに追加
		m_spDisplayTexList[texture[0].string_value()] = info;

	}
}

json11::Json::object Display::Serialize()
{
	json11::Json::object objectData = GameObject::Serialize();

	json11::Json::array displayTexList(m_spDisplayTexList.size());

	//レールのポイント
	UINT i = 0;
	for (auto&& displayTex : m_spDisplayTexList)
	{
		json11::Json::array display(6);
		display[0] = displayTex.first;
		display[1] = (int)displayTex.second.m_size.x;
		display[2] = (int)displayTex.second.m_size.y;
		display[3] = (int)displayTex.second.m_pos.x;
		display[4] = (int)displayTex.second.m_pos.y;
		display[5] = (int)displayTex.second.m_angleZ;

		displayTexList[i] = display;
		i++;
	}

	objectData["DisplayTexList"] = displayTexList;

	return objectData;
}

void Display::Update()
{
	GameObject::Update();

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}
}

void Display::DrawEffect()
{
	for (auto&& displayTex : m_spDisplayTexList)
	{
		Matrix drawMat=m_mWorld;
		drawMat.CreateRotationY(m_rot.y + (-90*ToRadians));
		drawMat.RotateAxis(m_mWorld.GetAxisX(),displayTex.second.m_angleZ * ToRadians);

		Matrix mTranse;
		Vector3 offset = m_mWorld.GetTranslation() + (m_mWorld.GetAxisX() * 0.22f);
		offset += m_mWorld.GetAxisZ() * (displayTex.second.m_pos.x * 0.01f);
		offset += m_mWorld.GetAxisY() * (displayTex.second.m_pos.y * 0.01f);
		mTranse.CreateTranslation(offset);

		drawMat *= mTranse;
		//描画
		SHADER.m_effectShader.SetWorldMatrix(drawMat);
		SHADER.m_effectShader.WriteToCB();
		displayTex.second.m_texture.Draw(0);
	}
}

void Display::ImGuiUpdate()
{
	GameObject::ImGuiUpdate();

	//TextureListを表示
	if (ImGui::CollapsingHeader("TextureList", ImGuiTreeNodeFlags_DefaultOpen))
	{
		UINT i = 0;
		for (auto&& displayTex : m_spDisplayTexList)
		{
			//1つのテクスチャの情報(変更可能)
			if (ImGui::TreeNodeEx(("Texture"+std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				//縦横比
				if (ImGui::DragFloat2(("Size" + std::to_string(i)).c_str(), &displayTex.second.m_size.x, 1.0f))
				{
					displayTex.second.m_texture.Init(displayTex.second.m_size.x*0.01f, displayTex.second.m_size.y * 0.01f, Math::Color(0, 0, 0, 1));
				}
				//オフセット座標
				ImGui::DragFloat2(("Offset" + std::to_string(i)).c_str(), &displayTex.second.m_pos.x, 1.0f);
				//Z軸回転角度
				ImGui::DragFloat(("AngleZ" + std::to_string(i)).c_str(), &displayTex.second.m_angleZ, 1.0f);

				ImGui::TreePop();
			}
			i++;
		}
	}

	ImGuiTextureFactoryUpdate();
}

void Display::ImGuiTextureFactoryUpdate()
{
	//オブジェクト生成
	if (ImGui::CollapsingHeader("TextureCreate", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//パス入力
		static std::string texturePath;
		ImGui::InputText("オブジェクトパス", &texturePath);

		//パス取得ボタン
		ImGui::SameLine();
		if (ImGui::Button(u8"textureパス取得"))
		{
			//エクスプローラを開く
			Window::OpenFileDialog(texturePath, "Textureファイルを開く", "Textureファイル\0*.png\0", "\\Data\\Texture");
		}

		//生成ボタン
		if (ImGui::Button("Create"))
		{
			DisplayTexInfo info;
			info.m_size = Math::Vector2(100, 100);
			info.m_pos = Math::Vector2(0, 0);
			info.m_angleZ = 0;
			info.m_texture.SetTexture(ResFac.GetTexture(texturePath));
			info.m_texture.Init(info.m_size.x * 0.01f, info.m_size.y * 0.01f, Math::Color(0, 0, 0, 1));

			//リストに追加
			m_spDisplayTexList[texturePath] = info;
		}
	}

	//テクスチャ削除
	if (ImGui::CollapsingHeader("TextureDelete", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}
