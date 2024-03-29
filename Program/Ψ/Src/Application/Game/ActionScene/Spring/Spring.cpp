﻿#include "Spring.h"

#include "../InputObject.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"
#include"./Application/Component/SoundComponent.h"

void Spring::Deserialize(const json11::Json& jsonObj)
{
	OutputObject::Deserialize(jsonObj);

	//押し出す時間
	if (jsonObj["PushTime"].is_null() == false)
	{
		m_pushTime = jsonObj["PushTime"].number_value();
	}

	//押し出す力
	if (jsonObj["PushPower"].is_null() == false)
	{
		m_pushPower = jsonObj["PushPower"].number_value() / 100.0f;
	}


	//ステートを決定
	if (jsonObj["State"].is_null() == false)
	{
		m_stateTag = (int)jsonObj["State"].number_value();
	}

}

json11::Json::object Spring::Serialize()
{
	json11::Json::object objectData = OutputObject::Serialize();

	//押し出す力
	objectData["PushPower"] = (int)(m_pushPower * 100.0f);
	objectData["State"] = (int)m_stateTag;

	return objectData;
}

void Spring::Update()
{
	GameObject::Update();

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;

	//当たり判定
	UpdateCollision();

	//回転
	m_mWorld.CreateRotation(m_rot);
	//座標
	m_mWorld.Move(m_pos);

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	//オーナーからの信号がONなら動く
	if (m_animator.IsAnimationEnd())
	{
		if (m_stateTag == SPRING_STATE::BASIC)
		{
			if (SCENE.FindObjectWithName(m_ownerName))
			{
				m_wpOwner = SCENE.FindObjectWithName(m_ownerName);

				std::shared_ptr<InputObject> button = std::dynamic_pointer_cast<InputObject>(m_wpOwner.lock());
				if (button->GetIsPush())
				{
					SetAnimation("Piston", false);
					//自己発光をオンに
					m_spModelComponent->SetEmissive(true);
					m_spSoundComponent->SoundPlay("Data/Sound/Spring.wav");
				}
				else
				{
					//自己発光をオフに
					m_spModelComponent->SetEmissive(false);
				}
			}
		}
		else if (m_stateTag == SPRING_STATE::FOREVER)
		{
			SetAnimation("Piston", false);
			//自己発光をオンに
			m_spModelComponent->SetEmissive(true);
			m_spSoundComponent->SoundPlay("Data/Sound/Spring.wav");
		}
	}

	
	

	//アニメーションの更新
	m_animator.AdvanceTime(m_spModelComponent->GetChangeableNodes());
}

void Spring::ImGuiUpdate()
{
	OutputObject::ImGuiUpdate();

	if (ImGui::TreeNodeEx("ChangePower", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float springPower = int(m_pushPower * 100.0f);

		bool isChange = false;

		isChange |= ImGui::DragFloat("PushPower", &springPower, 1.0f);

		if (isChange)
		{
			m_pushPower = springPower / 100.0f;
		}

		ImGui::TreePop();
	}
}

void Spring::UpdateCollision()
{
	//アニメーションタイムが(指定時間-1)から指定時間の間だけ実行
	if (!(m_animator.GetAnimTime() >= m_pushTime-1 && m_animator.GetAnimTime() <= m_pushTime)) { return; }
	//全てのオブジェクトと四角判定
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//TAG_Characterのみ
		if (!(obj->GetTag() & (TAG_Character))) { continue; }

		BoxInfo info;
		//ノード(Spring)と判定
		info.m_node = *m_spModelComponent->FindNode("Spring");
		//少し縦に伸ばした行列で判定
		Matrix mat = m_mWorld;
		mat.SetScale(m_mWorld.GetScale().x, m_mWorld.GetScale().y + 1.0f, m_mWorld.GetScale().z);
		info.m_matrix = mat;

		if (obj->HitCheckByBox(info))
		{
			//自分の上方向に飛ばす
			obj->SetForce(m_mWorld.GetAxisY() * m_pushPower);
		}
	}
}