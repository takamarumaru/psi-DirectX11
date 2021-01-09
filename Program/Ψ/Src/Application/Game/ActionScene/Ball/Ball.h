#pragma once

#include "../OperateObject.h"

class Ball :public OperateObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	//当たり判定更新
	void UpdateCollision();

	//回転の更新
	void UpdateRotate();

	//重力の強さ
	float m_gravity = 0.01f;

	Vector3 m_moveForce;

	bool m_isImpactGround = false;

};