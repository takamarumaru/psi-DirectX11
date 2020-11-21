#pragma once

#include "../OperateObject.h"

class Goal :public OperateObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	//当たり判定更新
	void UpdateCollision();

};