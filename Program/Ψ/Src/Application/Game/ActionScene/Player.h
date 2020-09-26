#pragma once

#include "./Application/Game/GameObject.h"

class Player :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	//移動更新
	void UpdateMove();
	void UpdateCamera();

	//当たり判定更新
	void UpdateCollision();

	//重力の強さ
	float m_gravity = 0.01f;

	//移動速度
	float m_moveSpeed = 0.1f;

//============================================
//ステート関係
//============================================

	//基底アクションステート
	class BaseAction
	{
	public:
		virtual void Update(Player& rOwner) = 0;
	};

	//待機
	class WaitAction:public BaseAction
	{
	public:
		virtual void Update(Player& rOwner)override;
	};

	//移動
	class WalkAction :public BaseAction
	{
	public:
		virtual void Update(Player& rOwner)override;
	};

	//待機ステートに切り替え
	void ShiftWait() { m_spActionState = std::make_shared<WaitAction>(); }
	//移動ステートに切り替え
	void ShiftWalk() { m_spActionState = std::make_shared<WalkAction>(); }

	//移動ステートへの移行条件を満たしているか
	bool IsShiftWalk();

	//アクションステート
	std::shared_ptr<BaseAction> m_spActionState;
};