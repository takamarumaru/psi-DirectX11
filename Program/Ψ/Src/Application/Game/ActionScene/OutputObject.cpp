#include "OutputObject.h"

void OutputObject::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//オーナーの名前
	if (jsonObj["OwnerName"].is_null() == false)
	{
		m_ownerName = jsonObj["OwnerName"].string_value();
	}

}

json11::Json::object OutputObject::Serialize()
{
	json11::Json::object objectData = GameObject::Serialize();

	//オーナー名
	objectData["OwnerName"] = m_ownerName;

	return objectData;
}

void OutputObject::ImGuiUpdate()
{
	GameObject::ImGuiUpdate();

	if (ImGui::TreeNodeEx("ChangeOwner", ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::string nowOwnerName = "nowOwner : " + m_ownerName;
		ImGui::Text(nowOwnerName.c_str());

		static std::string ownerName = "";
		ImGui::InputText("newOwnerName", &ownerName);

		//適用ボタン
		if (ImGui::Button(u8"適用"))
		{
			m_ownerName = ownerName.c_str();
		}


		ImGui::TreePop();
	}
}
