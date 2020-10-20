#include "Button.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/ModelComponent.h"

void Button::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//行列から座標へ代入
	m_pos = m_mWorld.GetTranslation();
}

void Button::Update()
{
	GameObject::Update();

	//移動力をキャラクターの座標に足しこむ
	m_pos += m_force;

	//当たり判定
	UpdateCollision();

	//座標
	m_mWorld.CreateTranslation(m_pos.x, m_pos.y, m_pos.z);

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	//アニメーションの更新
	m_animator.AdvanceTime(m_spModelComponent->GetChangeableNodes());
}

void Button::UpdateCollision()
{
	//全てのオブジェクトと四角判定
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//TAG_Characterのみ
		if (!(obj->GetTag() & (TAG_Character))) { continue; }

		BoxInfo info;
		//ノード(Button)と判定
		info.m_node = *m_spModelComponent->FindNode("Button");
		//少し縦に伸ばした行列で判定
		Matrix mat = m_mWorld;
		mat.SetScale(m_mWorld.GetScale().x, m_mWorld.GetScale().y+1.0f, m_mWorld.GetScale().z);
		info.m_matrix = mat;

		if (obj->HitCheckByBox(info))
		{
			if (m_isPush == false)
			{
				SetAnimation("On", false);
				m_isPush = true;
			}
			return;
		}
	}
	if (m_isPush == true)
	{
		SetAnimation("Off", false);
		m_isPush = false;
	}
}