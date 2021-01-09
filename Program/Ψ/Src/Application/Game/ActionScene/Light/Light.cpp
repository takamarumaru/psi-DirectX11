#include "Light.h"

#include "../InputObject.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"

void Light::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//光の強さ
	if (jsonObj["Power"].is_null() == false)
	{
		m_lightPower = (int)jsonObj["Power"].number_value();
	}

	//座標
	const std::vector<json11::Json>& rPos = jsonObj["Color"].array_items();
	if (rPos.size() == 3)
	{
		m_lightColor.x = (float)rPos[0].number_value();
		m_lightColor.y = (float)rPos[1].number_value();
		m_lightColor.z = (float)rPos[2].number_value();
	}

	//ステートを決定
	if (jsonObj["State"].is_null() == false)
	{
		m_stateTag = (int)jsonObj["State"].number_value();
	}

	switch (m_stateTag)
	{
	case LIGHT_STATE::LS_NORMAL:	m_spState = std::make_shared<NormalState>();break;
	case LIGHT_STATE::LS_BREAKING:	m_spState = std::make_shared<BreakingState>();break;
	case LIGHT_STATE::LS_SENSOR:	m_spState = std::make_shared<SensorState>();break;
	default:						m_spState = std::make_shared<NormalState>();break;
	}

	//状態によって初期化
	m_spState->Deserialize(*this,jsonObj);
}

json11::Json::object Light::Serialize()
{
	json11::Json::object objectData = GameObject::Serialize();

	objectData["Power"] = (int)m_lightPower;
	objectData["State"] = (int)m_stateTag;

	//座標
	json11::Json::array mat(3);
	mat[0] = m_lightColor.x;
	mat[1] = m_lightColor.y;
	mat[2] = m_lightColor.z;

	objectData["Color"] = mat;

	return objectData;
}

void Light::Update()
{
	GameObject::Update();

	//状態によって更新
	m_spState->Update(*this);

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;

	//回転
	m_mWorld.CreateRotation(m_rot);
	//座標
	m_mWorld.Move(m_pos);

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	if (m_spModelComponent->GetEmissive())
	{
		//点光を追加
		Vector3 pos = m_mWorld.GetTranslation();
		pos.y -= 1.0f;
		SHADER.AddPointLight(pos, m_lightPower, m_lightColor);
	}
}

void Light::ImGuiUpdate()
{
	GameObject::ImGuiUpdate();

	if (ImGui::TreeNodeEx("ChangeColor", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 lightColor = m_lightColor;

		bool isChange = false;

		isChange |= ImGui::DragFloat3("lightColor", &lightColor.x, 1.0f);

		if (isChange)
		{
			m_lightColor = lightColor;
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("ChangePower", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float lightPower = m_lightPower;

		bool isChange = false;

		isChange |= ImGui::DragFloat("lightPower", &lightPower, 1.0f);

		if (isChange)
		{
			m_lightPower = lightPower;
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("ChangeState", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float stateNum = m_stateTag;

		bool isChange = false;

		isChange |= ImGui::DragFloat("State", &stateNum, 1.0f);

		if (isChange)
		{
			m_stateTag = stateNum;
			switch (m_stateTag)
			{
			case LIGHT_STATE::LS_NORMAL:	m_spState = std::make_shared<NormalState>(); break;
			case LIGHT_STATE::LS_BREAKING:	m_spState = std::make_shared<BreakingState>(); break;
			case LIGHT_STATE::LS_SENSOR:	m_spState = std::make_shared<SensorState>(); break;
			default:						m_spState = std::make_shared<NormalState>(); break;
			}
		}

		ImGui::TreePop();
	}
}

