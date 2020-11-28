#pragma once

#include "./Application/Game/GameObject.h"

class Light :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;
	virtual void Update()override;

private:

	//光の色
	Vector3 m_lightColor = {0,0,0};

	//光の強さ
	UINT m_lightPower = 0.0f;
};