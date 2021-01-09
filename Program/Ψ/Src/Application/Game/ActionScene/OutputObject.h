#pragma once

#include "../GameObject.h"

class OutputObject :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;
	virtual void ImGuiUpdate()override;//ImGui更新

protected:
	
	//オーナー情報
	std::string m_ownerName;
	std::weak_ptr<GameObject> m_wpOwner;
};