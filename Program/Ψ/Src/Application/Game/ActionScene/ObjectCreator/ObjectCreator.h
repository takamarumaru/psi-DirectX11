#pragma once

#include "Application/Game/GameObject.h"

class ObjectCreator :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	void ChildCreate();

	std::string m_childClassName = "GameObject";
	std::string m_childPrefab;

	std::weak_ptr<GameObject> m_wpChildObject;

};