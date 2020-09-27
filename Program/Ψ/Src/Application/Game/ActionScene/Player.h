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
	void UpdateRotate(const Vector3& rMoveDir);
	void UpdateCamera();

	//当たり判定更新
	void UpdateCollision();

	//重力の強さ
	float m_gravity = 0.01f;

	//ジャンプ力
	float m_jumpPow = 0.2f;

	//移動速度
	float m_moveSpeed = 0.15f;
	float m_cameraRotSpeed = 0.15f;

	//回転速度
	float m_rotateAngle = 10.0f;

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

	//ジャンプ
	class JumpAction :public BaseAction
	{
	public:
		virtual void Update(Player& rOwner)override;
	};

	//待機ステートに切り替え
	void ShiftWait() { m_spActionState = std::make_shared<WaitAction>(); }
	//移動ステートに切り替え
	void ShiftWalk() { m_spActionState = std::make_shared<WalkAction>(); }
	//移動ステートに切り替え
	void ShiftJump() 
	{
		m_force.y += m_jumpPow;
		m_spActionState = std::make_shared<JumpAction>(); 
	}

	//移動ステートへの移行条件を満たしているか
	bool IsShiftWalk();
	//ジャンプステートへの移行条件を満たしているか
	bool IsShiftJump();

	//アクションステート
	std::shared_ptr<BaseAction> m_spActionState;
};