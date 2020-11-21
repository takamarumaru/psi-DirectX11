#pragma once

#include "../GameObject.h"

class OperateObject :public GameObject
{
public:

	OperateObject() {}
	virtual ~OperateObject() {}

	void SetOwner(std::shared_ptr<GameObject>& rOwner) { m_spOwner = rOwner; }
	void ClearOwner() { m_spOwner = nullptr; }

protected:

	//オーナー
	std::shared_ptr<GameObject> m_spOwner = nullptr;
};