#include "Box.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"

void Box::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//行列から座標へ代入
	m_pos = m_mWorld.GetTranslation();

}

void Box::Update()
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

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	//座標
	m_mWorld.CreateTranslation(m_pos.x, m_pos.y, m_pos.z);
}

void Box::UpdateCollision()
{
	//結果格納用
	float rayDistance = FLT_MAX;
	RayResult finalRayResult;

	//下方向への判定を行い、着地した
	if (CheckGround(finalRayResult, rayDistance,TAG_StageObject | TAG_Character))
	{
		//地面の上にｙ座標を移動
		m_pos.y += GameObject::s_allowToStepHeight - rayDistance;

		//地面があるので、ｙ方向への移動力は０に
		m_force.y = 0.0f;

		//地面の摩擦を加味
		m_force.x *= 0.8f;
		m_force.z *= 0.8f;
	}

	if (CheckBump({ 0,1.0f,0 }))
	{
		//摩擦による減速処理
		m_force.x *= 0.8f;
		m_force.z *= 0.8f;
	}
}