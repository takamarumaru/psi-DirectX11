#include "Button.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"
#include"./Application/Component/SoundComponent.h"
#include"./Application/Component/ModelComponent.h"

void Button::Update()
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
		Matrix mat;
		mat.CreateTranslation(m_mWorld.GetTranslation().x, m_mWorld.GetTranslation().y+0.1f, m_mWorld.GetTranslation().z);
		info.m_matrix = mat;



		if (obj->HitCheckByBox(info))
		{
			if (m_isPush == false)
			{
				SetAnimation("On", false);
				m_rail.SetTexture(ResFac.GetTexture("Data/Texture/railOn.png"));
				m_isPush = true;
				m_spModelComponent->SetEmissive(true);
				//起動音を再生
				m_spSoundComponent->SoundPlay("Data/Sound/ButtonOn.wav");
			}
			return;
		}
	}
	if (m_isPush == true)
	{
		SetAnimation("Off", false);
		m_rail.SetTexture(ResFac.GetTexture("Data/Texture/railOff.png"));
		m_isPush = false;
		m_spModelComponent->SetEmissive(false);
	}
}