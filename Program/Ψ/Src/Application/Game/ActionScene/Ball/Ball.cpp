#include "Ball.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"

void Ball::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);
}

void Ball::Update()
{
	GameObject::Update();

	//重力をキャラクターのYの移動力に変える
	if (m_isFall)
	{
		m_force.y -= m_gravity;
	}

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;
	m_pos += m_moveForce;

	//当たり判定
	UpdateCollision();

	//回転更新
	UpdateRotate();

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	//座標
	m_mWorld.CreateRotation(m_rot);
	m_mWorld.Move(m_pos.x,m_pos.y + 1.0f,m_pos.z);

	//自己発光をオンに
	m_spModelComponent->SetEmissive(true);
}

void Ball::UpdateCollision()
{
	//結果格納用
	float rayDistance = FLT_MAX;
	RayResult finalRayResult;

	//下方向への判定を行い、着地した
	if (CheckGround(finalRayResult, m_pos,rayDistance,TAG_StageObject | TAG_Character, m_spOwner))
	{
		//地面の上にｙ座標を移動
		m_pos.y += GameObject::s_allowToStepHeight - rayDistance;
		//摩擦による減速処理
		m_force.y *= 0.5f;

		if (Vector3(finalRayResult.m_polyDir.x,0.0f, finalRayResult.m_polyDir.z).Length() >= 0.1f)
		{
			//法線方向との壁ずりベクトル
			m_force += Vector3::WallScratch(Vector3(0,-1,0), finalRayResult.m_polyDir) * 0.005f;
		}
	}

	if (CheckBump(TAG_StageObject | TAG_Character,m_spOwner))
	{
		//摩擦による減速処理
		m_force *= 0.7f;
	}
}

//回転の更新
void Ball::UpdateRotate()
{

	//宙に浮いているなら返る
	if (!m_isGround) { return; }

	//移動方向
	Vector3 moveVec = m_prevPos - m_pos;
	//移動していないなら返る
	if (moveVec.Length() == 0.0f) { return; }

	//ベクトルの長さ
	Vector3 force = m_force + m_moveForce;
	force.y = 0.0f;

	//moveVecがZeroベクトルなら返る
	if (XMVector3Equal(moveVec, { 0.0f,0.0f,0.0f })){return;}

	//移動方向の右方向
	Vector3 rightVec = Vector3::Cross(moveVec,{ 0.0f,1.0f,0.0f });
	if (XMVector3Equal(rightVec, { 0.0f,0.0f,0.0f })) { return; }

	//移動方向の右方向を軸に回転した回転行列を作成
	Matrix mRotate = m_mWorld;
	mRotate.RotateAxis(rightVec, force.Length() / ((m_radius-0.3f) * M_PI));

	m_rot = mRotate.GetAngles();
}