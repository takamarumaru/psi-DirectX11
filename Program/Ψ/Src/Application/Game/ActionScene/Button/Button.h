#pragma once

#include "../InputObject.h"

class Button :public InputObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual json11::Json::object Serialize()override;
	virtual void Update()override;
	virtual void DrawEffect()override;

private:

	//当たり判定更新
	void UpdateCollision();

	//出力オブジェクトまでの軌跡
	TrailPolygon	m_rail;

};