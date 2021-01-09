#pragma once

#include "./Application/Game/GameObject.h"
#include "../OperateObject.h"

class TextureEffect;

class Player :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;
	virtual void DrawEffect()override;
	virtual void Draw2D()override;

private:

	//移動更新
	void UpdateMove();
	void UpdateRotate(const Vector3& rMoveDir);
	void UpdateCamera();

	//アクション更新
	void UpdateGrab();
	void UpdatePowerEffect(const Vector3 rStart, const Vector3 rEnd, const Vector3 rTarget);
	void OperateReset();

	//当たり判定更新
	void UpdateCollision();

	//重力の強さ
	float m_gravity = 0.01f;
	//判定距離
	float m_radius = 0.5f;

	//ジャンプ力
	float m_jumpPow = 0.2f;
	//移動速度
	float m_moveSpeed = 0.15f;
	float m_cameraRotSpeed = 0.15f;
	//キー移動
	Vector3 m_moveForce;
	//回転速度
	float m_rotateAngle = 10.0f;

	//オブジェクトを操作中か
	bool m_isOperate = false;

	//操作するオブジェクト======================

	std::shared_ptr<OperateObject> m_spOperateObj = nullptr;
	//距離制限
	float m_operateObjMaxDst = 10.0f;
	float m_operateObjMinDst = 3.0f;
	//追尾速度
	float m_operateObjTrackingSpeed = 0.095f;
	//追尾する最高速度
	float m_operateObjTrackingMaxSpeed = 0.5f;
	//マウスホイールによる移動速度
	float m_operateObjAroundSpeed = 0.01f;
	//発射するときの力
	float m_operateObjShotPower = 0.5f;

	//念　軌跡
	TrailPolygon	m_powerEffect;
	//ポインタ
	std::shared_ptr<Texture> m_spPointTex;

	//==========================================

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
	void ShiftWait() 
	{
		m_spActionState = std::make_shared<WaitAction>(); 
	}
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