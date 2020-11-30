#include "Ball.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"

void Ball::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//行列から座標へ代入
	m_pos = m_mWorld.GetTranslation();
	m_rot = m_mWorld.GetAngles();

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
}

void Ball::UpdateCollision()
{
	//結果格納用
	float rayDistance = FLT_MAX;
	RayResult finalRayResult;

	//下方向への判定を行い、着地した
	if (CheckGround(finalRayResult, rayDistance,TAG_StageObject | TAG_Character, m_spOwner))
	{
		//地面の上にｙ座標を移動
		m_pos.y += GameObject::s_allowToStepHeight - rayDistance;
		m_force.y *= 0.7f;
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
	Vector3 force = m_force;
	force.y = 0.0f;

	//moveVecがZeroベクトルなら返る
	if (XMVector3Equal(moveVec, { 0.0f,0.0f,0.0f })){return;}

	//移動方向の右方向
	Vector3 rightVec = Vector3::Cross(moveVec,{ 0.0f,1.0f,0.0f });
	if (XMVector3Equal(rightVec, { 0.0f,0.0f,0.0f })) { return; }

	//移動方向の右方向を軸に回転した回転行列を作成
	Matrix mRotate = m_mWorld;
	mRotate.RotateAxis(rightVec, force.Length() / ((m_radius-0.3f) * M_PI));

	//IMGUI_LOG.Clear();
	//IMGUI_LOG.AddLog(u8"移動量:%.2f", force.Length());
	//IMGUI_LOG.AddLog(u8"直径:%.2f", (1.0f * M_PI));

	m_rot = mRotate.GetAngles();
}