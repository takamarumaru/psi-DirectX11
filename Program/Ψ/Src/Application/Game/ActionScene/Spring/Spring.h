#pragma once

#include "../OutputObject.h"

class Spring :public OutputObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;
	virtual void Update()override;
	virtual void ImGuiUpdate()override;

private:

	//当たり判定更新
	void UpdateCollision();

	//オブジェクトと当たったか
	bool m_isHit = false;

	//押しだすまでの時間
	float m_pushTime = 0.0f;

	//押しだす力
	double m_pushPower = 0.0f;

};