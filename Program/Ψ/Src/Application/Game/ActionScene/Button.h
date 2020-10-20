#pragma once

#include "./Application/Game/GameObject.h"

class Button :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

	inline bool GetIsPush() { return m_isPush; }

private:

	//当たり判定更新
	void UpdateCollision();

	bool m_isPush = false;

};