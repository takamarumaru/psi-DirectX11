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
	m_force.y -= m_gravity;

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
	float distanceFromGround = FLT_MAX;

	//下方向への判定を行い、着地した
	if (CheckGround(distanceFromGround,TAG_StageObject | TAG_Character))
	{
		//地面の上にｙ座標を移動
		m_pos.y += GameObject::s_allowToStepHeight - distanceFromGround;

		//地面があるので、ｙ方向への移動力は０に
		m_force.y = 0.0f;
	}
}