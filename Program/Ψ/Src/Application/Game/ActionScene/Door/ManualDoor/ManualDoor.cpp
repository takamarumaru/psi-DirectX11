#include "ManualDoor.h"

#include "../../InputObject.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"
#include"./Application/Component/SoundComponent.h"

void ManualDoor::Deserialize(const json11::Json& jsonObj)
{
	OutputObject::Deserialize(jsonObj);
}

void ManualDoor::Update()
{
	GameObject::Update();

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;

	//回転
	m_mWorld.CreateRotation(m_rot);
	//座標
	m_mWorld.Move(m_pos);

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	//オーナーからの信号がONなら開く
	if (m_animator.IsAnimationEnd())
	{
		if (SCENE.FindObjectWithName(m_ownerName))
		{
			m_wpOwner = SCENE.FindObjectWithName(m_ownerName);
			std::shared_ptr<InputObject> button = std::dynamic_pointer_cast<InputObject>(m_wpOwner.lock());
			if (button->GetIsPush())
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

	//アニメーションの更新
	m_animator.AdvanceTime(m_spModelComponent->GetChangeableNodes());
}