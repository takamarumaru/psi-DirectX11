#include "Goal.h"

#include"./Application/Game/Scene.h"

#include"./Application/Component/CameraComponent.h"

void Goal::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//行列から座標へ代入
	m_pos = m_mWorld.GetTranslation();

}

void Goal::Update()
{
	GameObject::Update();

	//当たり判定
	UpdateCollision();

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	//座標
	m_mWorld.CreateTranslation(m_pos.x, m_pos.y, m_pos.z);
}

void Goal::UpdateCollision()
{
	//球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_radius;

	//デバック表示
	SCENE.AddDebugSphereLine(info.m_pos, info.m_radius, {1,0,1,1});

	for (auto& obj : SCENE.GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//TAG_Playerと球判定を行う
		if (!(obj->GetTag() & (TAG_Player))) { continue; }

		//球判定
		if (m_animator.IsAnimationEnd())
		{
			//プレイヤーが範囲内に入ったらシーン変更
			if (obj->HitCheckBySphere(info))
			{
				//最後のステージならタイトルに移動(デバッグ)
				if (SCENE.GetSceneNo()==10)
				{
					SCENE.RequestChangeScene("Data/JsonData/TitleScene.json");
				}

				//次のシーンに移動
				SCENE.RequestChangeScene("Data/JsonData/Area"+ std::to_string(SCENE.GetSceneNo()+1)+"Scene.json");
			}
		}
	}
}