#include "AutomaticDoor.h"

#include "../../InputObject.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"
#include"./Application/Component/SoundComponent.h"

void AutomaticDoor::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);
}

json11::Json::object AutomaticDoor::Serialize()
{
	json11::Json::object objectData = GameObject::Serialize();

	return objectData;
}

void AutomaticDoor::Update()
{
	GameObject::Update();

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;

	//回転
	m_mWorld.CreateRotation(m_rot);
	//座標
	m_mWorld.Move(m_pos);

	//カメラの更新
	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	//当たり判定更新
	UpdateCollision();

	//アニメーションの更新
	m_animator.AdvanceTime(m_spModelComponent->GetChangeableNodes());
}

//当たり判定更新
void AutomaticDoor::UpdateCollision()
{
	//球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_radius;

	for (auto& obj : SCENE.GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//TAG_Playerと球判定を行う
		if (!(obj->GetTag() & (TAG_Player))) { continue; }

		//球判定
		if (m_animator.IsAnimationEnd())
		{
			//プレイヤーが範囲内に入ったら開ける
			if (obj->HitCheckBySphere(info))
			{
				if (!m_isOpen)
				{
					SetAnimation("Open", false);
					m_isOpen = true;
					m_spSoundComponent->SoundPlay("Data/Sound/DoorOpen.wav");
				}
			}
			else
			{
				if (m_isOpen)
				{
					SetAnimation("Close", false);
					m_isOpen = false;
					m_spSoundComponent->SoundPlay("Data/Sound/DoorClose.wav");
				}
			}
		}
	}
}
