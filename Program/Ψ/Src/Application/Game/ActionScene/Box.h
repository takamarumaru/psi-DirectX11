#pragma once

#include "./Application/Game/GameObject.h"

class Box :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	//当たり判定更新
	void UpdateCollision();

	//重力の強さ
	float m_gravity = 0.01f;

};