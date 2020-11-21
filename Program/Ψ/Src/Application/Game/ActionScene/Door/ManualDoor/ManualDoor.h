#pragma once

#include "./Application/Game/GameObject.h"

class ManualDoor :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;
	virtual void Update()override;

private:

	bool m_isOpen = false;

	//オーナー情報
	std::string m_ownerName;
	std::weak_ptr<GameObject> m_wpOwner;

};