#pragma once

#include "./Application/Game/GameObject.h"

class AutomaticDoor :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;
	virtual void Update()override;

private:

	void UpdateCollision();

	bool m_isOpen = false;

};