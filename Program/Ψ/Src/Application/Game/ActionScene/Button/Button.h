#pragma once

#include "../InputObject.h"

class Button :public InputObject
{
public:
	virtual void Update()override;
private:
	//当たり判定更新
	void UpdateCollision();
};