#pragma once

#include "./Application/Game/GameObject.h"

class Spring :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	//当たり判定更新
	void UpdateCollision();

	//オブジェクトと当たったか
	bool m_isHit = false;

	//押しだすまでの時間
	float m_pushTime = 0.0f;

	//押しだす力
	float m_pushPower = 0.0f;

	//オーナー情報
	std::string m_ownerName;
	std::weak_ptr<GameObject> m_wpOwner;

};