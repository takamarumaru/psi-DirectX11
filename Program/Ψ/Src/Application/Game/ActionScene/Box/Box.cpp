#include "Box.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"
#include"./Application/Component/SoundComponent.h"

void Box::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//自己発光をオンに
	m_spModelComponent->SetEmissive(true);
}

void Box::Update()
{
	GameObject::Update();

	//カメラの更新
	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	//重力をキャラクターのYの移動力に変える
	if (m_isFall)
	{
		m_force.y -= m_gravity;
	}

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;

	//当たり判定
	UpdateCollision();

	//座標
	m_mWorld.CreateTranslation(m_pos.x, m_pos.y, m_pos.z);

}

void Box::UpdateCollision()
{
	//結果格納用
	float rayDistance = FLT_MAX;
	RayResult finalRayResult;
	bool isHitGround = false;

	Vector3 uv[4] =
	{
		Vector3(0.8f,0,0.8f),
		Vector3(0.8f,0,-0.8f),
		Vector3(-0.8f,0,0.8f),
		Vector3(-0.8f,0,-0.8f)
	};
	for (UINT i = 0; i < 4; i++)
	{
		//下方向への判定を行い、着地した
		if (CheckGround(finalRayResult, m_pos+uv[i], rayDistance, TAG_StageObject | TAG_Character, m_spOwner))
		{
			//地面の上にｙ座標を移動
			m_pos.y += GameObject::s_allowToStepHeight - rayDistance;
			isHitGround = true;
		}
	}

	//衝撃音を出す
	if (isHitGround)
	{
		if (m_isImpactGround == false)
		{
			m_isImpactGround = true;
			//力によって音量を調整
			m_spSoundComponent->SetStateVolume(CorrectionValue(m_force.Length() / 0.1f, 1.0f, 0.0f));
			//再生
			m_spSoundComponent->SoundPlay("Data/Sound/Impact.wav");
		}
	}
	else
	{
		m_isImpactGround = false;
	}


	if (CheckBump(TAG_StageObject | TAG_Character, TAG_Player,m_spOwner))
	{
		//摩擦による減速処理
		m_force *= 0.2f;
	}

}