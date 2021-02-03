#pragma once

#include "../InputObject.h"

enum TARGET_STATE
{
	TS_BASIC,
	REVERSE
};

class Target :public InputObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;
	virtual void Update()override;
	virtual void ImGuiUpdate()override;
	virtual bool GetIsPush()override;

private:

	//当たり判定更新
	void UpdateCollision();

	void LightOn();
	void LightOff();

	//復帰するまでの時間
	float m_returnTime = 1.0f;
	//開始時間
	std::chrono::system_clock::time_point m_startTime;
	//経過時間
	std::chrono::system_clock::time_point m_nowTime;

	//
	UINT m_stateTag = TARGET_STATE::TS_BASIC;
};