#include "ManualDoor.h"

#include "../../InputObject.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"

void ManualDoor::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//行列から代入
	m_pos = m_mWorld.GetTranslation();
	m_rot = m_mWorld.GetAngles();
	//オーナーの名前
	if (jsonObj["OwnerName"].is_null() == false)
	{
		m_ownerName = jsonObj["OwnerName"].string_value();
	}
}

json11::Json::object ManualDoor::Serialize()
{
	json11::Json::object objectData = GameObject::Serialize();

	objectData["OwnerName"] = m_ownerName;

	return objectData;
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
		m_wpOwner = SCENE.FindObjectWithName(m_ownerName);
		std::shared_ptr<InputObject> button = std::dynamic_pointer_cast<InputObject>(m_wpOwner.lock());
		if (button->GetIsPush())
		{
			if (!m_isOpen)
			{
				SetAnimation("Open", false);
				m_isOpen = true;
			}
		}
		else
		{
			if (m_isOpen)
			{
				SetAnimation("Close", false);
				m_isOpen = false;
			}
		}
	}

	//アニメーションの更新
	m_animator.AdvanceTime(m_spModelComponent->GetChangeableNodes());
}