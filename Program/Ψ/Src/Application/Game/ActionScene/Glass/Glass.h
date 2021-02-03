#pragma once

#include "Application/Game/GameObject.h"

class Glass :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;
	virtual void DrawEffect()override;
	virtual void Draw()override {};

private:

	Vector3 m_rimColor;		//リムカラー
	float m_count = 0;		//進み続けるカウント
};